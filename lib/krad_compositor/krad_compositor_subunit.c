#include "krad_compositor_subunit.h"

void krad_compositor_subunit_reset (krad_compositor_subunit_t *subunit) {
  
  
  subunit->width = 0;
  subunit->height = 0;
  subunit->x = 0;
  subunit->y = 0;
  subunit->z = 0;
  
  subunit->tickrate = KRAD_COMPOSITOR_SUBUNIT_DEFAULT_TICKRATE;
  subunit->tick = 0;

  subunit->xscale = 1.0f;
  subunit->yscale = 1.0f;
  subunit->opacity = 1.0f;
  subunit->rotation = 0.0f;
  
  
  subunit->new_x = subunit->x;
  subunit->new_y = subunit->y;
  
  subunit->new_xscale = subunit->xscale;
  subunit->new_yscale = subunit->yscale;
  subunit->new_opacity = subunit->opacity;
  subunit->new_rotation = subunit->rotation;

  subunit->start_x = subunit->x;
  subunit->start_y = subunit->y;
  subunit->change_x_amount = 0;
  subunit->change_y_amount = 0;  
  subunit->x_time = 0;
  subunit->y_time = 0;
  
  subunit->x_duration = 0;
  subunit->y_duration = 0;

  subunit->start_rotation = subunit->rotation;
  subunit->start_opacity = subunit->opacity;
  subunit->start_xscale = subunit->xscale;
  subunit->start_yscale = subunit->yscale;  
  
  subunit->rotation_change_amount = 0;
  subunit->opacity_change_amount = 0;
  subunit->xscale_change_amount = 0;
  subunit->yscale_change_amount = 0;
  
  subunit->rotation_time = 0;
  subunit->opacity_time = 0;
  subunit->xscale_time = 0;
  subunit->yscale_time = 0;
  
  subunit->rotation_duration = 0;
  subunit->opacity_duration = 0;
  subunit->xscale_duration = 0;
  subunit->yscale_duration = 0;  
  
  subunit->krad_ease_x = krad_ease_random();
  subunit->krad_ease_y = krad_ease_random();
  subunit->krad_ease_xscale = krad_ease_random();
  subunit->krad_ease_yscale = krad_ease_random();
  subunit->krad_ease_rotation = krad_ease_random();
  subunit->krad_ease_opacity = krad_ease_random();
  
}

void krad_compositor_subunit_set_xy (krad_compositor_subunit_t *subunit, int x, int y) {

  subunit->x = x;
  subunit->y = y;
  
  subunit->new_x = subunit->x;
  subunit->new_y = subunit->y;
  
}

void krad_compositor_subunit_set_z (krad_compositor_subunit_t *subunit, int z) {

  subunit->z = z;

}

void krad_compositor_subunit_set_new_xy (krad_compositor_subunit_t *subunit, int x, int y) {
  subunit->x_duration = 60;
  subunit->x_time = 0;
  subunit->y_duration = 60;
  subunit->y_time = 0;
  subunit->start_x = subunit->x;
  subunit->start_y = subunit->y;
  subunit->change_x_amount = x - subunit->start_x;
  subunit->change_y_amount = y - subunit->start_y;
  subunit->krad_ease_x = EASEINOUTSINE;
  subunit->krad_ease_y = EASEINOUTSINE;    
  subunit->new_x = x;
  subunit->new_y = y;
}

void krad_compositor_subunit_set_scale (krad_compositor_subunit_t *subunit, float scale) {
  subunit->xscale = scale;
  subunit->yscale = scale;
  subunit->new_xscale = subunit->xscale;
  subunit->new_yscale = subunit->yscale;
}

void krad_compositor_subunit_set_xscale (krad_compositor_subunit_t *subunit, float scale) {
  subunit->xscale = scale;
  subunit->new_xscale = subunit->xscale;
}

void krad_compositor_subunit_set_yscale (krad_compositor_subunit_t *subunit, float scale) {
  subunit->yscale = scale;
  subunit->new_yscale = subunit->yscale;
}

void krad_compositor_subunit_set_opacity (krad_compositor_subunit_t *subunit, float opacity) {
  subunit->opacity = opacity;
  subunit->new_opacity = subunit->opacity;
}

void krad_compositor_subunit_set_rotation (krad_compositor_subunit_t *subunit, float rotation) {
  subunit->rotation = rotation;
  subunit->new_rotation = subunit->rotation;
}

void krad_compositor_subunit_set_tickrate (krad_compositor_subunit_t *subunit, int tickrate) {
  subunit->tickrate = tickrate;
}

void krad_compositor_subunit_set_new_scale (krad_compositor_subunit_t *subunit, float scale) {
  subunit->xscale_duration = rand() % 100 + 10;
  subunit->xscale_time = 0;
  subunit->yscale_duration = rand() % 100 + 10;
  subunit->yscale_time = 0;  
  subunit->start_xscale = subunit->xscale;
  subunit->start_yscale = subunit->yscale;
  subunit->xscale_change_amount = scale - subunit->start_xscale;
  subunit->yscale_change_amount = scale - subunit->start_yscale;
  subunit->krad_ease_xscale = krad_ease_random();
  subunit->krad_ease_yscale = krad_ease_random();    
  subunit->new_xscale = scale;
  subunit->new_yscale = scale;  
}

void krad_compositor_subunit_set_new_xscale (krad_compositor_subunit_t *subunit, float scale) {
  subunit->xscale_duration = rand() % 100 + 10;
  subunit->xscale_time = 0;
  subunit->start_xscale = subunit->xscale;
  subunit->xscale_change_amount = scale - subunit->start_xscale;
  subunit->krad_ease_xscale = krad_ease_random();    
  subunit->new_xscale = scale;
}

void krad_compositor_subunit_set_new_yscale (krad_compositor_subunit_t *subunit, float scale) {
  subunit->yscale_duration = rand() % 100 + 10;
  subunit->yscale_time = 0;
  subunit->start_yscale = subunit->yscale;
  subunit->yscale_change_amount = scale - subunit->start_yscale;
  subunit->krad_ease_yscale = krad_ease_random();  
  subunit->new_yscale = scale;  
}

void krad_compositor_subunit_set_new_opacity (krad_compositor_subunit_t *subunit, float opacity) {
  subunit->opacity_duration = 60;
  subunit->opacity_time = 0;
  subunit->start_opacity = subunit->opacity;
  subunit->opacity_change_amount = opacity - subunit->start_opacity;
  subunit->krad_ease_opacity = EASEINOUTSINE;  
  subunit->new_opacity = opacity;
}

void krad_compositor_subunit_set_new_rotation (krad_compositor_subunit_t *subunit, float rotation) {
  subunit->rotation_duration = 60;
  subunit->rotation_time = 0;
  subunit->start_rotation = subunit->rotation;
  subunit->rotation_change_amount = rotation - subunit->start_rotation;
  subunit->krad_ease_rotation = EASEINOUTSINE;
  subunit->new_rotation = rotation;
}


void krad_compositor_subunit_update (krad_compositor_subunit_t *subunit) {

  if (subunit->x_time != subunit->x_duration) {
    subunit->x = krad_ease (subunit->krad_ease_x, subunit->x_time++, subunit->start_x, subunit->change_x_amount, subunit->x_duration);

    if (subunit->x_time == subunit->x_duration) {
      subunit->x = subunit->new_x;
    }
  }  
  
  if (subunit->y_time != subunit->y_duration) {
    subunit->y = krad_ease (subunit->krad_ease_y, subunit->y_time++, subunit->start_y, subunit->change_y_amount, subunit->y_duration);
    if (subunit->y_time == subunit->y_duration) {
      subunit->y = subunit->new_y;
    }
  }
  
  if (subunit->rotation_time != subunit->rotation_duration) {
    subunit->rotation = krad_ease (subunit->krad_ease_rotation, subunit->rotation_time++, subunit->start_rotation, subunit->rotation_change_amount, subunit->rotation_duration);
    if (subunit->rotation_time == subunit->rotation_duration) {
      subunit->rotation = subunit->new_rotation;
    }
  }
  
  if (subunit->opacity_time != subunit->opacity_duration) {
    subunit->opacity = krad_ease (subunit->krad_ease_opacity, subunit->opacity_time++, subunit->start_opacity, subunit->opacity_change_amount, subunit->opacity_duration);
    if (subunit->opacity_time == subunit->opacity_duration) {
      subunit->opacity = subunit->new_opacity;
    }
  }
  
  if (subunit->xscale_time != subunit->xscale_duration) {
    subunit->xscale = krad_ease (subunit->krad_ease_xscale, subunit->xscale_time++, subunit->start_xscale, subunit->xscale_change_amount, subunit->xscale_duration);
    if (subunit->xscale_time == subunit->xscale_duration) {
      subunit->xscale = subunit->new_xscale;
    }
  }
  
  if (subunit->yscale_time != subunit->yscale_duration) {
    subunit->yscale = krad_ease (subunit->krad_ease_yscale, subunit->yscale_time++, subunit->start_yscale, subunit->yscale_change_amount, subunit->yscale_duration);
    if (subunit->yscale_time == subunit->yscale_duration) {
      subunit->yscale = subunit->new_yscale;
    }
  }
}


