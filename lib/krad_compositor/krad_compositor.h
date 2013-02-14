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
typedef struct krad_compositor_St krad_compositor_t;
typedef struct krad_compositor_snapshot_St krad_compositor_snapshot_t;

#include "krad_radio.h"
#include "krad_compositor_subunit.h"
#include "krad_compositor_port.h"
#include "krad_compositor_interface.h"

#define DEFAULT_COMPOSITOR_BUFFER_FRAMES 120
#define KC_MAX_PORTS 32
#define KC_MAX_SPRITES 64
#define KC_MAX_TEXTS 64
#define KC_MAX_VECTORS 64
#define KC_MAX_SUBUNITS KC_MAX_PORTS * KC_MAX_SPRITES * KC_MAX_TEXTS * KC_MAX_VECTORS

#define RED 0.244 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0
#define GREY 0.197 / 0.255 * 1.0, 0.203 / 0.255 * 1.0, 0.203 / 0.255 * 1.0
#define BGCOLOR_CLR  0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.255 / 0.255   * 1.0

typedef enum {
  SYNTHETIC = 13999,
  WAYLAND,
} krad_display_api_t;

typedef enum {
  SNAPJPEG = 20000,  
  SNAPPNG,
} krad_snapshot_fmt_t;

struct krad_compositor_snapshot_St {

  int jpeg;
  krad_frame_t *krad_frame;
  char filename[512];

  int width;
  int height;

  krad_compositor_t *krad_compositor;
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
  char *dir;
  
  krad_sprite_t *sprite;
  int active_sprites;

  krad_text_t *text;
  int active_texts;
  
  krad_compositor_port_t *port;
  int active_ports;
  int active_output_ports;
  int active_input_ports;

  krad_sprite_t *background;

  int snapshot;
  int snapshot_jpeg;  
  pthread_t snapshot_thread;
  char last_snapshot_name[512];

  int display_open;
  pthread_t display_thread;

  krad_display_api_t pusher;
  krad_ticker_t *krad_ticker;
  int ticker_running;
  int ticker_period;
  pthread_t ticker_thread;

  struct timespec start_time;
  kr_address_t address;
};

void krad_compositor_subunit_create (krad_compositor_t *compositor,
                                     kr_compositor_subunit_t type,
                                     char *option);
void krad_compositor_subunit_destroy (krad_compositor_t *compositor, kr_address_t *address);
//void krad_compositor_subunit_update (krad_compositor_t *compositor, kr_address_t *address);

int krad_compositor_get_background_name (krad_compositor_t *krad_compositor, char *filename);
void krad_compositor_set_background (krad_compositor_t *krad_compositor, char *filename);
void krad_compositor_unset_background (krad_compositor_t *krad_compositor);

void krad_compositor_start_ticker (krad_compositor_t *krad_compositor);
void krad_compositor_start_ticker_at (krad_compositor_t *krad_compositor, struct timespec start_time);
void krad_compositor_stop_ticker (krad_compositor_t *krad_compositor);

krad_display_api_t krad_compositor_get_pusher (krad_compositor_t *krad_compositor);
int krad_compositor_has_pusher (krad_compositor_t *krad_compositor);
void krad_compositor_set_pusher (krad_compositor_t *krad_compositor, krad_display_api_t pusher);
void krad_compositor_unset_pusher (krad_compositor_t *krad_compositor);

void krad_compositor_update_resolution (krad_compositor_t *krad_compositor, int width, int height);
void krad_compositor_set_frame_rate (krad_compositor_t *krad_compositor,
                                     int fps_numerator, int fps_denominator);


void krad_compositor_port_destroy (krad_compositor_t *krad_compositor, krad_compositor_port_t *krad_compositor_port);
krad_compositor_port_t *krad_compositor_port_create (krad_compositor_t *krad_compositor, char *sysname, int direction,
                                                     int width, int height);
krad_compositor_port_t *krad_compositor_port_create_full (krad_compositor_t *krad_compositor, char *sysname, int direction,
                                                          int width, int height, int holdlock, int local);                           
krad_compositor_port_t *krad_compositor_local_port_create (krad_compositor_t *krad_compositor,
                                                           char *sysname, int direction, int shm_sd, int msg_sd);
void krad_compositor_port_set_io_params (krad_compositor_port_t *krad_compositor_port,
                                         int width, int height);
void krad_compositor_port_set_comp_params (krad_compositor_port_t *krad_compositor_port,
                                           int x, int y, int width, int height, 
                                           int crop_x, int crop_y,
                                           int crop_width, int crop_height, float opacity, float rotation);
int krad_compositor_port_get_fd (krad_compositor_port_t *krad_compositor_port);
int krad_compositor_port_frames_avail (krad_compositor_port_t *krad_compositor_port);
void krad_compositor_port_push_rgba_frame (krad_compositor_port_t *krad_compositor_port, krad_frame_t *krad_frame);
void krad_compositor_port_push_yuv_frame (krad_compositor_port_t *krad_compositor_port, krad_frame_t *krad_frame);
void krad_compositor_port_push_frame (krad_compositor_port_t *krad_compositor_port, krad_frame_t *krad_frame);
krad_frame_t *krad_compositor_port_pull_frame (krad_compositor_port_t *krad_compositor_port);
krad_frame_t *krad_compositor_port_pull_yuv_frame (krad_compositor_port_t *krad_compositor_port,
                                                   uint8_t *yuv_pixels[4], int yuv_strides[4], int color_depth);

void krad_compositor_get_frame_rate (krad_compositor_t *krad_compositor,
                                     int *fps_numerator, int *fps_denominator);
void krad_compositor_get_resolution (krad_compositor_t *compositor, int *width, int *height);

void krad_compositor_get_last_snapshot_name (krad_compositor_t *krad_compositor, char *filename);
void krad_compositor_take_snapshot (krad_compositor_t *krad_compositor, krad_frame_t *krad_frame, krad_snapshot_fmt_t format);
void krad_compositor_set_dir (krad_compositor_t *krad_compositor, char *dir);

void krad_compositor_close_display (krad_compositor_t *krad_compositor);
void krad_compositor_open_display (krad_compositor_t *krad_compositor);

void krad_compositor_process (krad_compositor_t *compositor);
void krad_compositor_destroy (krad_compositor_t *compositor);
krad_compositor_t *krad_compositor_create (int width, int height, int fps_numerator, int fps_denominator);

#endif
