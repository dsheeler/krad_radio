#include "krad_compositor_subunit.h"

void krad_compositor_subunit_reset (krad_compositor_subunit_t *subunit) {

  subunit->width = 0;
  subunit->height = 0;
  subunit->x = 0;
  subunit->y = 0;
  subunit->z = 0;

  subunit->red = 0.255 / 0.255 * 1.0;
  subunit->blue = 0.255 / 0.255 * 1.0;
  subunit->green = 0.255 / 0.255 * 1.0;

  subunit->xscale = 1.0f;
  subunit->yscale = 1.0f;
  //subunit->opacity = 0.0f;
  subunit->opacity = 1.0f;
  subunit->rotation = 0.0f;
}

void krad_compositor_subunit_set_x (krad_compositor_subunit_t *subunit, int x, int duration) {
  subunit->x = x;
}

void krad_compositor_subunit_set_y (krad_compositor_subunit_t *subunit, int y, int duration) {
  subunit->y = y;
}

void krad_compositor_subunit_set_xy (krad_compositor_subunit_t *subunit, int x, int y, int duration) {
  subunit->x = x;
  subunit->y = y;
}

void krad_compositor_subunit_set_z (krad_compositor_subunit_t *subunit, int z) {
  subunit->z = z;
}

void krad_compositor_subunit_set_xscale (krad_compositor_subunit_t *subunit, float xscale, int duration) {
  subunit->xscale = xscale;
}

void krad_compositor_subunit_set_yscale (krad_compositor_subunit_t *subunit, float yscale, int duration) {
  subunit->yscale = yscale;
}

void krad_compositor_subunit_set_scale (krad_compositor_subunit_t *subunit, float scale, int duration) {
  subunit->xscale = scale;
}

void krad_compositor_subunit_set_opacity (krad_compositor_subunit_t *subunit, float opacity, int duration) {
  subunit->opacity = opacity;
}

void krad_compositor_subunit_set_rotation (krad_compositor_subunit_t *subunit, float rotation, int duration) {
  subunit->rotation = rotation;
}

void krad_compositor_subunit_tick (krad_compositor_subunit_t *subunit) {

}
