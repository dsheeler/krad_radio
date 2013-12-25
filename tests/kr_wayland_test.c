#include "krad_wayland.h"

#define TEST_WINDOWS 2

typedef struct kr_wayland_test kr_wayland_test;
typedef struct kr_wayland_test_window kr_wayland_test_window;

struct kr_wayland_test_window {
  kr_wayland_path *window;
  kr_wayland_path_info info;
  void *buffer;
  char name[32];
};

struct kr_wayland_test {
  kr_wayland *wayland;
  kr_wayland_test_window windows[TEST_WINDOWS];
};

static int destroy = 0;

void signal_recv (int sig) {
  destroy = 1;
}

int kr_wl_test_frame_cb(void *user, kr_wayland_event *event) {

  kr_wayland_test_window *wayland_test_window;
  int updated;
  uint32_t *p;
  int i;
  int end;
  int offset;
  int time;

  wayland_test_window = (kr_wayland_test_window *)user;

  updated = 0;

  if (wayland_test_window == NULL) {
    /* Its bad */
    return -1;
  }

  time = rand();
  p = (uint32_t *)event->frame_event.buffer;
  end = wayland_test_window->info.width * wayland_test_window->info.height;
  offset = time >> 4;
  for (i = 0; i < end; i++) {
    p[i] = (i + offset) * 0x0080401;
  }

  updated = 1;
  return updated;
}

int kr_wl_test_pointer_cb(void *user, kr_wayland_event *event) {

  kr_wayland_test_window *wayland_test_window;

  if (user == NULL) {
    return -1;
  }

  wayland_test_window = (kr_wayland_test_window *)user;

  if (event->pointer_event.pointer_in) {
    printf("pointer in\n");
  }

  if (event->pointer_event.pointer_out) {
    printf("pointer out\n");
    return 0;
  }

  printf("pointer event: %s: %dx%d - click: %d\n", wayland_test_window->name,
   event->pointer_event.x, event->pointer_event.y, event->pointer_event.click);


  return 0;
}

int kr_wl_test_key_cb(void *user, kr_wayland_event *event) {

  kr_wayland_test_window *wayland_test_window;

  if (user == NULL) {
    return -1;
  }

  wayland_test_window = (kr_wayland_test_window *)user;

  printf("key event: %s: %d - %d %c\n", wayland_test_window->name,
   event->key_event.down, event->key_event.key, event->key_event.key);
  return 0;
}

int kr_wl_test_cb(void *user, kr_wayland_event *event) {
  switch (event->type) {
    case KR_WL_FRAME:
      return kr_wl_test_frame_cb(user, event);
    case KR_WL_POINTER:
      return kr_wl_test_pointer_cb(user, event);
    case KR_WL_KEY:
      return kr_wl_test_key_cb(user, event);
  }
  return 0;
}

void wayland_test_loop(kr_wayland_test *wayland_test) {

  int count;
  int ret;
  count = 0;

  while ((!destroy) && (count < 2180)) {
    ret = kr_wayland_process(wayland_test->wayland);
    if (ret < 0) {
      break;
    }
    count++;
  }
}

void wayland_test_destroy (kr_wayland_test *wayland_test) {

  int i;
  int ret;

  for (i = 0; i < TEST_WINDOWS/2; i++) {
    printf("Destroying window %d\n", i);
    ret = kr_wayland_unlink(&wayland_test->windows[i].window);
    if (ret < 0) {
      fprintf(stderr, "Could not destroy window %dx%d\n",
       wayland_test->windows[i].info.width, wayland_test->windows[i].info.height);
      exit(1);
    }
  }

  kr_wayland_destroy(&wayland_test->wayland);
  free(wayland_test);
}

kr_wayland_test *wayland_test_create() {

  int i;
  int width;
  int height;
  kr_wayland_test *wayland_test;
  kr_wayland_path_setup window_params;

  wayland_test = calloc(1, sizeof(kr_wayland_test));

  width = 160;
  height = 120;

  wayland_test->wayland = kr_wayland_create(NULL);

  if (wayland_test->wayland == NULL) {
    fprintf(stderr, "Could not connect to wayland\n");
    exit(1);
  }

  signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);

  for (i = 0; i < TEST_WINDOWS; i++) {
    wayland_test->windows[i].info.width = width;
    wayland_test->windows[i].info.height = height;
    window_params.info = wayland_test->windows[i].info;
    window_params.user = &wayland_test->windows[i];
    window_params.callback = kr_wl_test_cb;
    wayland_test->windows[i].window =
     kr_wayland_mkpath(wayland_test->wayland, &window_params);
    if (wayland_test->windows[i].window == NULL) {
      fprintf(stderr, "Could not create window %dx%d\n", width, height);
      exit(1);
    }
    snprintf(wayland_test->windows[i].name,
     sizeof(wayland_test->windows[i].name), "Window Num %d", i);
    width = width + width;
    height = height + height;
  }

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
