#ifndef KRAD_COMPOSITOR_SUBUNIT_H
#define KRAD_COMPOSITOR_SUBUNIT_H

#include "krad_radio_client.h"
#include "krad_easing.h"

typedef struct kr_compositor_subunit krad_compositor_subunit_t;
typedef struct kr_compositor_control_easers kr_compositor_control_easers;

struct kr_compositor_subunit {

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

  float red;
  float green;
  float blue;
  float alpha;

  kr_easer x_easer;
  kr_easer y_easer;
  kr_easer width_easer;
  kr_easer height_easer;
  kr_easer rotation_easer;
  kr_easer opacity_easer;
  kr_easer red_easer;
  kr_easer green_easer;
  kr_easer blue_easer;
  kr_easer alpha_easer;
};

struct kr_compositor_control_easers {
  kr_easer x;
  kr_easer y;
  kr_easer w;
  kr_easer h;
  kr_easer rotation;
  kr_easer opacity;
};

void krad_compositor_subunit_reset(krad_compositor_subunit_t *subunit);
void krad_compositor_subunit_tick(krad_compositor_subunit_t *subunit);
void krad_compositor_subunit_set_x(krad_compositor_subunit_t *subunit, int x, int duration);
void krad_compositor_subunit_set_y(krad_compositor_subunit_t *subunit, int y, int duration);
void krad_compositor_subunit_set_xy(krad_compositor_subunit_t *subunit, int x, int y, int duration);
void krad_compositor_subunit_set_z(krad_compositor_subunit_t *subunit, int z);
void krad_compositor_subunit_set_width(krad_compositor_subunit_t *subunit, int width, int duration);
void krad_compositor_subunit_set_height(krad_compositor_subunit_t *subunit, int height, int duration);
void krad_compositor_subunit_set_opacity(krad_compositor_subunit_t *subunit, float opacity, int duration);
void krad_compositor_subunit_set_rotation(krad_compositor_subunit_t *subunit, float rotation, int duration);
void krad_compositor_subunit_set_red(krad_compositor_subunit_t *subunit, float red, int duration);
void krad_compositor_subunit_set_green(krad_compositor_subunit_t *subunit, float green, int duration);
void krad_compositor_subunit_set_blue(krad_compositor_subunit_t *subunit, float blue, int duration);
void krad_compositor_subunit_set_alpha(krad_compositor_subunit_t *subunit, float alpha, int duration);

#endif
