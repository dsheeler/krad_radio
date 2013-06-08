#include "krad_wayland.h"

typedef struct kr_wayland_test kr_wayland_test;

struct kr_wayland_test {
  kr_wayland *wayland;
  int width;
  int height;
  void *buffer;
  int done;
};

int kr_wl_test_frame_cb(void *pointer, uint32_t time) {

  kr_wayland_test *wayland_test;
  int updated;
  uint32_t *p;
  int i, end, offset;

  wayland_test = (kr_wayland_test *)pointer;

  updated = 0;
  
  if (wayland_test == NULL) {
    /* Its bad */
  }  

  p = wayland_test->buffer;
  end = wayland_test->width * wayland_test->height;
  offset = time >> 4;
  for (i = 0; i < end; i++) {
    p[i] = (i + offset) * 0x0080401;
  }

  updated = 1;
  //printf("frame callback time is %u\r", time);
  //fflush(stdout);

  return updated;
}

void wayland_test_loop(kr_wayland_test *wayland_test) {
  
  int count;

  count = 0;

  while (!wayland_test->done) {
    kr_wayland_process(wayland_test->wayland);
    //if ((wayland_test->wayland->mousein) && (wayland_test->wayland->click)) {

    //}
  }
  count++;
  printf("bye bye now!\n");
}

void wayland_test_destroy (kr_wayland_test *wayland_test) {
  kr_wayland_close_window(wayland_test->wayland);
  kr_wayland_destroy(wayland_test->wayland);
  free (wayland_test);
}

kr_wayland_test *wayland_test_create() {

  kr_wayland_test *wayland_test;
  
  wayland_test = calloc (1, sizeof(kr_wayland_test));
  
  wayland_test->width = 1280;
  wayland_test->height = 720;

  wayland_test->wayland = kr_wayland_create();

  kr_wayland_window_create(wayland_test->wayland, wayland_test->width,
   wayland_test->height, &wayland_test->buffer);

  kr_wayland_set_frame_callback(wayland_test->wayland, kr_wl_test_frame_cb,
   wayland_test);

  kr_wayland_open_window(wayland_test->wayland);
  return wayland_test;
}

void wayland_test_run() {

  kr_wayland_test *wayland_test;

  wayland_test = wayland_test_create();
  wayland_test_loop(wayland_test);
  wayland_test_destroy(wayland_test);
}

int main (int argc, char *argv[]) {

  wayland_test_run();

  return 0;
}
