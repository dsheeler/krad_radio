#ifndef KRAD_COMPOSITOR_COMMON_H
#define KRAD_COMPOSITOR_COMMON_H


#include <stdint.h>
#include "krad_ebml.h"

typedef struct krad_text_rep_St krad_text_rep_t;
typedef struct krad_sprite_rep_St krad_sprite_rep_t;
typedef struct kr_compositor_subunit_controls_St kr_compositor_subunit_controls_t;
typedef struct krad_compositor_rep_St krad_compositor_rep_t;

struct kr_compositor_subunit_controls_St {

	int x;
	int y;
	int z;

  int tickrate;
  
	int width;
	int height;

	float xscale;
	float yscale;

	float rotation;
	float opacity;

};

struct krad_sprite_rep_St {
	char filename[256];
	kr_compositor_subunit_controls_t *controls;
};

struct krad_text_rep_St {
	
	char text[1024];
	char font[128];
	
	float red;
	float green;
	float blue;
	
	kr_compositor_subunit_controls_t *controls;
};

struct krad_compositor_rep_St {
	
	uint32_t width;
	uint32_t height;
	uint32_t fps_numerator;
	uint32_t fps_denominator;
	uint64_t current_frame_number;
	
};


kr_compositor_subunit_controls_t *krad_compositor_subunit_controls_create ();
kr_compositor_subunit_controls_t *krad_compositor_subunit_controls_create_and_init ( int x, int y, int z, int tickrate, int width, int height, float scale, float opacity, float rotation);
void krad_compositor_subunit_controls_reset (kr_compositor_subunit_controls_t *krad_compositor_subunit_controls);
void krad_compositor_subunit_controls_destroy (kr_compositor_subunit_controls_t *krad_compositor_subunit_controls);
void krad_compositor_subunit_controls_to_ebml (krad_ebml_t *krad_ebml, kr_compositor_subunit_controls_t *krad_compositor_subunit_controls);
kr_compositor_subunit_controls_t *krad_compositor_read_subunit_controls (krad_ebml_t *krad_ebml);
int krad_compositor_validate_text_rep (krad_text_rep_t *krad_text_rep);

#endif // KRAD_COMPOSITOR_COMMON_H
