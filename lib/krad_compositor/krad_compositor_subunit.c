#include "krad_compositor_subunit.h"

krad_compositor_subunit_t *krad_compositor_subunit_create () {

	krad_compositor_subunit_t *krad_compositor_subunit;

	if ((krad_compositor_subunit = calloc (1, sizeof (krad_compositor_subunit_t))) == NULL) {
		failfast ("Krad compositor_subunit mem alloc fail");
	}
	
	krad_compositor_subunit_reset (krad_compositor_subunit);
	
	return krad_compositor_subunit;

}

void krad_compositor_subunit_reset (krad_compositor_subunit_t *krad_compositor_subunit) {
	
	
	krad_compositor_subunit->width = 0;
	krad_compositor_subunit->height = 0;
	krad_compositor_subunit->x = 0;
	krad_compositor_subunit->y = 0;
	krad_compositor_subunit->tickrate = KRAD_COMPOSITOR_SUBUNIT_DEFAULT_TICKRATE;
	krad_compositor_subunit->tick = 0;
	krad_compositor_subunit->frame = 0;

	krad_compositor_subunit->xscale = 1.0f;
	krad_compositor_subunit->yscale = 1.0f;
	krad_compositor_subunit->opacity = 1.0f;
	krad_compositor_subunit->rotation = 0.0f;
	
	
	krad_compositor_subunit->new_x = krad_compositor_subunit->x;
	krad_compositor_subunit->new_y = krad_compositor_subunit->y;
	
	krad_compositor_subunit->new_xscale = krad_compositor_subunit->xscale;
	krad_compositor_subunit->new_yscale = krad_compositor_subunit->yscale;
	krad_compositor_subunit->new_opacity = krad_compositor_subunit->opacity;
	krad_compositor_subunit->new_rotation = krad_compositor_subunit->rotation;

	krad_compositor_subunit->start_x = krad_compositor_subunit->x;
	krad_compositor_subunit->start_y = krad_compositor_subunit->y;
	krad_compositor_subunit->change_x_amount = 0;
	krad_compositor_subunit->change_y_amount = 0;	
	krad_compositor_subunit->x_time = 0;
	krad_compositor_subunit->y_time = 0;
	
	krad_compositor_subunit->x_duration = 0;
	krad_compositor_subunit->y_duration = 0;

	krad_compositor_subunit->start_rotation = krad_compositor_subunit->rotation;
	krad_compositor_subunit->start_opacity = krad_compositor_subunit->opacity;
	krad_compositor_subunit->start_xscale = krad_compositor_subunit->xscale;
	krad_compositor_subunit->start_yscale = krad_compositor_subunit->yscale;	
	
	krad_compositor_subunit->rotation_change_amount = 0;
	krad_compositor_subunit->opacity_change_amount = 0;
	krad_compositor_subunit->xscale_change_amount = 0;
	krad_compositor_subunit->yscale_change_amount = 0;
	
	krad_compositor_subunit->rotation_time = 0;
	krad_compositor_subunit->opacity_time = 0;
	krad_compositor_subunit->xscale_time = 0;
	krad_compositor_subunit->yscale_time = 0;
	
	krad_compositor_subunit->rotation_duration = 0;
	krad_compositor_subunit->opacity_duration = 0;
	krad_compositor_subunit->xscale_duration = 0;
	krad_compositor_subunit->yscale_duration = 0;	
	
	krad_compositor_subunit->krad_ease_x = krad_ease_random();
	krad_compositor_subunit->krad_ease_y = krad_ease_random();
	krad_compositor_subunit->krad_ease_xscale = krad_ease_random();
	krad_compositor_subunit->krad_ease_yscale = krad_ease_random();
	krad_compositor_subunit->krad_ease_rotation = krad_ease_random();
	krad_compositor_subunit->krad_ease_opacity = krad_ease_random();
	
}

void krad_compositor_subunit_destroy (krad_compositor_subunit_t *krad_compositor_subunit) {
	
	krad_compositor_subunit_reset (krad_compositor_subunit);
	free (krad_compositor_subunit);

}
