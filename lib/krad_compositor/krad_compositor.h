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

//#include "pixman.h"

typedef struct krad_compositor_port_St krad_compositor_port_t;
typedef struct krad_compositor_St krad_compositor_t;
typedef struct krad_compositor_snapshot_St krad_compositor_snapshot_t;

#include "krad_radio.h"
#include "krad_compositor_port.h"

#define DEFAULT_COMPOSITOR_BUFFER_FRAMES 120
#define KC_MAX_PORTS 32
#define KC_MAX_SPRITES 64
#define KC_MAX_TEXTS 64
#define KC_MAX_VECTORS 64
#define KC_MAX_SUBUNITS KC_MAX_PORTS * KC_MAX_SPRITES * KC_MAX_TEXTS * KC_MAX_VECTORS

#define GREY  0.197 / 0.255 * 1.0, 0.203 / 0.255 * 1.0, 0.203 / 0.255   * 1.0
#define BGCOLOR_CLR  0.0 / 0.255 * 1.0, 0.0 / 0.255 * 1.0, 0.0 / 0.255   * 1.0, 0.255 / 0.255   * 1.0

typedef enum {
	SYNTHETIC = 13999,
	WAYLAND,
} krad_display_api_t;

typedef enum {
	SNAPJPEG = 20000,	
	SNAPPNG,
} krad_snapshot_fmt_t;

typedef struct krad_point_St krad_point_t;

struct krad_point_St {
	int x;
	int y;
};

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

	cairo_surface_t *mask_cst;
	cairo_t *mask_cr;

  krad_text_rep_t *krad_text_rep_ipc;
  
	int width;
	int height;

	int frame_byte_size;

	int frame_rate_numerator;
	int frame_rate_denominator;

	char *dir;
	
	int snapshot;
	int snapshot_jpeg;	
	pthread_t snapshot_thread;
	char last_snapshot_name[1024];
	pthread_mutex_t last_snapshot_name_lock;

	krad_mixer_t *krad_mixer;

	krad_framepool_t *krad_framepool;

	krad_compositor_port_t *port;
	
	pthread_mutex_t settings_lock;
	
	int display_width;
	int display_height;
	int display_open;
	pthread_t display_thread;

	int active_passthru_ports;

	int skipped_processes;

	int active_ports;
	int active_output_ports;
	int active_input_ports;		

	krad_display_api_t pusher;
	krad_ticker_t *krad_ticker;
	int ticker_running;
	int ticker_period;
	pthread_t ticker_thread;

	uint64_t no_input;
	uint64_t frame_num;
	uint64_t timecode;

	krad_sprite_t *background;

	krad_sprite_t *krad_sprite;
	int active_sprites;

	krad_text_t *krad_text;
	int active_texts;
	
	struct timespec start_time;
	
  krad_compositor_subunit_t *subunit_z_order[KC_MAX_SUBUNITS];

};
void krad_compositor_unset_krad_mixer (krad_compositor_t *krad_compositor);
void krad_compositor_set_krad_mixer (krad_compositor_t *krad_compositor, krad_mixer_t *krad_mixer);

void krad_compositor_get_last_snapshot_name (krad_compositor_t *krad_compositor, char *filename);

//void krad_compositor_create_keystone_matrix (krad_point_t q[4], double w, double h, pixman_transform_t *transform);

void krad_compositor_add_text (krad_compositor_t *krad_compositor, krad_text_rep_t *krad_text_rep);

void krad_compositor_set_text (krad_compositor_t *krad_compositor, krad_text_rep_t *krad_text_rep);

void krad_compositor_remove_text (krad_compositor_t *krad_compositor, int num);

void krad_compositor_add_sprite (krad_compositor_t *krad_compositor, krad_sprite_rep_t *krad_sprite_rep);

void krad_compositor_set_sprite (krad_compositor_t *krad_compositor,
                                 krad_sprite_rep_t *krad_sprite_rep);

void krad_compositor_remove_sprite (krad_compositor_t *krad_compositor, int num);

void krad_compositor_set_background (krad_compositor_t *krad_compositor, char *filename);
void krad_compositor_unset_background (krad_compositor_t *krad_compositor);
void krad_compositor_render_background (krad_compositor_t *krad_compositor);

void krad_compositor_aspect_scale (int width, int height,
								   int avail_width, int avail_height,
								   int *new_width, int *new_heigth);

void krad_compositor_free_resources (krad_compositor_t *krad_compositor);
void krad_compositor_alloc_resources (krad_compositor_t *krad_compositor);

void *krad_compositor_ticker_thread (void *arg);
void krad_compositor_start_ticker (krad_compositor_t *krad_compositor);
void krad_compositor_start_ticker_at (krad_compositor_t *krad_compositor, struct timespec start_time);
void krad_compositor_stop_ticker (krad_compositor_t *krad_compositor);

krad_display_api_t krad_compositor_get_pusher (krad_compositor_t *krad_compositor);
int krad_compositor_has_pusher (krad_compositor_t *krad_compositor);
void krad_compositor_set_pusher (krad_compositor_t *krad_compositor, krad_display_api_t pusher);
void krad_compositor_unset_pusher (krad_compositor_t *krad_compositor);

void krad_compositor_update_resolution (krad_compositor_t *krad_compositor, int width, int height);
void krad_compositor_set_resolution (krad_compositor_t *krad_compositor, int width, int height);
void krad_compositor_set_frame_rate (krad_compositor_t *krad_compositor,
									 int frame_rate_numerator, int frame_rate_denominator);


void krad_compositor_set_peak (krad_compositor_t *krad_compositor, int channel, float value);

void krad_compositor_port_set_io_params (krad_compositor_port_t *krad_compositor_port,
										   int width, int height);

void krad_compositor_port_set_comp_params (krad_compositor_port_t *krad_compositor_port,
										   int x, int y, int width, int height, 
										   int crop_x, int crop_y,
										   int crop_width, int crop_height, float opacity, float rotation);

void krad_compositor_port_push_rgba_frame (krad_compositor_port_t *krad_compositor_port, krad_frame_t *krad_frame);
void krad_compositor_port_push_yuv_frame (krad_compositor_port_t *krad_compositor_port, krad_frame_t *krad_frame);
void krad_compositor_port_push_frame (krad_compositor_port_t *krad_compositor_port, krad_frame_t *krad_frame);
krad_frame_t *krad_compositor_port_pull_frame (krad_compositor_port_t *krad_compositor_port);

krad_frame_t *krad_compositor_port_pull_yuv_frame (krad_compositor_port_t *krad_compositor_port,
												   uint8_t *yuv_pixels[4], int yuv_strides[4], int color_depth);

int krad_compositor_port_frames_avail (krad_compositor_port_t *krad_compositor_port);

krad_compositor_port_t *krad_compositor_passthru_port_create (krad_compositor_t *krad_compositor, char *sysname, int direction);
krad_compositor_port_t *krad_compositor_port_create (krad_compositor_t *krad_compositor, char *sysname, int direction,
													 int width, int height);
krad_compositor_port_t *krad_compositor_port_create_full (krad_compositor_t *krad_compositor, char *sysname, int direction,
													 int width, int height, int holdlock, int local);													 
void krad_compositor_port_destroy (krad_compositor_t *krad_compositor, krad_compositor_port_t *krad_compositor_port);
void krad_compositor_port_destroy_unlocked (krad_compositor_t *krad_compositor, krad_compositor_port_t *krad_compositor_port);
int krad_compositor_handler ( krad_compositor_t *krad_compositor, krad_ipc_server_t *krad_ipc );

void krad_compositor_get_frame_rate (krad_compositor_t *krad_compositor,
									 int *frame_rate_numerator, int *frame_rate_denominator);

void krad_compositor_get_resolution (krad_compositor_t *compositor, int *width, int *height);
void krad_compositor_passthru_process (krad_compositor_t *krad_compositor);
void krad_compositor_process (krad_compositor_t *compositor);
void krad_compositor_destroy (krad_compositor_t *compositor);
krad_compositor_t *krad_compositor_create (int width, int height,
										   int frame_rate_numerator, int frame_rate_denominator);


void krad_compositor_take_snapshot (krad_compositor_t *krad_compositor, krad_frame_t *krad_frame, krad_snapshot_fmt_t format);
void *krad_compositor_snapshot_thread (void *arg);


void krad_compositor_set_dir (krad_compositor_t *krad_compositor, char *dir);

#endif
