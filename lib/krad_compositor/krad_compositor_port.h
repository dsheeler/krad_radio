#ifndef KRAD_COMPOSITOR_PORT_H
#define KRAD_COMPOSITOR_PORT_H

#include "krad_radio.h"

struct krad_compositor_port_St {

	krad_compositor_t *krad_compositor;

	char sysname[256];
	int direction;
	int active;
	
	krad_frame_t *last_frame;
	krad_ringbuffer_t *frame_ring;
	
	int passthru;

	int source_width;
	int source_height;
	
	int width;
	int height;
	
	int crop_x;
	int crop_y;
	
	int crop_width;
	int crop_height;
	
	int crop_start_pixel[4];
		
	int x;
	int y;
	int z;	
	
	int new_x;
	int new_y;
	int last_x;
	int last_y;
	
	int tickrate;
	int tick;
	
	float rotation;
	float opacity;	
	
	struct SwsContext *sws_converter;	
	int yuv_color_depth;
	
	int io_params_updated;
	int comp_params_updated;
	
	uint64_t start_timecode;
	
	
	int local;
	int shm_sd;
	int msg_sd;
	char *local_buffer;
	int local_buffer_size;
	krad_frame_t *local_frame;	
	
	
};

#endif // KRAD_COMPOSITOR_PORT_H
