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
#include "krad_compositor_common.h"
#include "krad_compositor_subunit.h"

#define PRO_REEL_SIZE 26.67
#define PRO_REEL_SPEED 38.1
#define NORMAL_REEL_SIZE 17.78
#define NORMAL_REEL_SPEED 19.05
#define SLOW_NORMAL_REEL_SPEED 9.53

#define BLUE 0.0, 0.152 / 0.255 * 1.0, 0.212 / 0.255 * 1.0
#define BLUE_TRANS 0.0, 0.152 / 0.255 * 1.0, 0.212 / 0.255 * 1.0, 0.255
#define BLUE_TRANS2 0.0, 0.152 / 0.255 * 1.0, 0.212 / 0.255 * 1.0, 0.144 / 0.255 * 1.0
#define BLUE_TRANS3 0.0, 0.122 / 0.255 * 1.0, 0.112 / 0.255 * 1.0, 0.144 / 0.255 * 1.0
#define GREEN  0.001 / 0.255 * 1.0, 0.187 / 0.255 * 1.0, 0.0
#define LGREEN  0.001 / 0.255 * 1.0, 0.187 / 0.255 * 1.0, 0.0, 0.044 / 0.255 * 1.0
#define WHITE 0.222 / 0.255 * 1.0, 0.232 / 0.255 * 1.0, 0.233 / 0.255 * 1.0
#define WHITE_TRANS 0.222 / 0.255 * 1.0, 0.232 / 0.255 * 1.0, 0.233 / 0.255 * 1.0, 0.555
#define ORANGE  0.255 / 0.255 * 1.0, 0.080 / 0.255 * 1.0, 0.0
#define GREY  0.197 / 0.255 * 1.0, 0.203 / 0.255 * 1.0, 0.203 / 0.255   * 1.0
#define GREY2  0.037 / 0.255 * 1.0, 0.037 / 0.255 * 1.0, 0.038 / 0.255   * 1.0
#define BGCOLOR  0.033 / 0.255 * 1.0, 0.033 / 0.255 * 1.0, 0.033 / 0.255   * 1.0
#define GREY3  0.103 / 0.255 * 1.0, 0.103 / 0.255 * 1.0, 0.124 / 0.255   * 1.0
#define BGCOLOR_TRANS  0.033 / 0.255 * 1.0, 0.033 / 0.255 * 1.0, 0.033 / 0.255 * 1.0, 0.144 / 0.255 * 1.0
#define BGCOLOR_CLR  0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.0 / 0.255   * 1.0, 0.255 / 0.255   * 1.0

typedef struct krad_vector_St krad_vector_t;

struct krad_vector_St {
  krad_vector_type_t type;
  krad_compositor_subunit_t subunit;
};

krad_vector_t *krad_vector_create_arr (int count);
void krad_vector_destroy_arr (krad_vector_t *vector, int count);

void krad_vector_reset (krad_vector_t *vector);
void krad_vector_set_type (krad_vector_t *vector, char *type);

void krad_vector_render (krad_vector_t *krad_vector, cairo_t *cr);

int krad_vector_to_rep (krad_vector_t *krad_vector, krad_vector_rep_t *krad_vector_rep);

#endif
