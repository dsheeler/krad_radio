
#include "krad_compositor_common.h"
#include "krad_ebml.h"
#include "krad_radio.h"

kr_compositor_subunit_controls_t *krad_compositor_read_subunit_controls (krad_ebml_t *krad_ebml) {
  uint32_t ebml_id;
	
	uint64_t ebml_data_size;
  
  kr_compositor_subunit_controls_t *subunit_controls = krad_compositor_subunit_controls_create();
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_X) {
    subunit_controls->x = krad_ebml_read_number (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_Y) {
    subunit_controls->y = krad_ebml_read_number (krad_ebml, ebml_data_size);
  }
      
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_Y) {
    subunit_controls->z = krad_ebml_read_number (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_SPRITE_TICKRATE) {
    subunit_controls->tickrate = krad_ebml_read_number (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_SPRITE_SCALE) {
    subunit_controls->xscale = subunit_controls->yscale = krad_ebml_read_float (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY) {
    subunit_controls->opacity = krad_ebml_read_float (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION) {
    subunit_controls->rotation = krad_ebml_read_float (krad_ebml, ebml_data_size);
  }		
  return subunit_controls;
}

void krad_compositor_subunit_controls_destroy (kr_compositor_subunit_controls_t *krad_compositor_subunit_controls) {
  free (krad_compositor_subunit_controls);
}

kr_compositor_subunit_controls_t *krad_compositor_subunit_controls_create () {

	kr_compositor_subunit_controls_t *krad_compositor_subunit_controls;

	krad_compositor_subunit_controls = calloc (1, sizeof (kr_compositor_subunit_controls_t));
	
	krad_compositor_subunit_controls_reset (krad_compositor_subunit_controls);
	
	return krad_compositor_subunit_controls;

}

kr_compositor_subunit_controls_t *krad_compositor_subunit_controls_create_and_init (int x, int y, int z, int tickrate, int width, int height,
                                                                                    float scale, float opacity, float rotation) {

	kr_compositor_subunit_controls_t *krad_compositor_subunit_controls;

	krad_compositor_subunit_controls = calloc (1, sizeof (kr_compositor_subunit_controls_t));
	
	krad_compositor_subunit_controls_reset (krad_compositor_subunit_controls);
  
  krad_compositor_subunit_controls->x = x;
	krad_compositor_subunit_controls->y = y;
  krad_compositor_subunit_controls->z = z;
  
  krad_compositor_subunit_controls->tickrate = tickrate;
  
//  krad_compositor_subunit_controls->width = width;
//  krad_compositor_subunit_controls->height = height;
  
  krad_compositor_subunit_controls->xscale = scale;
  krad_compositor_subunit_controls->yscale = scale;
  
  krad_compositor_subunit_controls->rotation = rotation;
  krad_compositor_subunit_controls->opacity = opacity;

  return krad_compositor_subunit_controls;

}

void krad_compositor_subunit_controls_reset (kr_compositor_subunit_controls_t *krad_compositor_subunit_controls) {
  krad_compositor_subunit_controls->x = 0;
	krad_compositor_subunit_controls->y = 0;
  krad_compositor_subunit_controls->z = 0;
  
  krad_compositor_subunit_controls->tickrate = KRAD_COMPOSITOR_SUBUNIT_DEFAULT_TICKRATE;
  
  krad_compositor_subunit_controls->width = 0;
  krad_compositor_subunit_controls->height = 0;
  
  krad_compositor_subunit_controls->xscale = 1.0f;
  krad_compositor_subunit_controls->yscale = 1.0f;
  
  krad_compositor_subunit_controls->rotation = 0.0f;
  krad_compositor_subunit_controls->opacity = 0.0f;

}

int krad_compositor_validate_text_rep (krad_text_rep_t *krad_text_rep) {
  
  if ((krad_text_rep->red < 0.0) || (krad_text_rep->red > 255.0)) {
    return 1;
  }
  return 0;
}
