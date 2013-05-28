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
  subunit->opacity = 0.0f;
  //subunit->opacity = 1.0f;
  krad_compositor_subunit_set_opacity (subunit, 1.0f, 24);
  subunit->rotation = 0.0f;
}

void krad_compositor_subunit_set_x (krad_compositor_subunit_t *subunit, int x, int duration) {
  krad_easing_set_new_value (&subunit->x_easing, x, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_y (krad_compositor_subunit_t *subunit, int y, int duration) {
  krad_easing_set_new_value (&subunit->y_easing, y, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_xy (krad_compositor_subunit_t *subunit, int x, int y, int duration) {
  krad_easing_set_new_value (&subunit->x_easing, x, duration, EASEINOUTSINE, NULL);
  krad_easing_set_new_value (&subunit->y_easing, y, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_z (krad_compositor_subunit_t *subunit, int z) {
  subunit->z = z;
}

void krad_compositor_subunit_set_width (krad_compositor_subunit_t *subunit, int width, int duration) {
  krad_easing_set_new_value (&subunit->width_easing, width, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_height (krad_compositor_subunit_t *subunit, int height, int duration) {
  krad_easing_set_new_value (&subunit->height_easing, height, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_xscale (krad_compositor_subunit_t *subunit, float xscale, int duration) {
  krad_easing_set_new_value (&subunit->xscale_easing, xscale, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_yscale (krad_compositor_subunit_t *subunit, float yscale, int duration) {
  krad_easing_set_new_value (&subunit->yscale_easing, yscale, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_scale (krad_compositor_subunit_t *subunit, float scale, int duration) {
  krad_easing_set_new_value (&subunit->xscale_easing, scale, duration, EASEINOUTSINE, NULL);
  krad_easing_set_new_value (&subunit->yscale_easing, scale, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_opacity (krad_compositor_subunit_t *subunit, float opacity, int duration) {
  krad_easing_set_new_value (&subunit->opacity_easing, opacity, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_rotation (krad_compositor_subunit_t *subunit, float rotation, int duration) {
  krad_easing_set_new_value (&subunit->rotation_easing, rotation, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_red (krad_compositor_subunit_t *subunit, float red, int duration) {
  krad_easing_set_new_value (&subunit->red_easing, red, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_green (krad_compositor_subunit_t *subunit, float green, int duration) {
  krad_easing_set_new_value (&subunit->green_easing, green, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_blue (krad_compositor_subunit_t *subunit, float blue, int duration) {
  krad_easing_set_new_value (&subunit->blue_easing, blue, duration, EASEINOUTSINE, NULL);
}

void krad_compositor_subunit_set_alpha (krad_compositor_subunit_t *subunit, float alpha, int duration) {
  krad_easing_set_new_value (&subunit->alpha_easing, alpha, duration, EASEINOUTSINE, NULL);
}


void krad_compositor_subunit_tick (krad_compositor_subunit_t *subunit) {
  if (subunit->x_easing.active) {
    subunit->x = krad_easing_process (&subunit->x_easing, subunit->x, NULL);
  }
  if (subunit->y_easing.active) {
    subunit->y = krad_easing_process (&subunit->y_easing, subunit->y, NULL);
  }
  if (subunit->width_easing.active) {
    subunit->width = krad_easing_process (&subunit->width_easing, subunit->width, NULL);
  }
  if (subunit->height_easing.active) {
    subunit->height = krad_easing_process (&subunit->height_easing, subunit->height, NULL);
  }
  if (subunit->xscale_easing.active) {
    subunit->xscale = krad_easing_process (&subunit->xscale_easing, subunit->xscale, NULL);
  }
  if (subunit->yscale_easing.active) {
    subunit->yscale = krad_easing_process (&subunit->yscale_easing, subunit->yscale, NULL);
  }
  if (subunit->opacity_easing.active) {
    subunit->opacity = krad_easing_process (&subunit->opacity_easing, subunit->opacity, NULL);
  }
  if (subunit->rotation_easing.active) {
    subunit->rotation = krad_easing_process (&subunit->rotation_easing, subunit->rotation, NULL);
  }
  if (subunit->red_easing.active) {
    subunit->red = krad_easing_process (&subunit->red_easing, subunit->red, NULL);
  }
  if (subunit->green_easing.active) {
    subunit->green = krad_easing_process (&subunit->green_easing, subunit->green, NULL);
  }
  if (subunit->blue_easing.active) {
    subunit->blue = krad_easing_process (&subunit->blue_easing, subunit->blue, NULL);
  }
  if (subunit->alpha_easing.active) {
    subunit->alpha = krad_easing_process (&subunit->alpha_easing, subunit->alpha, NULL);
  }
}
