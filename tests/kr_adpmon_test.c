#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <libudev.h>
#include <locale.h>

/* gcc -Wall kr_adpmon_test.c -o kr_adpmon_test `pkg-config --libs --cflags libudev` */

#define KR_ADPMONMAX 4

typedef struct kr_adapter_monitor kr_adapter_monitor;
typedef struct kr_adapter_watch kr_adapter_watch;

struct kr_adapter_watch {
  int type;
  int wd;
  char path[64];
};

struct kr_adapter_monitor {
  struct udev *udev;
  int fd;
  struct udev_monitor *mon;
  kr_adapter_watch watches[KR_ADPMONMAX];
  int initialized;
};

int kr_adapter_monitor_destroy(kr_adapter_monitor *monitor);
kr_adapter_monitor *kr_adapter_monitor_create();
void kr_adapter_monitor_wait(kr_adapter_monitor *monitor, int ms);

static void handle_device(kr_adapter_monitor *m, struct udev_device *dev);
static void setup(kr_adapter_monitor *m);

static void handle_device(kr_adapter_monitor *m, struct udev_device *dev) {
  struct udev_device *parent;
  const char *name;
  const char *subsys;
  const char *action;
  int name_len;
  int subsys_len;

  name = udev_device_get_sysname(dev);
  if (!name) return;
  subsys = udev_device_get_subsystem(dev);
  if (!subsys) return;
  name_len = strlen(name);
  subsys_len = strlen(subsys);
  if ((subsys_len == 5) && (name_len > 4)
   && (memcmp(subsys, "sound", 5) == 0)
   && (memcmp(name, "card", 4) == 0)) {
    printf("Got ALSA device\n");
  } else {
    if ((subsys_len == 4) && (name_len > 10)
     && (memcmp(subsys, "misc", 4) == 0)
     && (memcmp(name, "blackmagic", 10) == 0)) {
      printf("Got Blackmagic device\n");
    } else {
      if ((subsys_len == 11) && (name_len > 5)
       && (memcmp(subsys, "video4linux", 4) == 0)
       && (memcmp(name, "video", 5) == 0)) {
        printf("Got V4L2 device\n");
      } else {
        return;
      }
    }
  }
  action = udev_device_get_action(dev);
  printf("   syspath: %s\n", udev_device_get_syspath(dev));
  printf("   sysname: %s\n", udev_device_get_sysname(dev));
  printf("   sysnum: %s\n", udev_device_get_sysnum(dev));
  printf("   Node: %s\n", udev_device_get_devnode(dev));
  printf("   Subsystem: %s\n", udev_device_get_subsystem(dev));
  printf("   Action: %s\n", udev_device_get_action(dev));
  if (!((action == NULL) || ((strlen(action) == 3)
   && (memcmp(action, "add", 3) == 0))))  {
    return;
  }
  parent = udev_device_get_parent_with_subsystem_devtype(dev, "usb",
   "usb_device");
  if (parent) {
    printf("  VID/PID: %s %s\n",
     udev_device_get_sysattr_value(parent,"idVendor"),
     udev_device_get_sysattr_value(parent, "idProduct"));
    printf("  serial: %s\n",
     udev_device_get_sysattr_value(parent, "serial"));
  } else {
    parent = udev_device_get_parent_with_subsystem_devtype(dev, "pci", NULL);
    if (parent) {
      printf("  vendor: %s\n",
       udev_device_get_sysattr_value(parent, "vendor"));
      printf("  device: %s\n",
       udev_device_get_sysattr_value(parent, "device"));
    }
  }
}

static void setup(kr_adapter_monitor *m) {
  struct udev_enumerate *enumerate;
  struct udev_list_entry *devices;
  struct udev_list_entry *dev_list_entry;
  struct udev_device *dev;
  const char *path;
  int i;
  char sysname[64];
  if (m == NULL) return;
  if (m->udev != NULL) return;
  m->udev = udev_new();
  if (m->udev == NULL) return;
	m->mon = udev_monitor_new_from_netlink(m->udev, "udev");
	udev_monitor_filter_add_match_subsystem_devtype(m->mon, "video4linux", NULL);
	udev_monitor_filter_add_match_subsystem_devtype(m->mon, "sound", NULL);
	udev_monitor_enable_receiving(m->mon);
	m->fd = udev_monitor_get_fd(m->mon);
  enumerate = udev_enumerate_new(m->udev);
	udev_enumerate_add_match_subsystem(enumerate, "video4linux");
  udev_enumerate_add_match_subsystem(enumerate, "sound");
  udev_enumerate_scan_devices(enumerate);
  devices = udev_enumerate_get_list_entry(enumerate);
  udev_list_entry_foreach(dev_list_entry, devices) {
    path = udev_list_entry_get_name(dev_list_entry);
    dev = udev_device_new_from_syspath(m->udev, path);
    handle_device(m, dev);
    udev_device_unref(dev);
  }
  udev_enumerate_unref(enumerate);

  enumerate = udev_enumerate_new(m->udev);
	udev_enumerate_add_match_subsystem(enumerate, "misc");
  for (i = 0; i < 24; i++) {
	  snprintf(sysname, sizeof(sysname), "blackmagic%d", i);
    udev_enumerate_add_match_sysname(enumerate, sysname);
  }
	udev_enumerate_scan_devices(enumerate);
  devices = udev_enumerate_get_list_entry(enumerate);
  udev_list_entry_foreach(dev_list_entry, devices) {
    path = udev_list_entry_get_name(dev_list_entry);
    dev = udev_device_new_from_syspath(m->udev, path);
    handle_device(m, dev);
    udev_device_unref(dev);
  }
  udev_enumerate_unref(enumerate);

  m->initialized = 1;
}

void kr_adapter_monitor_wait(kr_adapter_monitor *monitor, int ms) {
  int ret;
  struct udev_device *dev;
  struct pollfd pollfds[1];
  if (monitor == NULL) return;
  if (!monitor->initialized) setup(monitor);
  for(;;) {
    memset(&pollfds, 0, sizeof(pollfds));
    pollfds[0].fd = monitor->fd;
    pollfds[0].events = POLLIN;
    ret = poll(pollfds, 1, ms);
    if (ret < 0) break;
    if (ret == 0) break;
    if (ret > 0) {
      dev = udev_monitor_receive_device(monitor->mon);
      if (dev != NULL) {
        handle_device(monitor, dev);
        udev_device_unref(dev);
      }
    }
  }
}

int kr_adapter_monitor_destroy(kr_adapter_monitor *monitor) {
  if (monitor == NULL) return -1;
  if (monitor->fd > -1) {
    close(monitor->fd);
    monitor->fd = -1;
    udev_monitor_unref(monitor->mon);
  }
  if (monitor->udev != NULL) {
    udev_unref(monitor->udev);
    monitor->udev = NULL;
  }
  free(monitor);
  return 0;
}

kr_adapter_monitor *kr_adapter_monitor_create() {
  kr_adapter_monitor *monitor;
	monitor = calloc(1, sizeof(kr_adapter_monitor));
  monitor->fd = -1;
  return monitor;
}

int main(int argc, char **argv) {
  int times;
  kr_adapter_monitor *monitor;
  monitor = kr_adapter_monitor_create();
  times = 0;
  if (argc == 2) {
    times = atoi(argv[1]);
  }
  kr_adapter_monitor_wait(monitor, 0);
  while (times-- > 0) {
    kr_adapter_monitor_wait(monitor, 1000);
  }
  kr_adapter_monitor_destroy(monitor);
  return 0;
}
