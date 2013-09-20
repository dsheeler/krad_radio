#include "krad_compositor_subunit.h"

void krad_compositor_subunit_reset(krad_compositor_subunit_t *subunit) {

  subunit->width = 0;
  subunit->height = 0;
  subunit->x = 0;
  subunit->y = 0;
  subunit->z = 0;

  subunit->red = 0.255 / 0.255 * 1.0;
  subunit->blue = 0.255 / 0.255 * 1.0;
  subunit->green = 0.255 / 0.255 * 1.0;

  subunit->opacity = 0.0f;
  //subunit->opacity = 1.0f;
  krad_compositor_subunit_set_opacity (subunit, 1.0f, 24);
  subunit->rotation = 0.0f;
}

void krad_compositor_subunit_set_x(krad_compositor_subunit_t *subunit, int x, int duration) {
  kr_easer_set(&subunit->x_easer, x, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_y (krad_compositor_subunit_t *subunit, int y, int duration) {
  kr_easer_set (&subunit->y_easer, y, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_xy (krad_compositor_subunit_t *subunit, int x, int y, int duration) {
  kr_easer_set (&subunit->x_easer, x, duration, EASEINOUTSINE, NULL);
  kr_easer_set (&subunit->y_easer, y, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_z (krad_compositor_subunit_t *subunit, int z) {
  subunit->z = z;
}

void krad_compositor_subunit_set_width (krad_compositor_subunit_t *subunit, int width, int duration) {
  kr_easer_set (&subunit->width_easer, width, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_height (krad_compositor_subunit_t *subunit, int height, int duration) {
  kr_easer_set (&subunit->height_easer, height, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_opacity (krad_compositor_subunit_t *subunit, float opacity, int duration) {
  kr_easer_set (&subunit->opacity_easer, opacity, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_rotation (krad_compositor_subunit_t *subunit, float rotation, int duration) {
  kr_easer_set (&subunit->rotation_easer, rotation, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_red (krad_compositor_subunit_t *subunit, float red, int duration) {
  kr_easer_set (&subunit->red_easer, red, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_green (krad_compositor_subunit_t *subunit, float green, int duration) {
  kr_easer_set (&subunit->green_easer, green, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_blue (krad_compositor_subunit_t *subunit, float blue, int duration) {
  kr_easer_set (&subunit->blue_easer, blue, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_alpha (krad_compositor_subunit_t *subunit, float alpha, int duration) {
  kr_easer_set (&subunit->alpha_easer, alpha, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_tick (krad_compositor_subunit_t *subunit) {
  if (kr_easer_active(&subunit->x_easer)) {
    subunit->x = kr_easer_process(&subunit->x_easer, subunit->x, NULL);
  }
  if (kr_easer_active(&subunit->y_easer)) {
    subunit->y = kr_easer_process(&subunit->y_easer, subunit->y, NULL);
  }
  if (kr_easer_active(&subunit->width_easer)) {
    subunit->width = kr_easer_process(&subunit->width_easer, subunit->width, NULL);
  }
  if (kr_easer_active(&subunit->height_easer)) {
    subunit->height = kr_easer_process(&subunit->height_easer, subunit->height, NULL);
  }
  if (kr_easer_active(&subunit->opacity_easer)) {
    subunit->opacity = kr_easer_process(&subunit->opacity_easer, subunit->opacity, NULL);
  }
  if (kr_easer_active(&subunit->rotation_easer)) {
    subunit->rotation = kr_easer_process(&subunit->rotation_easer, subunit->rotation, NULL);
  }
  if (kr_easer_active(&subunit->red_easer)) {
    subunit->red = kr_easer_process(&subunit->red_easer, subunit->red, NULL);
  }
  if (kr_easer_active(&subunit->green_easer)) {
    subunit->green = kr_easer_process(&subunit->green_easer, subunit->green, NULL);
  }
  if (kr_easer_active(&subunit->blue_easer)) {
    subunit->blue = kr_easer_process(&subunit->blue_easer, subunit->blue, NULL);
  }
  if (kr_easer_active(&subunit->alpha_easer)) {
    subunit->alpha = kr_easer_process(&subunit->alpha_easer, subunit->alpha, NULL);
  }
}
