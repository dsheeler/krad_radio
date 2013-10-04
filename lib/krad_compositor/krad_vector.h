#ifndef KRAD_VECTOR_H
#define KRAD_VECTOR_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include <cairo.h>

#include "krad_system.h"

typedef struct kr_vector kr_vector;

#include "krad_compositor_common.h"
#include "krad_compositor.h"

size_t kr_vector_size();
void kr_vector_clear(kr_vector *vector);
int kr_vector_init(kr_vector *vector, char *type);
void kr_vector_render(kr_vector *vector, cairo_t *cr);
int kr_vector_info_get(kr_vector *vector, kr_vector_info *info);

#endif
