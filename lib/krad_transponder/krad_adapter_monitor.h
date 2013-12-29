#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <libudev.h>
#include <locale.h>

#include "krad_system.h"

typedef struct kr_adapter_monitor kr_adapter_monitor;

int kr_adapter_monitor_destroy(kr_adapter_monitor *monitor);
kr_adapter_monitor *kr_adapter_monitor_create();
void kr_adapter_monitor_wait(kr_adapter_monitor *monitor, int ms);
