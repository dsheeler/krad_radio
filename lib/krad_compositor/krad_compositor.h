#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <inttypes.h>

#ifndef KRAD_COMPOSITOR_H
#define KRAD_COMPOSITOR_H

#include <libswscale/swscale.h>

typedef struct krad_compositor_port_St krad_compositor_port_t;
typedef struct krad_compositor_port_St kr_comp_port_t;
typedef struct krad_compositor_St krad_compositor_t;
typedef struct krad_compositor_St kr_compositor;

typedef struct kr_compositor_path_setup kr_compositor_path_setup;
typedef struct kr_compositor_path_frame_cb_arg kr_compositor_path_frame_cb_arg;
typedef struct kr_compositor_path_info_cb_arg kr_compositor_path_info_cb_arg;
typedef struct kr_compositor_setup kr_compositor_setup;
typedef struct kr_compositor_info_cb_arg kr_compositor_info_cb_arg;

typedef void (kr_compositor_info_cb)(kr_compositor_info_cb_arg *);
typedef void (kr_compositor_path_info_cb)(kr_compositor_path_info_cb_arg *);
typedef void (kr_compositor_path_frame_cb)(kr_compositor_path_frame_cb_arg *);

#include "krad_radio.h"
#include "krad_compositor_subunit.h"
#include "krad_videoport.h"
#include "krad_text.h"
#include "krad_compositor_interface.h"

#define DEFAULT_COMPOSITOR_BUFFER_FRAMES 120
#define KC_MAX_PORTS 32
#define KC_MAX_SPRITES 32
#define KC_MAX_TEXTS 32
#define KC_MAX_VECTORS 32
#define KC_MAX_ANY MAX(KC_MAX_PORTS, MAX(KC_MAX_SPRITES, MAX(KC_MAX_TEXTS, KC_MAX_VECTORS)))
#define KC_MAX_SUBUNITS KC_MAX_PORTS * KC_MAX_SPRITES * KC_MAX_TEXTS * KC_MAX_VECTORS

#define RED 0.244 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0
#define GREY 0.197 / 0.255 * 1.0, 0.203 / 0.255 * 1.0, 0.203 / 0.255 * 1.0
#define BGCOLOR_CLR  0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.255 / 0.255   * 1.0

struct kr_compositor_info_cb_arg {
  void *user;
};

struct kr_compositor_path_info_cb_arg {
  void *user;
};

struct kr_compositor_path_frame_cb_arg {
  uint32_t channels;
  uint32_t nframes;
  float **samples;
  void *user;
};

struct kr_compositor_setup {
  uint32_t width;
  uint32_t height;
  uint32_t fps_num;
  uint32_t fps_den;
  void *user;
  kr_compositor_info_cb *cb;
};

struct kr_compositor_path_setup {
  kr_compositor_path_info info;
  void *user;
  kr_compositor_path_frame_cb *cb;
};

struct krad_compositor_St {

  cairo_t *cr;

  krad_frame_t *frame;
  krad_framepool_t *framepool;

  int width;
  int height;
  int fps_numerator;
  int fps_denominator;

  uint64_t frames;
  uint64_t timecode;

  int frame_byte_size;

  krad_sprite_t *sprite;
  int active_sprites;

  krad_text_t *text;
  int active_texts;

  krad_vector_t *vector;
  int active_vectors;

  krad_compositor_port_t *port;
  int active_ports;
  int active_output_ports;
  int active_input_ports;

  krad_compositor_subunit_t *subunit[KC_MAX_SUBUNITS];

  krad_sprite_t *background;

  krad_ticker_t *krad_ticker;
  int ticker_running;
  int ticker_period;
  pthread_t ticker_thread;

  struct timespec start_time;
  kr_address_t address;

  FT_Library ft_library;

  int had_a_subunit;

};

int krad_compositor_subunit_create (krad_compositor_t *compositor,
                                    kr_compositor_subunit_t type,
                                    char *option,
                                    char *option2);

int krad_compositor_subunit_destroy (krad_compositor_t *compositor,
                                     kr_address_t *address);

void krad_compositor_subunit_update (krad_compositor_t *compositor,
                                     kr_unit_control_t *uc);

int krad_compositor_get_background_name (krad_compositor_t *compositor,
                                         char **filename);
void krad_compositor_set_background (krad_compositor_t *compositor,
                                     char *filename);

void krad_compositor_unset_background (krad_compositor_t *compositor);

void krad_compositor_start_ticker (krad_compositor_t *compositor);

void krad_compositor_start_ticker_at (krad_compositor_t *compositor,
                                      struct timespec start_time);

void krad_compositor_stop_ticker (krad_compositor_t *compositor);

void krad_compositor_port_destroy (krad_compositor_t *compositor,
                                   krad_compositor_port_t *port);

kr_comp_port_t *krad_compositor_port_create (krad_compositor_t *compositor,
                                             char *sysname, int direction,
                                             int width, int height);

kr_comp_port_t *krad_compositor_port_create_full (krad_compositor_t *compositor,
                                                  char *sysname, int direction,
                                                  int width, int height,
                                                  int holdlock, int local);
krad_compositor_port_t *
krad_compositor_local_port_create (krad_compositor_t *compositor,
                                   char *sysname, int direction,
                                   int shm_sd, int msg_sd);

void krad_compositor_port_set_source_size (kr_comp_port_t *port,
                                           int width, int height);

void krad_compositor_port_set_comp_params (krad_compositor_port_t *port,
                                           int x, int y,
                                           int width, int height,
                                           int crop_x, int crop_y,
                                           int crop_width, int crop_height,
                                           float opacity, float rotation);

int krad_compositor_port_get_fd (krad_compositor_port_t *port);
int krad_compositor_port_frames_avail (krad_compositor_port_t *port);

void krad_compositor_port_push_rgba_frame (kr_comp_port_t *port,
                                           krad_frame_t *krad_frame);

void krad_compositor_port_push_yuv_frame (kr_comp_port_t *port,
                                          krad_frame_t *krad_frame);

void krad_compositor_port_push_frame (kr_comp_port_t *port,
                                      krad_frame_t *krad_frame);
krad_frame_t *krad_compositor_port_pull_frame (kr_comp_port_t *port);

krad_frame_t *krad_compositor_port_pull_yuv_frame (kr_comp_port_t *port,
                                                   uint8_t *yuv_pixels[4],
                                                   int yuv_strides[4],
                                                   int color_depth);

void krad_compositor_get_frame_rate (krad_compositor_t *compositor,
                                     int *fps_numerator, int *fps_denominator);
void krad_compositor_get_resolution (krad_compositor_t *compositor,
                                     int *width, int *height);
void krad_compositor_set_resolution (krad_compositor_t *comp,
                                     uint32_t width, uint32_t height);

void krad_compositor_process(krad_compositor_t *compositor);
void kr_compositor_destroy(krad_compositor_t *compositor);

kr_compositor *kr_compositor_create(kr_compositor_setup *setup);

#endif
