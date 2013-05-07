#include "krad_radio_client_internal.h"
#include "krad_radio_client.h"

#include "krad_radio_common.h"
#include "krad_compositor_common.h"
#include "krad_transponder_common.h"
#include "krad_mixer_common.h"

#include "krad_compositor_client.h"
#include "krad_transponder_client.h"
#include "krad_mixer_client.h"

static int kr_radio_crate_to_rep (kr_crate_t *crate);
static int kr_ebml_to_radio_rep (kr_ebml2_t *ebml, kr_radio_t *radio_rep);
static void kr_crate_payload_ebml_reset (kr_crate_t *crate);
static int kr_radio_crate_to_int (kr_crate_t *crate, int *integer);
static int kr_ebml_to_remote_status_rep (kr_ebml2_t *ebml, kr_remote_t *remote);
static int kr_ebml_to_tag_rep (kr_ebml2_t *ebml, kr_tag_t *tag);
static void kr_crate_destroy (kr_crate_t **crate);

void frak_print_raw_ebml (unsigned char *buffer, int len) {

  int i;

  printf ("\nRaw EBML: %d\n", len);
  for (i = 0; i < len; i++) {
    printf ("%02X", buffer[i]);
  }
  printf ("\nEnd EBML\n");
}

int kr_client_sync (kr_client_t *client) {
  kr_io2_flush (client->io);
  kr_ebml2_set_buffer ( client->ebml2, client->io->buf, client->io->space );
  return 0;
}

int kr_poll_out (kr_client_t *client, uint32_t timeout_ms) {

  int ret;
  struct pollfd pollfds[1];

  pollfds[0].fd = client->krad_app_client->sd;
  pollfds[0].events = POLLOUT;

  ret = poll (pollfds, 1, timeout_ms);

  if (pollfds[0].revents & POLLHUP) {
    ret = -1;
  }

  return ret;
}

int kr_client_want_out (kr_client_t *client) {
  return kr_io2_want_out (client->io);
}

int kr_client_push (kr_client_t *client) {
  kr_io2_advance (client->io, client->ebml2->pos);
  if (kr_client_want_out (client)) {
    if ((client->autosync == 1) && (kr_poll_out (client, 0) > 0)) {
      kr_client_sync (client);
    }
  }
  return 0;  
}

kr_client_t *kr_client_create (char *client_name) {

  kr_client_t *client;
  int len;
  
  if (client_name == NULL) {
    return NULL;
  }
  len = strlen (client_name);
  if ((len == 0) || (len > 255)) {
    return NULL;
  }

  client = calloc (1, sizeof(kr_client_t));
  client->name = strdup (client_name);

  client->autosync = 1;

  return client;
}

int kr_connect_remote (kr_client_t *client, char *host, int port, int timeout_ms) {
  
  char url[532];
  int len;
  if ((client == NULL) || (host == NULL) || 
      ((port < 1) || (port > 65535))) {
    return 0;
  }
  len = strlen (host);
  if ((len == 0) || (len > 512)) {
    return 0;
  }

  snprintf (url, sizeof(url), "%s:%d", host, port);

  return kr_connect_full (client, url, timeout_ms);
}


int kr_check_connection (kr_client_t *client) {

  int ret;
  char doctype[32];
  uint32_t version;
  uint32_t read_version;

  kr_ebml2_pack_header (client->ebml2, KRAD_APP_CLIENT_DOCTYPE, KRAD_APP_DOCTYPE_VERSION, KRAD_APP_DOCTYPE_READ_VERSION);
  kr_client_push (client);

  kr_poll (client, 25);
  kr_io2_read (client->io_in);
  kr_ebml2_set_buffer ( client->ebml_in, client->io_in->rd_buf, client->io_in->len );
  //printf ("len %zu\n", client->io_in->len);

  //frak_print_raw_ebml (client->io_in->rd_buf, client->io_in->len);

  ret = kr_ebml2_unpack_header (client->ebml_in, doctype, sizeof(doctype),
                                &version, &read_version);
  if (ret > 0) {
    if ((version == KRAD_APP_DOCTYPE_VERSION) && (read_version == KRAD_APP_DOCTYPE_READ_VERSION) &&
        (strlen(KRAD_APP_SERVER_DOCTYPE) == strlen(doctype)) &&
        (strncmp(doctype, KRAD_APP_SERVER_DOCTYPE, strlen(KRAD_APP_SERVER_DOCTYPE)) == 0)) {

        kr_io2_pulled (client->io_in, client->io_in->len);  
        kr_ebml2_set_buffer ( client->ebml_in, client->io_in->rd_buf, client->io_in->len );
        return ret;
            
    } else {
      printf ("frak %u %u %s \n", version, read_version, doctype);
    }
  } else {
    printf ("frakr %d\n", ret);
  }

  return 0;
}

int kr_connect (kr_client_t *client, char *sysname) {
  return kr_connect_full (client, sysname, 3000);
}

int kr_connect_full (kr_client_t *client, char *sysname, int timeout_ms) {

  if (client == NULL) {
    return 0;
  }
  if (kr_connected (client)) {
    kr_disconnect (client);
  }
  client->krad_app_client = krad_app_connect (sysname, timeout_ms);
  if (client->krad_app_client != NULL) {


    client->io = kr_io2_create ();
    client->ebml2 = kr_ebml2_create ();

    kr_io2_set_fd ( client->io, client->krad_app_client->sd );
    kr_ebml2_set_buffer ( client->ebml2, client->io->buf, client->io->space );

    client->io_in = kr_io2_create ();
    client->ebml_in = kr_ebml2_create ();

    kr_io2_set_fd ( client->io_in, client->krad_app_client->sd );
    kr_ebml2_set_buffer ( client->ebml_in, client->io_in->buf, client->io_in->space );

    if (kr_check_connection (client) > 0) {
      return 1;
    } else {
      kr_disconnect (client);
    }
  }

  return 0;
}

int kr_connected (kr_client_t *client) {
  if (client->krad_app_client != NULL) {
    return 1; 
  }
  return 0;
}

int kr_disconnect (kr_client_t *client) {
  if (client != NULL) {
    if (kr_connected (client)) {
      krad_app_disconnect (client->krad_app_client);
      client->krad_app_client = NULL;
      if (client->io != NULL) {
        kr_io2_destroy (&client->io);
      }
      if (client->ebml2 != NULL) {
        kr_ebml2_destroy (&client->ebml2);
      }
      
      if (client->io_in != NULL) {
        kr_io2_destroy (&client->io_in);
      }
      if (client->ebml_in != NULL) {
        kr_ebml2_destroy (&client->ebml_in);
      }
      return 1;
    }
    return -2;
  }
  return -1;
}

int kr_client_destroy (kr_client_t **client) {
  if (*client != NULL) {
    if (kr_connected (*client)) {
      kr_disconnect (*client);
    }
    if ((*client)->re_crate != NULL) {
      kr_crate_destroy (&(*client)->re_crate);
    }
    if ((*client)->name != NULL) {
      free ((*client)->name);
      (*client)->name = NULL;
    }
    free (*client);
    *client = NULL;
    return 1;
  }
  return -1;
}

int kr_client_local (kr_client_t *client) {
  if (client != NULL) {
    if (kr_connected (client)) {
      if (client->krad_app_client->tcp_port == 0) {
        return 1;
      }
      return 0;
    }
  }
  return -1;
}

int kr_client_get_fd (kr_client_t *client) {
  if (client != NULL) {
    if (kr_connected (client)) {
      return client->krad_app_client->sd;
    }
  }
  return -1;
}

void kr_subscribe_all (kr_client_t *client) {
  kr_subscribe (client, EBML_ID_KRAD_RADIO_GLOBAL_BROADCAST);
}

void kr_subscribe (kr_client_t *client, uint32_t broadcast_id) {

  unsigned char *radio_command;
  unsigned char *subscribe;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_BROADCAST_SUBSCRIBE, &subscribe);  
  kr_ebml2_pack_uint32 (client->ebml2, EBML_ID_KRAD_RADIO_CMD_BROADCAST_SUBSCRIBE, broadcast_id);
  kr_ebml2_finish_element (client->ebml2, subscribe);
  kr_ebml2_finish_element (client->ebml2, radio_command);

  kr_client_push (client);

  client->subscriber = 1;
}

void kr_shm_destroy (kr_shm_t *kr_shm) {

  if (kr_shm != NULL) {
    if (kr_shm->buffer != NULL) {
      munmap (kr_shm->buffer, kr_shm->size);
      kr_shm->buffer = NULL;
    }
    if (kr_shm->fd != 0) {
      close (kr_shm->fd);
    }
    free(kr_shm);
  }
}

kr_shm_t *kr_shm_create (kr_client_t *client) {

  char filename[] = "/tmp/krad-shm-XXXXXX";
  kr_shm_t *kr_shm;

  kr_shm = calloc (1, sizeof(kr_shm_t));

  if (kr_shm == NULL) {
    return NULL;
  }

  kr_shm->size = 1920 * 1080 * 4 * 2;

  kr_shm->fd = mkstemp (filename);
  if (kr_shm->fd < 0) {
    fprintf(stderr, "open %s failed: %m\n", filename);
    kr_shm_destroy (kr_shm);
    return NULL;
  }

  if (ftruncate (kr_shm->fd, kr_shm->size) < 0) {
    fprintf (stderr, "ftruncate failed: %m\n");
    kr_shm_destroy (kr_shm);
    return NULL;
  }

  kr_shm->buffer = mmap (NULL, kr_shm->size, PROT_READ | PROT_WRITE, MAP_SHARED, kr_shm->fd, 0);
  unlink (filename);

  if (kr_shm->buffer == MAP_FAILED) {
    fprintf (stderr, "mmap failed: %m\n");
    kr_shm_destroy (kr_shm);
    return NULL;
  }

  return kr_shm;
}

int kr_send_fd (kr_client_t *client, int fd) {
  return krad_app_client_send_fd (client->krad_app_client, fd);
}

void kr_response_free_string (char **string) {
  free (*string);
}

char *kr_response_alloc_string (int length) {
  return calloc (1, length + 16);
}

int kr_poll (kr_client_t *client, uint32_t timeout_ms) {

  int ret;
  struct pollfd pollfds[1];

  pollfds[0].fd = client->krad_app_client->sd;

  if ((kr_client_want_out (client)) && (client->autosync == 1)) {
    pollfds[0].events = POLLIN | POLLOUT;
  } else {
    pollfds[0].events = POLLIN;
  }

  ret = poll (pollfds, 1, timeout_ms);

  if (pollfds[0].revents & POLLOUT) {
    kr_client_sync (client);
  }

  if (pollfds[0].revents & POLLHUP) {
    ret = -1;
  }

  return ret;
}

int kr_delivery_final (kr_client_t *client) {
  return client->last_delivery_was_final;
}

void kr_delivery_final_reset (kr_client_t *client) {
  client->last_delivery_was_final = 0;
}

int kr_crate_addr_path_match (kr_crate_t *crate, int unit, int subunit) {

  if (crate == NULL) {
    return 0;
  }

  if ((crate->addr->path.unit == unit) &&
      (crate->addr->path.subunit.zero == subunit)) {
    return 1;   
  }
  return 0;
}

int kr_radio_uptime_to_string (uint64_t uptime, char *string) {

  int days;
  int hours;
  int minutes;
  int seconds;
  int pos;
  
  pos = 0;

  days = uptime / (60*60*24);
  minutes = uptime / 60;
  hours = (minutes / 60) % 24;
  minutes %= 60;
  seconds = uptime % 60;

  if (days) {
    pos += sprintf (string + pos, "%d day%s,", days, (days != 1) ? "s" : "");
  }
  if (hours) {
    pos += sprintf (string + pos, "%d:%02d", hours, minutes);
  } else {
    if (minutes) {
      pos += sprintf (string + pos, "%02d min", minutes);
    } else {
      pos += sprintf (string + pos, "%02d seconds", seconds);
    }
  }
  return pos;
}

int kr_radio_response_get_string_from_remote (kr_crate_t *crate, char **string) {

  int len;
  kr_remote_t remote;
  
  len = 0;
  
  kr_ebml_to_remote_status_rep (&crate->payload_ebml, &remote);
  len += sprintf (*string + len, "Interface: %s", remote.interface);
  len += sprintf (*string + len, " Port: %d", remote.port);

  return len;
}

int kr_radio_response_get_string_from_radio (kr_crate_t *crate, char **string) {

  int pos;
  kr_radio_t kr_radio;

  pos = 0;

  kr_ebml_to_radio_rep (&crate->payload_ebml, &kr_radio);
  pos += sprintf (*string + pos, "Krad Radio System Info:\n%s\n", kr_radio.sysinfo);
  if (kr_radio.logname[0] != '\0') {
    pos += sprintf (*string + pos, "Logname: %s\n", kr_radio.logname);
  }
  pos += sprintf (*string + pos, "Station Uptime: ");
  pos += kr_radio_uptime_to_string (kr_radio.uptime, *string + pos);
  pos += sprintf (*string + pos, "\n");  
  pos += sprintf (*string + pos, "System CPU Usage: %u%%", kr_radio.cpu_usage);  
  
  return pos; 
}

int kr_radio_response_get_string_from_cpu (kr_crate_t *crate, char **string) {

  int len;
  uint32_t usage;

  len = 0;
  usage = 0;

  kr_ebml2_unpack_element_uint32 (&crate->payload_ebml, NULL, &usage);
  len += sprintf (*string + len, "System CPU Usage: %u%%", usage);

  return len; 
}

int kr_radio_response_get_string_from_tags (kr_crate_t *crate, char **string) {

  int len;
  uint32_t element;
  uint64_t size;
  kr_tag_t tag;

  len = 0;
  
  //while (kr_ebml2_unpack_id (&crate->payload_ebml, &element, &size) == 0) {
  while (crate->payload_ebml.pos < crate->size) {
    if (len > 0) {
      len += sprintf (*string + len, "\n");  
    }
    kr_ebml2_unpack_id (&crate->payload_ebml, &element, &size);
    kr_ebml_to_tag_rep (&crate->payload_ebml, &tag);
    len += sprintf (*string + len, "%s tag: %s - %s", tag.unit, tag.name, tag.value);
  }
  return len; 
}

int kr_radio_crate_to_string (kr_crate_t *crate, char **string) {

  if (crate->address.path.subunit.zero == KR_TAGS) {
    *string = kr_response_alloc_string (crate->size * 4);
    return kr_radio_response_get_string_from_tags (crate, string);
  }

  switch ( crate->address.path.subunit.station_subunit ) {
    case KR_STATION_UNIT:
      *string = kr_response_alloc_string (crate->size * 4);
      return kr_radio_response_get_string_from_radio (crate, string);
    case KR_CPU:
      *string = kr_response_alloc_string (crate->size * 8);
      return kr_radio_response_get_string_from_cpu (crate, string);
    case KR_REMOTE:
      *string = kr_response_alloc_string (crate->size * 8);
      return kr_radio_response_get_string_from_remote (crate, string);
  }
  
  return 0;  
}

static int kr_compositor_crate_to_int (kr_crate_t *crate, int *integer) {

  uint32_t id;
  uint64_t size;
  uint32_t unsigned_integer;
  
  kr_ebml2_unpack_id (&crate->payload_ebml, &id, &size);

  switch ( crate->address.control.compositor_control ) {
    case KR_X:
    case KR_Y:
    case KR_Z:
    case KR_WIDTH:
    case KR_HEIGHT:                
    case KR_TICKRATE:
      kr_ebml2_unpack_uint32 (&crate->payload_ebml, &unsigned_integer, size);
      *integer = unsigned_integer;
      return 1;
    default:
      break;
  }

  *integer = 0;
  return 0;  
}

static int kr_radio_crate_to_int (kr_crate_t *crate, int *integer) {

  uint32_t id;
  uint64_t size;
  uint32_t unsigned_integer;
  
  kr_ebml2_unpack_id (&crate->payload_ebml, &id, &size);

  switch ( id ) {
    case EBML_ID_KRAD_RADIO_SYSTEM_CPU_USAGE:
      kr_ebml2_unpack_uint32 (&crate->payload_ebml, &unsigned_integer, size);
      *integer = unsigned_integer;
      return 1;
  }

  *integer = 0;
  return 0;
}

int kr_compositor_crate_to_float (kr_crate_t *crate, float *real) {

  switch ( crate->address.control.compositor_control ) {
    case KR_ROTATION:
    case KR_OPACITY:
    case KR_XSCALE:
    case KR_YSCALE:
    case KR_RED:                
    case KR_GREEN:
    case KR_BLUE:
    case KR_ALPHA:    
      kr_ebml2_unpack_element_float (&crate->payload_ebml, NULL, real);
      return 1;
    default:
      break;
  }

  *real = 0.0f;
  return 0; 
}

int kr_mixer_crate_to_float (kr_crate_t *crate, float *real) {
  if (crate->address.path.subunit.mixer_subunit == KR_PORTGROUP) {
    if ((crate->address.control.portgroup_control == KR_VOLUME) ||
        (crate->address.control.portgroup_control == KR_CROSSFADE) ||
        (crate->address.control.portgroup_control == KR_PEAK)) {
      kr_ebml2_unpack_element_float (&crate->payload_ebml, NULL, real);
      return 1;
    }
  } else {
    if (crate->address.path.subunit.mixer_subunit == KR_EFFECT) {
      kr_ebml2_unpack_element_float (&crate->payload_ebml, NULL, real);
      return 1;
    }
  }
  return 0;
}

int kr_crate_to_int (kr_crate_t *crate, int *number) {

  if (crate->size == 0) {
    return 0;
  }

  if (crate->notice != EBML_ID_KRAD_SUBUNIT_CONTROL) {
    return 0;
  }

  kr_crate_payload_ebml_reset (crate);

  switch ( crate->address.path.unit ) {
    case KR_STATION:
      return kr_radio_crate_to_int (crate, number);
    case KR_MIXER:
      break;
    case KR_COMPOSITOR:
      return kr_compositor_crate_to_int (crate, number);
    case KR_TRANSPONDER:
      break;
  }
  return 0;
}

int kr_crate_to_float (kr_crate_t *crate, float *number) {

  if (crate->size == 0) {
    return 0;
  }

  if (crate->notice != EBML_ID_KRAD_SUBUNIT_CONTROL) {
    return 0;
  }

  kr_crate_payload_ebml_reset (crate);

  switch ( crate->address.path.unit ) {
    case KR_STATION:
      break;
    case KR_MIXER:
      return kr_mixer_crate_to_float (crate, number);
    case KR_COMPOSITOR:
      return kr_compositor_crate_to_float (crate, number);
    case KR_TRANSPONDER:
      break;
  }
  return 0;
}

int kr_crate_to_string (kr_crate_t *crate, char **string) {

  if (crate->notice == EBML_ID_KRAD_RADIO_UNIT_DESTROYED) {
    return 0;
  }
  
  if (crate->size == 0) {
    return 0;
  }
  
  kr_crate_payload_ebml_reset (crate);

  switch ( crate->address.path.unit ) {
    case KR_STATION:
      return kr_radio_crate_to_string (crate, string);
    case KR_MIXER:
      return kr_mixer_crate_to_string (crate, string);
    case KR_COMPOSITOR:
      return kr_compositor_crate_to_string (crate, string);
    case KR_TRANSPONDER:
      return kr_transponder_crate_to_string (crate, string);
      break;
  }
  return 0;
}

static int kr_ebml_to_remote_status_rep (kr_ebml2_t *ebml, kr_remote_t *remote) {

  kr_ebml2_unpack_element_string (ebml, NULL, remote->interface, 666);
  kr_ebml2_unpack_element_uint16 (ebml, NULL, &remote->port);

  return 0;
}

static int kr_ebml_to_tag_rep (kr_ebml2_t *ebml, kr_tag_t *tag) {
    
  kr_ebml2_unpack_element_string (ebml, NULL, tag->unit, 666);
  kr_ebml2_unpack_element_string (ebml, NULL, tag->name, 666);
  kr_ebml2_unpack_element_string (ebml, NULL, tag->value, 666);    

  return 0;
}

void kr_response_address (kr_response_t *response, kr_address_t **address) {
  *address = &response->address;
}

static int kr_ebml_to_radio_rep (kr_ebml2_t *ebml, kr_radio_t *radio_rep) {
  kr_ebml2_unpack_element_string (ebml, NULL, radio_rep->sysinfo, 666);
  kr_ebml2_unpack_element_string (ebml, NULL, radio_rep->logname, 666);
  kr_ebml2_unpack_element_uint64 (ebml, NULL, &radio_rep->uptime);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &radio_rep->cpu_usage);
  return 1;
}

static void kr_crate_payload_ebml_reset (kr_crate_t *crate) {
  kr_ebml2_set_buffer ( &crate->payload_ebml, crate->buffer, crate->size );
}

static int kr_radio_crate_to_rep (kr_crate_t *crate) {

  switch ( crate->address.path.subunit.station_subunit ) {
    case KR_STATION_UNIT:
      kr_ebml_to_radio_rep (&crate->payload_ebml, &crate->rep.radio);
      return 1;
    case KR_CPU:
      return 0;
    case KR_REMOTE:
      kr_ebml_to_remote_status_rep (&crate->payload_ebml, &crate->rep.remote);
      return 1;
  }
  return 0;
}

int kr_uncrate_rep (kr_crate_t *crate) {

  if (!((crate->notice == EBML_ID_KRAD_SUBUNIT_CREATED) || 
      (crate->notice == EBML_ID_KRAD_SUBUNIT_INFO) ||
      (crate->notice == EBML_ID_KRAD_UNIT_INFO))) {
    return 0;
  }

  if (crate->size == 0) {
    return 0;
  }
  
  kr_crate_payload_ebml_reset (crate);

  switch ( crate->address.path.unit ) {
    case KR_STATION:
      kr_radio_crate_to_rep (crate);
      return 1;
    case KR_MIXER:
      kr_mixer_crate_to_rep (crate);
      return 1;
    case KR_COMPOSITOR:
      kr_compositor_crate_to_rep (crate);
      return 1;
    case KR_TRANSPONDER:
      break;
  }

  return 0;
}

uint32_t kr_response_size (kr_response_t *kr_response) {
  return kr_response->size;
}

static void kr_crate_destroy (kr_crate_t **crate) {
  if (*crate != NULL) {
    if ((*crate)->buffer != NULL) {
      free ((*crate)->buffer);
    }
    free ((*crate));
    *crate = NULL;
  }
}

void kr_crate_reset (kr_crate_t *crate) {

  kr_client_t *client;
  unsigned char *buffer;
  
  client = NULL;
  buffer = NULL;

  if (crate != NULL) {
    client = crate->client;
    if (crate->buffer != NULL) {
      buffer = crate->buffer;
    }
    memset (crate, 0, sizeof(kr_crate_t));
    crate->client = client;
    if (buffer != NULL) {
      crate->buffer = buffer;
    }
  }
}

void kr_crate_recycle (kr_crate_t **crate) {
  if (*crate != NULL) {
    if ((*crate)->client->re_crate == NULL) {
      kr_crate_reset (*crate);
      (*crate)->client->re_crate = *crate;
      *crate = NULL;
    } else {
      kr_crate_destroy (crate);
    }
  }
}

kr_crate_t *kr_crate_create (kr_client_t *client) {

  kr_crate_t *crate;
  
  crate = calloc (1, sizeof(kr_crate_t));
  crate->client = client;
  
  return crate;
}

int krad_radio_address_to_ebml2 (kr_ebml2_t *ebml, unsigned char **element_loc, kr_address_t *address) {

  switch (address->path.unit) {
    case KR_MIXER:
      kr_ebml2_start_element (ebml, EBML_ID_KRAD_MIXER_MSG, element_loc);
      kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_RADIO_SUBUNIT, address->path.subunit.mixer_subunit);
      if (address->path.subunit.mixer_subunit != KR_UNIT) {
        kr_ebml2_pack_string (ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NAME, address->id.name);
        if (address->path.subunit.mixer_subunit == KR_EFFECT) {
          kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, address->sub_id);
          kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, address->sub_id2);
          kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_RADIO_SUBUNIT_CONTROL, address->control.effect_control);
        }
        if (address->path.subunit.mixer_subunit == KR_PORTGROUP) {
          kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_RADIO_SUBUNIT_CONTROL, address->control.portgroup_control);
        }
      }
      return 1;
    case KR_COMPOSITOR:
      kr_ebml2_start_element (ebml, EBML_ID_KRAD_COMPOSITOR_MSG, element_loc);
      kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_RADIO_SUBUNIT, address->path.subunit.compositor_subunit);
      if (address->path.subunit.compositor_subunit != KR_UNIT) {
        kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, address->id.number);
        kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_RADIO_SUBUNIT_CONTROL, address->control.compositor_control);
      }
      return 1;
    case KR_TRANSPONDER:
      kr_ebml2_start_element (ebml, EBML_ID_KRAD_TRANSPONDER_MSG, element_loc);
      kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_RADIO_SUBUNIT, address->path.subunit.transponder_subunit);
      if (address->path.subunit.transponder_subunit != KR_UNIT) {
        //if (address->path.subunit.transponder_subunit == KR_ADAPTER) {
          kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, address->id.number);
        //}
      }
      return 1;
    case KR_STATION:
      kr_ebml2_start_element (ebml, EBML_ID_KRAD_RADIO_MSG, element_loc);
      kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_RADIO_SUBUNIT, address->path.subunit.station_subunit);
      if (address->path.subunit.station_subunit != KR_UNIT) {
        if (address->path.subunit.station_subunit == KR_REMOTE) {
          kr_ebml2_pack_int32 (ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, address->id.number);
        }
      }
      return 1;
  }
  
  return 0;
}

int krad_read_address_from_ebml (kr_ebml2_t *ebml, kr_address_t *address) {

  uint32_t id;
  uint64_t size;

  id = 0;
  size = 0;

  kr_ebml2_unpack_id (ebml, &id, &size);

  switch ( id ) {
    case EBML_ID_KRAD_RADIO_MSG:
      address->path.unit = KR_STATION;
      kr_ebml2_unpack_id (ebml, &id, &size);
      kr_ebml2_unpack_uint32 (ebml, &address->path.subunit.zero, size);
      if (address->path.subunit.station_subunit == KR_REMOTE) {
        kr_ebml2_unpack_id (ebml, &id, &size);
        kr_ebml2_unpack_uint32 (ebml, &address->id.number, size);
      }
      break;
    case EBML_ID_KRAD_MIXER_MSG:
      address->path.unit = KR_MIXER;
      kr_ebml2_unpack_id (ebml, &id, &size);
      kr_ebml2_unpack_uint32 (ebml, &address->path.subunit.mixer_subunit, size);
      if (address->path.subunit.mixer_subunit != KR_UNIT) {
        kr_ebml2_unpack_id (ebml, &id, &size);
        kr_ebml2_unpack_string (ebml, address->id.name, size);
        if (address->path.subunit.mixer_subunit == KR_EFFECT) {
          kr_ebml2_unpack_id (ebml, &id, &size);
          kr_ebml2_unpack_uint32 (ebml, &address->sub_id, size);
          kr_ebml2_unpack_id (ebml, &id, &size);
          kr_ebml2_unpack_uint32 (ebml, &address->sub_id2, size);
          kr_ebml2_unpack_id (ebml, &id, &size);
          kr_ebml2_unpack_uint32 (ebml, &address->control.effect_control, size);
        }
        if (address->path.subunit.mixer_subunit == KR_PORTGROUP) {
          kr_ebml2_unpack_id (ebml, &id, &size);
          kr_ebml2_unpack_uint32 (ebml, &address->control.portgroup_control, size);
        }
      }
      break;
    case EBML_ID_KRAD_COMPOSITOR_MSG:
      address->path.unit = KR_COMPOSITOR;
      kr_ebml2_unpack_id (ebml, &id, &size);
      kr_ebml2_unpack_uint32 (ebml, &address->path.subunit.zero, size);
      if (address->path.subunit.mixer_subunit != KR_UNIT) {
        kr_ebml2_unpack_id (ebml, &id, &size);
        kr_ebml2_unpack_uint32 (ebml, &address->id.number, size);
        kr_ebml2_unpack_id (ebml, &id, &size);
        kr_ebml2_unpack_uint32 (ebml, &address->control.compositor_control, size);
      }
      break;
    case EBML_ID_KRAD_TRANSPONDER_MSG:
      address->path.unit = KR_TRANSPONDER;
      kr_ebml2_unpack_id (ebml, &id, &size);
      kr_ebml2_unpack_uint32 (ebml, &address->path.subunit.transponder_subunit, size);
      if (address->path.subunit.transponder_subunit != KR_UNIT) {
        kr_ebml2_unpack_id (ebml, &id, &size);
        kr_ebml2_unpack_uint32 (ebml, &address->id.number, size);
      }
      break;
  }
  
  return 1;
}

int krad_message_notice_has_payload (uint32_t notice) {

  if (notice == EBML_ID_KRAD_SHIPMENT_TERMINATOR) {
    return 0;
  }

  if (notice != EBML_ID_KRAD_RADIO_UNIT_DESTROYED) {
    return 1;
  }
  return 0;
}

char *kr_message_notice_to_string (uint32_t notice) {
  switch (notice) {
    case EBML_ID_KRAD_SUBUNIT_CONTROL:
      return "Subunit Controlled";
    case EBML_ID_KRAD_SUBUNIT_CREATED:
      return "Subunit Created";
    case EBML_ID_KRAD_UNIT_INFO:
      return "Unit Info";
    case EBML_ID_KRAD_SUBUNIT_INFO:
      return "Subunit Info";
    case EBML_ID_KRAD_RADIO_UNIT_DESTROYED:
      return "Subunit Destroyed";
  }
  
  return "Unknown Message Notice";
}

void kr_message_notice_debug_print (uint32_t notice) {
  printf ("Message Notice: %s\n", kr_message_notice_to_string (notice));
}

void kr_response_payload_print_raw_ebml (kr_response_t *response) {

  int i;

  printf ("\nRaw EBML: \n");
  for (i = 0; i < response->size; i++) {
    printf ("%02X", response->buffer[i]);
  }
  printf ("\nEnd EBML\n");
}

uint32_t kr_response_get_event (kr_response_t *response) {
  return response->notice;
}

int kr_have_full_crate (kr_io2_t *in) {

  kr_ebml2_t ebml;
  uint32_t element;
  uint64_t size;
  int ret;

  if (!(kr_io2_has_in (in))) {
    return 0;
  }

  kr_ebml2_set_buffer ( &ebml, in->rd_buf, in->len );
  
  ret = kr_ebml2_unpack_id (&ebml, &element, &size);

  if (ret < 0) {
    printf ("full_command EBML ID Not found");
    return 0;
  }

  size += ebml.pos;

  if (in->len < size) {
    //printf ("full_command Not Enough bytes.. have %zu need %zu\n", in->len, size);
    return 0;
  } else {
    //printf ("Got command have %zu need %zu\n", in->len, size);
  }
  
  return size;
}


int kr_delivery_get (kr_client_t *client, kr_crate_t **kr_crate) {

  kr_response_t *response;

  uint32_t ebml_id;
  uint64_t ebml_data_size;
  int have_crate;

  have_crate = 0;
  ebml_id = 0;
  ebml_data_size = 0;

  have_crate = kr_have_full_crate (client->io_in);

  if (have_crate) {

    kr_ebml2_set_buffer ( client->ebml_in, client->io_in->rd_buf, client->io_in->len );

    if (client->re_crate != NULL) {
      response = client->re_crate;
      client->re_crate = NULL;
    } else {
      response = kr_crate_create (client);
    }
    *kr_crate = response;

    response->inside.actual = &response->rep.actual;

    krad_read_address_from_ebml (client->ebml_in, &response->address);

    kr_ebml2_unpack_element_uint32 (client->ebml_in, NULL, &response->notice);

    response->addr = &response->address;
    
    if (!client->subscriber) {
      if (response->notice == EBML_ID_KRAD_SHIPMENT_TERMINATOR) {
        client->last_delivery_was_final = 1;
      } else {
        client->last_delivery_was_final = 0;
      }
    }

    //kr_address_debug_print (&response->address);
    //kr_message_notice_debug_print (response->notice);

    if (krad_message_notice_has_payload (response->notice)) {
      kr_ebml2_unpack_id (client->ebml_in, &ebml_id, &ebml_data_size);
      if (ebml_data_size > 0) {
        //printf ("KR Client Response payload size: %"PRIu64"\n", ebml_data_size);
        response->size = ebml_data_size;
        if (response->buffer == NULL) {
          response->buffer = malloc (2048);
        }
        kr_ebml2_unpack_data (client->ebml_in, response->buffer, ebml_data_size);
      }
    }

    if (kr_uncrate_int (response, &response->integer)) {
      response->has_int = 1;
    }
    if (kr_uncrate_float (response, &response->real)) {
      response->has_float = 1;
    }
    
    kr_io2_pulled (client->io_in, have_crate);

  }
  return have_crate;
}

int kr_crate_has_float (kr_crate_t *crate) {
  return crate->has_float;
}

int kr_crate_has_int (kr_crate_t *crate) {
  return crate->has_int;
}

void kr_client_response_wait (kr_client_t *client, kr_response_t **kr_response) {
  kr_poll (client, 250);
  kr_delivery_get (client, kr_response);
}

void kr_delivery_recv (kr_client_t *client) {
  kr_io2_read (client->io_in);
}

void kr_client_response_wait_print (kr_client_t *client) {

  kr_crate_t *crate;
  char *string;
  int wait_time_ms;
  int length;
  int got_all_delivery;

  got_all_delivery = 0;
  string = NULL;
  crate = NULL;  
  wait_time_ms = 250;

  while (1) {
    kr_delivery_get (client, &crate);
    if (crate == NULL) {
      if (kr_poll (client, wait_time_ms)) {
        kr_delivery_recv (client);
      } else {
        break;
      }
    } else {
      length = kr_uncrate_string (crate, &string);
      if (length > 0) {
        printf ("%s\n", string);
        kr_string_recycle (&string);
      }
      kr_crate_recycle (&crate);
      if (kr_delivery_final (client)) {
        kr_delivery_final_reset (client);
        got_all_delivery = 1;
        break;
      }
    }
  }

  if (got_all_delivery == 0) {
    printf ("No response after waiting %dms\n", wait_time_ms);
  }
}

int kr_delivery_get_until_final (kr_client_t *client, kr_crate_t **crate, uint32_t timeout_ms) {

  kr_crate_t *lcrate;
  
  lcrate = NULL;
 
  if (client == NULL) {
    failfast ("kr_delivery_get_until_final called with NULL client pointer");
  }
  
  if (crate == NULL) {
    failfast ("kr_delivery_get_until_final called with NULL crate pointer");
  }
  
  if ((crate != NULL) && (*crate != NULL)) {
    kr_crate_recycle (crate);
  }

  while (1) {
    kr_delivery_get (client, &lcrate);
    if (lcrate == NULL) {
      if (kr_poll (client, timeout_ms)) {
        kr_delivery_recv (client);
      } else {
        break;
      }
    } else {
      if (kr_delivery_final (client)) {
        kr_crate_recycle (&lcrate);
        kr_delivery_final_reset (client);
        return 0;
      }
      *crate = lcrate;
      return 1;
    }
  }

  printf ("No response after waiting %dms\n", timeout_ms);

  return 0;
}

void kr_set_dir (kr_client_t *client, char *dir) {

  unsigned char *command;
  unsigned char *setdir;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_SET_DIR, &setdir);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_DIR, dir);
  kr_ebml2_finish_element (client->ebml2, setdir);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}

void kr_system_info (kr_client_t *client) {

  unsigned char *command;
  unsigned char *info_command;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_GET_SYSTEM_INFO, &info_command);
  kr_ebml2_finish_element (client->ebml2, info_command);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}

void kr_remote_list (kr_client_t *client) {

  unsigned char *command;
  unsigned char *remote_status_command;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_GET_REMOTE_STATUS, &remote_status_command);
  kr_ebml2_finish_element (client->ebml2, remote_status_command);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}

int kr_remote_on (kr_client_t *client, char *interface, int port) {

  unsigned char *radio_command;
  unsigned char *enable_remote;
  uint16_t port_actual;

  if (!kr_sys_port_valid (port)) {
    return -1;
  }
  
  port_actual = port;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_REMOTE_ENABLE, &enable_remote);
  if ((interface != NULL) && (strlen(interface))) {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_REMOTE_INTERFACE, interface);
  } else {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_REMOTE_INTERFACE, "");
  }
  kr_ebml2_pack_uint16 (client->ebml2, EBML_ID_KRAD_RADIO_TCP_PORT, port_actual);
  kr_ebml2_finish_element (client->ebml2, enable_remote);
  kr_ebml2_finish_element (client->ebml2, radio_command);

  kr_client_push (client);
  return 1;
}

int kr_remote_off (kr_client_t *client, char *interface, int port) {

  unsigned char *radio_command;
  unsigned char *disable_remote;

  if (!kr_sys_port_valid (port)) {
    return -1;
  }

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_REMOTE_DISABLE, &disable_remote);
  if ((interface != NULL) && (strlen(interface))) {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_REMOTE_INTERFACE, interface);
  } else {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_REMOTE_INTERFACE, "");
  }
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_TCP_PORT, port);
  kr_ebml2_finish_element (client->ebml2, disable_remote);
  kr_ebml2_finish_element (client->ebml2, radio_command);

  kr_client_push (client);
  return 1;
}

void kr_web_enable (kr_client_t *client, uint32_t http_port, uint32_t websocket_port,
                    char *headcode, char *header, char *footer) {

  unsigned char *radio_command;
  unsigned char *webon;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_WEB_ENABLE, &webon);
  kr_ebml2_pack_uint32 (client->ebml2, EBML_ID_KRAD_RADIO_HTTP_PORT, http_port);
  kr_ebml2_pack_uint32 (client->ebml2, EBML_ID_KRAD_RADIO_WEBSOCKET_PORT, websocket_port);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_WEB_HEADCODE, headcode);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_WEB_HEADER, header);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_WEB_FOOTER, footer);
  kr_ebml2_finish_element (client->ebml2, webon);
  kr_ebml2_finish_element (client->ebml2, radio_command);


  kr_client_push (client);
}

void kr_web_disable (kr_client_t *client) {

  unsigned char *radio_command;
  unsigned char *weboff;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_WEB_DISABLE, &weboff);
  kr_ebml2_finish_element (client->ebml2, weboff);
  kr_ebml2_finish_element (client->ebml2, radio_command);

  kr_client_push (client);
}

void kr_osc_enable (kr_client_t *client, int port) {

  unsigned char *radio_command;
  unsigned char *enable_osc;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_OSC_ENABLE, &enable_osc);  
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_UDP_PORT, port);
  kr_ebml2_finish_element (client->ebml2, enable_osc);
  kr_ebml2_finish_element (client->ebml2, radio_command);

  kr_client_push (client);
}

void kr_osc_disable (kr_client_t *client) {

  unsigned char *radio_command;
  unsigned char *disable_osc;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_OSC_DISABLE, &disable_osc);
  kr_ebml2_finish_element (client->ebml2, disable_osc);
  kr_ebml2_finish_element (client->ebml2, radio_command);

  kr_client_push (client);
}

void kr_tags (kr_client_t *client, char *item) {

  unsigned char *radio_command;
  unsigned char *get_tags;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_LIST_TAGS, &get_tags);  

  if (item == NULL) {
    item = "station";
  }

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_TAG_ITEM, item);

  kr_ebml2_finish_element (client->ebml2, get_tags);
  kr_ebml2_finish_element (client->ebml2, radio_command);
    
  kr_client_push (client);
}

void kr_tag (kr_client_t *client, char *item, char *tag_name) {

  unsigned char *radio_command;
  unsigned char *get_tag;
  unsigned char *tag;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_GET_TAG, &get_tag);  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_TAG, &tag);  

  if (item == NULL) {
    item = "station";
  }

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_TAG_ITEM, item);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_TAG_NAME, tag_name);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_TAG_VALUE, "");  

  kr_ebml2_finish_element (client->ebml2, tag);
  kr_ebml2_finish_element (client->ebml2, get_tag);
  kr_ebml2_finish_element (client->ebml2, radio_command);
    
  kr_client_push (client);
}

void kr_set_tag (kr_client_t *client, char *item, char *tag_name, char *tag_value) {

  unsigned char *radio_command;
  unsigned char *set_tag;
  unsigned char *tag;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_SET_TAG, &set_tag);  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_TAG, &tag);  

  if (item == NULL) {
    item = "station";
  }

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_TAG_ITEM, item);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_TAG_NAME, tag_name);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_TAG_VALUE, tag_value);  

  kr_ebml2_finish_element (client->ebml2, tag);
  kr_ebml2_finish_element (client->ebml2, set_tag);
  kr_ebml2_finish_element (client->ebml2, radio_command);
    
  kr_client_push (client);
}

void kr_address_debug_print (kr_address_t *addr) {

  kr_unit_t *unit;
  kr_subunit_t *subunit;
  kr_unit_id_t *id;
  kr_unit_control_name_t *control;
    
  unit = &addr->path.unit;
  subunit = &addr->path.subunit;
  id = &addr->id;
  control = &addr->control;

  printf ("Address: ");

  switch (*unit) {
    case KR_STATION:
      printf ("Station ");
      break;
    case KR_MIXER:
      printf ("Mixer ");
      break;
    case KR_COMPOSITOR:
      printf ("Compositor ");
      break;
    case KR_TRANSPONDER:
      printf ("Transponder ");
      break;
  }

  if (subunit->zero == KR_UNIT) {
    printf ("Unit Info\n");
    return;
  }

  switch (*unit) {
    case KR_STATION:
      if (subunit->zero == KR_TAGS) {
        printf ("Station Tags");
        break;
      }
      switch (subunit->station_subunit) {
        case KR_CPU:
          printf ("CPU Usage");
          break;
        case KR_REMOTE:
          printf ("Remote %d", id->number);
          break;
        default:
          break;
      }
      break;
    case KR_MIXER:
      switch (subunit->mixer_subunit) {
        case KR_PORTGROUP:
          printf ("%s ", id->name);
          if (control->portgroup_control == 0) {
            break;
          }
          if (control->portgroup_control == KR_VOLUME) {
            printf ("Volume");
          }
          if (control->portgroup_control == KR_CROSSFADE) {
            printf ("Crossfade");
          }
          if (control->portgroup_control == KR_PEAK) {
            printf ("Peak");
          }
          break;
        case KR_EFFECT:
          printf ("%s ", id->name);
          printf ("Effect: %d Control ID: %d Control: %s", addr->sub_id, addr->sub_id2,
                  effect_control_to_string(addr->control.effect_control));
          break;
      }
      break;
    case KR_COMPOSITOR:
      switch (subunit->compositor_subunit) {
        case KR_VIDEOPORT:
          printf ("Unit, Videoport");
          break;
        case KR_SPRITE:
          printf ("Unit, Sprite");
          break;
        case KR_TEXT:
          printf ("Unit, Text");
          break;
        case KR_VECTOR:
          printf ("Unit, Vector");
          break;
      }
      break;
    case KR_TRANSPONDER:
      switch (subunit->transponder_subunit) {
        case KR_ADAPTER:
          printf ("Adapter: %d", id->number);
          break;
        case KR_TRANSMITTER:
          printf ("Transmitter");
          break;
        case KR_RECEIVER:
          printf ("Receiver");
          break;
        case KR_RAWIN:
          printf ("RAW In");
          break;
        case KR_RAWOUT:
          printf ("RAW Out");
          break;
        case KR_DEMUXER:
          printf ("Demuxer");
          break;
        case KR_MUXER:
          printf ("Muxer");
          break;
        case KR_ENCODER:
          printf ("Encoder");
          break;
        case KR_DECODER:
          printf ("Decoder");
          break;
      }
      break;
  }

  printf ("\n");
}

#define MAX_TOKENS 8

int kr_string_to_address (char *string, kr_address_t *addr) {

  char *pch;
  char *save;
  char *tokens[MAX_TOKENS];
  int t;
  int i;
  char *effect_num;
  char *effect_control_id;
  char *effect_control;

  kr_unit_t *unit;
  kr_subunit_t *subunit;
  kr_unit_id_t *id;
  kr_unit_control_name_t *control;
    
  unit = &addr->path.unit;
  subunit = &addr->path.subunit;
  id = &addr->id;
  control = &addr->control;
  
  i = 0;
  t = 0;
  save = NULL;

  //printf ("string was %s\n", string);

  pch = strtok_r (string, "/ ", &save);
  while ((pch != NULL) && (t < MAX_TOKENS)) {
    tokens[t] = pch;
    t++;
    //printf ("%s ", pch);
    pch = strtok_r (NULL, "/ ", &save);
  }

  //printf ("\n%d tokens\n", t);

  for (i = 0; i < t; i++) {
    //printf ("%d: %s  ", i + 1, tokens[i]);
  }
  
  //printf ("\n");
  
  if (t < 2) {
    printf ("Invalid Unit Control Address: Less than 2 parts\n");
    return -1;
  }
  if (t > 5) {
    printf ("Invalid Unit Control Address: More than 5 parts\n");
    return -1;
  }
    
  // Determine Unit
  
  if (tokens[0][1] == '\0') {
    switch (tokens[0][0]) {
      case 'm':
        *unit = KR_MIXER;
        break;
      case 'c':
        *unit = KR_COMPOSITOR;
        break;
      case 't':
        *unit = KR_TRANSPONDER;
        break;
      case 'e':
        *unit = KR_MIXER;
        subunit->mixer_subunit = KR_EFFECT;
        if ((t != 4) && (t != 5)) {
          printf ("Invalid Mixer Effect control address, not 4 or 5 parts\n");
          return -1;
        }
        break;
      case 's':
        *unit = KR_COMPOSITOR;
        subunit->compositor_subunit = KR_SPRITE;
        break;
      case 'v':
        *unit = KR_COMPOSITOR;
        subunit->compositor_subunit = KR_VIDEOPORT;
        break;
    }
  } else {
    if (tokens[0][2] == '\0') {
      if ((tokens[0][0] == 'c') && (tokens[0][1] == 's')) {
        *unit = KR_COMPOSITOR;
        subunit->compositor_subunit = KR_SPRITE;
      }
      if ((tokens[0][0] == 'v') && (tokens[0][1] == 'e')) {
        *unit = KR_COMPOSITOR;
        subunit->compositor_subunit = KR_VECTOR;
      }
      if ((tokens[0][0] == 't') && (tokens[0][1] == 't')) {
        *unit = KR_COMPOSITOR;
        subunit->compositor_subunit = KR_TEXT;
      }
    } else {
      if (tokens[0][3] == '\0') {
        if ((tokens[0][0] == 'v') && (tokens[0][1] == 'e') && (tokens[0][2] == 'c')) {
          *unit = KR_COMPOSITOR;
        }
        if ((tokens[0][0] == 'm') && (tokens[0][1] == 'i') && (tokens[0][2] == 'x')) {
          *unit = KR_MIXER;
        }
        if ((tokens[0][0] == 'c') && (tokens[0][1] == 'o') && (tokens[0][2] == 'm')) {
          *unit = KR_COMPOSITOR;
        }
      } else {
        if (tokens[0][4] == '\0') {
          if ((tokens[0][0] == 't') && (tokens[0][1] == 'e') &&
              (tokens[0][2] == 'x') && (tokens[0][3] == 't')) {
            *unit = KR_COMPOSITOR;
            subunit->compositor_subunit = KR_TEXT;
          }
          if ((tokens[0][0] == 'c') && (tokens[0][1] == 'o') &&
              (tokens[0][2] == 'm') && (tokens[0][3] == 'p')) {
            *unit = KR_COMPOSITOR;
          }
        } else {
          if (tokens[0][5] == '\0') {
            if ((tokens[0][0] == 'v') && (tokens[0][1] == 'e') &&
                (tokens[0][2] == 'c') && (tokens[0][3] == 'o') && (tokens[0][4] == 'r')) {
              *unit = KR_COMPOSITOR;
              subunit->compositor_subunit = KR_VECTOR;
            }
           if ((tokens[0][0] == 'm') && (tokens[0][1] == 'i') &&
                (tokens[0][2] == 'x') && (tokens[0][3] == 'e') && (tokens[0][4] == 'r')) {
              *unit = KR_MIXER;
            }
          } else {
            if (tokens[0][6] == '\0') {
              if ((tokens[0][0] == 's') && (tokens[0][1] == 'p') && (tokens[0][2] == 'r') &&
                  (tokens[0][3] == 'i') && (tokens[0][4] == 't') && (tokens[0][5] == 'e')) {
                *unit = KR_COMPOSITOR;
                subunit->compositor_subunit = KR_SPRITE;
              }
            } else {
              if ((strlen(tokens[0]) == 10) && (strncmp(tokens[0], "compositor", 10) == 0)) {
                *unit = KR_COMPOSITOR;
              } else {
                if ((strlen(tokens[0]) == 11) && (strncmp(tokens[0], "transponder", 11) == 0)) {
                  *unit = KR_TRANSPONDER;
                }
              }
            }
          }
        }
      }
    }
  }

  if (*unit == 0) {
    printf ("Unit could not be identified\n");
    return -1;
  }
  
  switch (*unit) {
    case KR_MIXER:
      strncpy (id->name, tokens[1], sizeof (id->name));
      if (t == 2) {
        subunit->mixer_subunit = KR_PORTGROUP;
        control->portgroup_control = KR_VOLUME;
      }
      if (t == 3) {
        subunit->mixer_subunit = KR_PORTGROUP;
        if ((tokens[2][0] == 'f') || (tokens[2][0] == 'c')) {
          control->portgroup_control = KR_CROSSFADE;
        } else {
          if (tokens[2][0] == 'v') {
            control->portgroup_control = KR_VOLUME;
          } else {
            printf ("Invalid Mixer Portgroup Control\n");
            return -1;
          }
        }
      }
      if ((t == 4) || (t == 5)) {
        subunit->mixer_subunit = KR_EFFECT;
        if (t == 4) {
          effect_num = tokens[2];
          effect_control = tokens[3];
        }
        if (t == 5) {
          effect_num = tokens[2];
          effect_control_id = tokens[3];
          effect_control = tokens[4];
        }
        if (effect_num[0] == 'e') {
          addr->sub_id = 0;
        } else {
          if (effect_num[0] == 'l') {
            addr->sub_id = 1;
          } else {
            if (effect_num[0] == 'h') {
              addr->sub_id = 2;
            } else {
              if (effect_num[0] == 'a') {
                addr->sub_id = 3;
              } else {
                addr->sub_id = atoi(effect_num);
              }
            }
          }
        }
        if ((addr->sub_id == 0) && (t != 5)) {
          printf ("Invalid Mixer Portgroup Effect Control\n");
          return -1;
        }
        if ((addr->sub_id == 0) && (t == 5)) {
          addr->sub_id2 = atoi(effect_control_id);
        }
        if ((effect_control[0] == 'd') && (effect_control[1] == 'b')) {
          addr->control.effect_control = DB;
        } else {
          if ((effect_control[0] == 'h') && (effect_control[1] == 'z')) {
            addr->control.effect_control = HZ;
          } else {
            if ((effect_control[0] == 'b') && (effect_control[1] == 'w')) {
              addr->control.effect_control = BANDWIDTH;
            } else {
              if (effect_control[0] == 't') {
                addr->control.effect_control = TYPE;
              } else {
                if ((effect_control[0] == 'd') && (effect_control[1] == 'r')) {
                  addr->control.effect_control = DRIVE;
                } else {
                  if ((effect_control[0] == 'b') && (effect_control[1] == 'l')) {
                    addr->control.effect_control = BLEND;
                  }
                }
              }
            }
          }
        }
      }
      break;
    case KR_COMPOSITOR:
      if ((subunit->compositor_subunit > 0) && (t == 2)) {
          addr->id.number = atoi (tokens[1]);
          return 1;
      }
      if ((subunit->compositor_subunit > 0) && (t == 3)) {
          addr->id.number = atoi (tokens[1]);
          addr->control.compositor_control = krad_string_to_compositor_control (tokens[2]);
          return 1;
      }
      printf ("Invalid COMPOSITOR Control\n");
      return -1;
      break;
    case KR_TRANSPONDER:
      if (t == 2) {
        addr->id.number = atoi (tokens[1]);
        return 1;
      }
      printf ("Invalid TRANSPONDER Control\n");
      return -1;
      break;
    case KR_STATION:
      printf ("Invalid RADIO Control\n");
      return -1;
      break;
  }

  //kr_address_debug_print (addr); 

  return 1;
}

void kr_unit_destroy (kr_client_t *client, kr_address_t *address) {

  if ((client == NULL) || (address == NULL)) {
    return;
  }
  if (address->path.unit == KR_COMPOSITOR) {
    kr_compositor_subunit_destroy (client, address);
  }
  if (address->path.unit == KR_MIXER) {
    kr_mixer_remove_portgroup (client, address->id.name);
  }
  if (address->path.unit == KR_TRANSPONDER) {
    kr_transponder_subunit_destroy (client, address->id.number);
  }
}

int kr_unit_control_data_type_from_address (kr_address_t *address, kr_unit_control_data_t *data_type) {

  switch (address->path.unit) {
    case KR_MIXER:
      *data_type = KR_FLOAT;
      return 1;
    case KR_COMPOSITOR:
      if ((address->control.compositor_control == KR_X) ||
          (address->control.compositor_control == KR_Y) ||
          (address->control.compositor_control == KR_Z) ||
          (address->control.compositor_control == KR_WIDTH) ||
          (address->control.compositor_control == KR_HEIGHT) ||
          (address->control.compositor_control == KR_TICKRATE)) {
          *data_type = KR_INT32;
      } else {
        *data_type = KR_FLOAT;
      }  
      return 1;
    default:
      break;
  }

  return 0;
}

void kr_print_ebml (unsigned char *buffer, int len) {

  int i;

  i = 0;

  printf ("Raw EBML: \n");
  for (i = 0; i < len; i++) {
    printf ("%02X", buffer[i]);
  }
  printf ("\nEnd Raw EBML\n");
}

int kr_unit_control_set (kr_client_t *client, kr_unit_control_t *uc) {

  unsigned char *my_command;
  unsigned char *my_set_control;

  my_command = NULL;
  my_set_control = NULL;
  
  kr_unit_control_data_type_from_address (&uc->address, &uc->data_type);

  switch (uc->address.path.unit) {
    case KR_MIXER:
      switch (uc->address.path.subunit.mixer_subunit) {
        case KR_PORTGROUP:
          if (uc->address.control.portgroup_control == KR_VOLUME) {
            kr_mixer_set_control (client, uc->address.id.name, "volume", uc->value.real, uc->duration);
          } else {
            kr_mixer_set_control (client, uc->address.id.name, "crossfade", uc->value.real, uc->duration);
          }
          break;
        case KR_EFFECT:
          kr_mixer_set_effect_control (client, uc->address.id.name, uc->address.sub_id, uc->address.sub_id2,
                                       effect_control_to_string(uc->address.control.effect_control),
                                       uc->value.real, uc->duration, EASEINOUTSINE);
          break;
      }
      break;
    case KR_COMPOSITOR:

      kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &my_command);
      kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_SET_SUBUNIT, &my_set_control);
      kr_ebml2_pack_int32 (client->ebml2,
                           EBML_ID_KRAD_RADIO_SUBUNIT,
                           uc->address.path.subunit.compositor_subunit);
      kr_ebml2_pack_int32 (client->ebml2,
                           EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER,
                           uc->address.id.number);
      kr_ebml2_pack_int32 (client->ebml2,
                           EBML_ID_KRAD_SUBUNIT_CONTROL,
                           uc->address.control.compositor_control);
      if (uc->data_type == KR_FLOAT) {
        kr_ebml2_pack_float (client->ebml2, EBML_ID_KRAD_SUBUNIT_CONTROL, uc->value.real);
      }
      if (uc->data_type == KR_INT32) {
        kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_SUBUNIT_CONTROL, uc->value.integer);
      }
      if (uc->data_type == KR_STRING) {
        kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_SUBUNIT_CONTROL, uc->value.string);
      }
      kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_SUBUNIT_CONTROL, uc->duration);
      kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_SUBUNIT_CONTROL, 666);
      kr_ebml2_finish_element (client->ebml2, my_set_control);
      kr_ebml2_finish_element (client->ebml2, my_command);

      if (client->autosync == 1) {
        kr_client_push (client);
      }

      break;
    case KR_STATION:
      break;
    case KR_TRANSPONDER:
      return -1;
      break;
  }

  return 0;
}

