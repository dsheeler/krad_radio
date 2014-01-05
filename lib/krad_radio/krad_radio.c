#include "krad_radio.h"
#include "krad_radio_server.h"
#include "krad_radio_internal.h"

static void radio_shutdown(kr_radio *radio);
static kr_radio *radio_create(char *sysname);
static void radio_start(kr_radio *radio);
static void radio_exist(kr_radio *radio);
static void radio_cpu_monitor_callback(kr_radio *radio, uint32_t usage);

static void radio_shutdown(kr_radio *radio) {
  kr_timer *timer;
  timer = kr_timer_create_with_name("shutdown");
  kr_timer_start(timer);
  krad_system_monitor_cpu_off();
  kr_timer_status(timer);
  if (radio->system_broadcaster != NULL) {
    krad_app_server_broadcaster_unregister(&radio->system_broadcaster);
  }
  if (radio->app != NULL) {
    krad_app_server_disable(radio->app);
  }
  kr_timer_status(timer);
  if (radio->remote.osc != NULL) {
    krad_osc_destroy(radio->remote.osc);
    radio->remote.osc = NULL;
  }
  kr_timer_status(timer);
  if (radio->remote.interweb != NULL) {
    krad_interweb_server_destroy(&radio->remote.interweb);
  }
  kr_timer_status(timer);
  if (radio->transponder != NULL) {
    kr_transponder_destroy(radio->transponder);
    radio->transponder = NULL;
  }
  kr_timer_status(timer);
  if (radio->mixer != NULL) {
    kr_mixer_destroy(radio->mixer);
    radio->mixer = NULL;
  }
  kr_timer_status(timer);
  if (radio->compositor != NULL) {
    kr_compositor_destroy(radio->compositor);
    radio->compositor = NULL;
  }
  kr_timer_status(timer);
  if (radio->tags != NULL) {
    krad_tags_destroy(radio->tags);
    radio->tags = NULL;
  }
  if (radio->app != NULL) {
    krad_app_server_destroy(radio->app);
    radio->app = NULL;
  }
  if (radio->log.startup_timer != NULL) {
    kr_timer_destroy(radio->log.startup_timer);
    radio->log.startup_timer = NULL;
  }
  if (timer != NULL) {
    kr_timer_finish(timer);
    printk("Krad Radio took %"PRIu64"ms to shutdown",
           kr_timer_duration_ms(timer));
    kr_timer_destroy(timer);
    timer = NULL;
  }
  free(radio);
  printk("Krad Radio exited cleanly");
  krad_system_log_off();
}

static void radio_mixer_info_cb(kr_mixer_info_cb_arg *arg) {
  printk("got a mixer info cb");
}

static void radio_mixer_bus_info_cb(kr_mixer_path_info_cb_arg *arg) {
  printk("got a mixer bus info cb");
}

static void kr_mixer_masterbus_setup(kr_radio *radio, kr_mixer *mixer) {

  kr_mixer_path *masterbus;
  kr_mixer_path_setup mbs;

  memset(&mbs, 0, sizeof(kr_mixer_path_setup));
  mbs.info.type = KR_MXR_BUS;
  strncpy(mbs.info.name, "Master", sizeof(mbs.info.name));
  mbs.info.channels = 2;
  mbs.info.volume[0] = KR_MXR_DEF_MBUS_LVL;
  mbs.info.volume[1] = KR_MXR_DEF_MBUS_LVL;
  mbs.info_cb = radio_mixer_bus_info_cb;
  mbs.user = radio;

  masterbus = kr_mixer_mkpath(mixer, &mbs);
  if (masterbus == NULL) {
    printke("radio couldn't create mixer master bus");
  }
}

static kr_radio *radio_create(char *sysname) {

  kr_radio *radio;
  kr_mixer_setup mixer_setup;
  kr_compositor_setup compositor_setup;
  kr_transponder_setup transponder_setup;

  radio = calloc(1, sizeof(kr_radio));
  radio->log.startup_timer = kr_timer_create_with_name("startup");
  kr_timer_start(radio->log.startup_timer);
  strncpy(radio->sysname, sysname, sizeof(radio->sysname));
  radio->tags = krad_tags_create("station");
  if (radio->tags == NULL) {
    radio_shutdown(radio);
    return NULL;
  }

  radio->app = krad_app_server_create("krad_radio", radio->sysname,
   krad_radio_client_create, krad_radio_client_destroy,
   krad_radio_client_handler, radio);

  if (radio->app == NULL) {
    radio_shutdown(radio);
    return NULL;
  }

  kr_mixer_setup_init(&mixer_setup);
  mixer_setup.user = radio;
  mixer_setup.cb = radio_mixer_info_cb;
  radio->mixer = kr_mixer_create(&mixer_setup);
  if (radio->mixer == NULL) {
    radio_shutdown(radio);
    return NULL;
  }
  kr_mixer_masterbus_setup(radio, radio->mixer);

  kr_compositor_setup_init(&compositor_setup);
  //same as above and hrm also enable mix/comp from AS
  radio->compositor = kr_compositor_create(&compositor_setup);
  if (radio->compositor == NULL) {
    radio_shutdown(radio);
    return NULL;
  }

  transponder_setup.mixer = radio->mixer;
  transponder_setup.compositor = radio->compositor;
  radio->transponder = kr_transponder_create(&transponder_setup);
  if (radio->transponder == NULL) {
    radio_shutdown(radio);
    return NULL;
  }
  radio->remote.osc = krad_osc_create(radio->sysname);
  if (radio->remote.osc == NULL) {
    radio_shutdown(radio);
    return NULL;
  }

  radio->system_broadcaster = krad_app_server_broadcaster_register(radio->app);

  if (radio->system_broadcaster == NULL) {
    radio_shutdown(radio);
    return NULL;
  }

  krad_app_server_broadcaster_register_broadcast(radio->system_broadcaster,
   EBML_ID_KRAD_SYSTEM_BROADCAST);

  return radio;
}

static void radio_start(kr_radio *radio) {
  krad_system_monitor_cpu_on();
  krad_system_set_monitor_cpu_callback((void *)radio,
   (void (*)(void *, uint32_t))radio_cpu_monitor_callback);
  krad_app_server_run(radio->app);
  if (radio->log.startup_timer != NULL) {
    kr_timer_finish(radio->log.startup_timer);
  }
}

static void radio_exist(kr_radio *radio) {
  krad_system_daemon_wait();
}

static void radio_cpu_monitor_callback(kr_radio *radio, uint32_t usage) {

  unsigned char buffer[128];
  krad_broadcast_msg_t *msg;
  kr_ebml2_t ebml;
  kr_address_t address;
  unsigned char *message_loc;
  unsigned char *payload_loc;

  address.path.unit = KR_STATION;
  address.path.subunit.station_subunit = KR_CPU;

  kr_ebml2_set_buffer(&ebml, buffer, 128);
  krad_radio_address_to_ebml2(&ebml, &message_loc, &address);
  kr_ebml_pack_int32(&ebml, EBML_ID_KRAD_RADIO_MESSAGE_TYPE,
   EBML_ID_KRAD_SUBUNIT_INFO);
  kr_ebml2_start_element(&ebml, EBML_ID_KRAD_RADIO_MESSAGE_PAYLOAD,
   &payload_loc);
  kr_ebml_pack_int32(&ebml, EBML_ID_KRAD_RADIO_SYSTEM_CPU_USAGE, usage);
  kr_ebml2_finish_element(&ebml, payload_loc);
  kr_ebml2_finish_element(&ebml, message_loc);

  msg = krad_broadcast_msg_create(radio->system_broadcaster, buffer, ebml.pos);

  if (msg != NULL) {
    krad_app_server_broadcaster_broadcast(radio->system_broadcaster, &msg);
  }
}

static int kr_was_launched_by_systemd() {
  const char *e;
  e = getenv("NOTIFY_SOCKET");
  if (e) {
    return 1;
  }
  return 0;
}

static void kr_systemd_notify(const char *data) {
  const char *e;
  struct sockaddr_un un = { .sun_family = AF_UNIX };
  int fd;
  e = getenv("NOTIFY_SOCKET");
  if (e) {
    strncpy(un.sun_path, e, sizeof(un.sun_path));
    if (un.sun_path[0] == '@') {
      un.sun_path[0] = 0;
    }
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (fd < 0) return;
    sendto(fd, data, strlen(data), MSG_NOSIGNAL, (struct sockaddr*) &un,
     sizeof(un) - sizeof(un.sun_path) + strlen(e));
    close(fd);
  }
}

int kr_radio_daemon(char *sysname) {
  pid_t pid;
  kr_radio *radio;
  if (!krad_valid_sysname(sysname)) {
    return -1;
  }
  if (!kr_was_launched_by_systemd()) {
    pid = fork();
    if (pid < 0) {
      return -1;
    } else if (pid > 0) {
      return 0;
    }
    krad_system_daemonize();
  }
  krad_system_init();
  radio = radio_create(sysname);
  if (radio == NULL) {
    return -1;
  }
  radio_start(radio);
  if (kr_was_launched_by_systemd()) {
    kr_systemd_notify("READY=1");
  }
  radio_exist(radio);
  radio_shutdown(radio);
  return 0;
}

krad_tags *kr_radio_find_tags_for_item(kr_radio *radio, char *item) {
/*
  kr_mixer_path *unit;

  unit = kr_mixer_path_from_name(radio->mixer, item);
  if (unit != NULL) {
    return unit->tags;
  } else {
    //return krad_transponder_get_tags_for_link(radio->krad_transponder, item);
  }
*/
  return NULL;
}

void kr_radio_set_dir(kr_radio *radio, char *dir) {
  if ((dir == NULL) || (!dir_exists(dir))) {
    return;
  }
  sprintf(radio->log.filename, "%s/%s_%"PRIu64".log", dir, radio->sysname,
   krad_unixtime());
  krad_system_log_on(radio->log.filename);
  printk(KR_VERSION_STR_FULL);
  printk("Station: %s", radio->sysname);
  if (radio->log.startup_timer != NULL) {
    printk("Krad Radio took %"PRIu64"ms to startup",
     kr_timer_duration_ms(radio->log.startup_timer));
  }
  printk("Current Unix Time: %"PRIu64"", krad_unixtime());
}
