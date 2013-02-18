#include "krad_wayland.h"

int krad_wayland_test_frame (void *pointer, uint32_t time) {

	int updated;

	updated = 0;

	printf ("frame callback time is %u\r", time);
	fflush (stdout);

	return updated;
}

int main (int argc, char *argv[]) {

	krad_wayland_t *krad_wayland;

	int width;
	int height;

	void *buffer;

	width = 960;
	height = 540;

	if (argc == 3) {
		width = atoi(argv[1]);
		height = atoi(argv[2]);
	}

	krad_wayland = krad_wayland_create ();

	krad_wayland->render_test_pattern = 1;

	krad_wayland_prepare_window (krad_wayland, width, height, &buffer);

  //krad_wayland_set_frame_callback (krad_compositor_wayland_display->krad_wayland,
  //                 krad_compositor_wayland_display_render_callback,
  //                 krad_compositor_wayland_display);

  krad_wayland_prepare_window (krad_wayland,
                 width,
                 height,
                 &buffer);

  printk("Wayland display prepared");

  krad_wayland_open_window (krad_wayland);

  printk("Wayland display running");

  while (1) {
    krad_wayland_iterate (krad_wayland);
  }

  krad_wayland_close_window (krad_wayland);

	krad_wayland_destroy (krad_wayland);


	return 0;

}
