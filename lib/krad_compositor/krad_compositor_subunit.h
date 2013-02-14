#ifndef KRAD_COMPOSITOR_SUBUNIT_H
#define KRAD_COMPOSITOR_SUBUNIT_H

#include "krad_radio_client.h"
#include "krad_easing.h"

typedef struct krad_compositor_subunit_St krad_compositor_subunit_t;

struct krad_compositor_subunit_St {

  kr_compositor_subunit_t type;
  kr_address_t address;
  
  int active;

  int x;
  int y;
  int z;  

  int width;
  int height;

  float rotation;
  float opacity;

  float xscale;
  float yscale;
};


void krad_compositor_subunit_reset (krad_compositor_subunit_t *subunit);
void krad_compositor_subunit_tick (krad_compositor_subunit_t *subunit);

void krad_compositor_subunit_set_x (krad_compositor_subunit_t *subunit, int x, int duration);
void krad_compositor_subunit_set_y (krad_compositor_subunit_t *subunit, int y, int duration);
void krad_compositor_subunit_set_xy (krad_compositor_subunit_t *subunit, int x, int y, int duration);
void krad_compositor_subunit_set_z (krad_compositor_subunit_t *subunit, int z);
void krad_compositor_subunit_set_opacity (krad_compositor_subunit_t *subunit, float opacity, int duration);
void krad_compositor_subunit_set_rotation (krad_compositor_subunit_t *subunit, float rotation, int duration);
void krad_compositor_subunit_set_xscale (krad_compositor_subunit_t *subunit, float xscale, int duration);
void krad_compositor_subunit_set_yscale (krad_compositor_subunit_t *subunit, float yscale, int duration);
void krad_compositor_subunit_set_scale (krad_compositor_subunit_t *subunit, float scale, int duration);


#endif // KRAD_COMPOSITOR_SUBUNIT_H
