#include "krad_wayland.h"
#include "krad_vector.h"
#include "krad_easing.h"

#define TEST_WINDOWS 1
#define NUM_VECTORS 4

typedef struct kr_wayland_test kr_wayland_test;
typedef struct kr_wayland_test_window kr_wayland_test_window;

struct kr_wayland_test_window {
  kr_wayland_path *window;
  kr_wayland_path_info info;
  void *buffer;
  char name[32];
  kr_vector *vector;
  int pointer_in;
  int pointer_x;
  int pointer_y;
  int rotin;
  int delayor;
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
  cairo_surface_t *cst;
  cairo_t *cr;
  float rot;

  wayland_test_window = (kr_wayland_test_window *)user;

  updated = 0;

  if (wayland_test_window == NULL) {
    /* Its bad */
    return -1;
  }

  cst = cairo_image_surface_create_for_data ((unsigned char *)event->frame_event.buffer,
                         CAIRO_FORMAT_ARGB32,
                         wayland_test_window->info.width,
                         wayland_test_window->info.height,
                         wayland_test_window->info.width * 4);

  cr = cairo_create (cst);

  cairo_save (cr);
  cairo_set_source_rgba (cr, BGCOLOR_CLR);
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (cr);
  cairo_restore (cr);


  if (((wayland_test_window->vector[2].subunit.x - wayland_test_window->vector[0].subunit.x) < 200) == 0) {

    uint32_t *p;
    int i;
    int end;
    int offset;
    int time;

    time = rand();
    p = (uint32_t *)event->frame_event.buffer;
    end = wayland_test_window->info.width * wayland_test_window->info.height;
    offset = time >> 4;
    for (i = 0; i < end; i++) {
      p[i] = (i + offset) * 0x0080401;

      //if ((wayland_test_window->vector[0].subunit.x % 3) == 0) {
      //  offset = time >> 5;
      //}

    }

  } else {

    cairo_save (cr);
    cairo_set_source_rgba (cr, BGCOLOR_CLR);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint (cr);
    cairo_restore (cr);


  }

  if ((wayland_test_window->pointer_in == 1) && (wayland_test_window->rotin)) {

    if (wayland_test_window->delayor > 0) {
      krad_compositor_subunit_set_xy(&wayland_test_window->vector[3].subunit,
       wayland_test_window->pointer_x + 6, wayland_test_window->pointer_y + 7, wayland_test_window->delayor);
      wayland_test_window->delayor = 0;
    }

    kr_vector_render(&wayland_test_window->vector[3], cr);

  }

  if (wayland_test_window->pointer_in == 1) {
    krad_compositor_subunit_set_xy(&wayland_test_window->vector[0].subunit,
     wayland_test_window->pointer_x, wayland_test_window->pointer_y, 0);
    if (wayland_test_window->rotin) {
      rot = wayland_test_window->vector[2].subunit.rotation += 7.0;
      krad_compositor_subunit_set_rotation(&wayland_test_window->vector[2].subunit,
       rot, 0);
      cairo_save(cr);
      cairo_translate(cr, wayland_test_window->vector[2].subunit.x, wayland_test_window->vector[0].subunit.y);
      cairo_translate(cr, wayland_test_window->pointer_x / 2, wayland_test_window->pointer_y / 2);
      cairo_rotate(cr, rot - 9.0 * (M_PI/180.0));
      cairo_translate(cr, wayland_test_window->pointer_x / -2, wayland_test_window->pointer_y / -2);
      cairo_translate(cr, wayland_test_window->vector[0].subunit.x * -1, wayland_test_window->vector[2].subunit.x * -1);
    }
    kr_vector_render(&wayland_test_window->vector[0], cr);
    kr_vector_render(&wayland_test_window->vector[2], cr);
    if ((!wayland_test_window->rotin) && (wayland_test_window->delayor > 0)) {
      krad_compositor_subunit_set_xy(&wayland_test_window->vector[2].subunit,
       wayland_test_window->pointer_x + 6, wayland_test_window->pointer_y + 7, wayland_test_window->delayor);
      wayland_test_window->delayor = 0;
    }
    if (wayland_test_window->rotin) {
     wayland_test_window->delayor = rand() % 240;
      wayland_test_window->delayor--;
      cairo_restore(cr);
    }
  }

  kr_vector_render(&wayland_test_window->vector[1], cr);

  cairo_surface_flush (cst);
  cairo_destroy (cr);
  cairo_surface_destroy (cst);

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
  //  printf("pointer in\n");
    wayland_test_window->pointer_in = 1;
  }

  if (event->pointer_event.pointer_out) {
  //  printf("pointer out\n");
    wayland_test_window->pointer_in = 0;
    return 0;
  }

 // printf("pointer event: %s: %dx%d - click: %d\n", wayland_test_window->name,
 //  event->pointer_event.x, event->pointer_event.y, event->pointer_event.click);

  wayland_test_window->pointer_x = event->pointer_event.x;
  wayland_test_window->pointer_y = event->pointer_event.y;

  if (event->pointer_event.click) {
    krad_compositor_subunit_set_xy(&wayland_test_window->vector[1].subunit,
     wayland_test_window->pointer_x, wayland_test_window->pointer_y, 20);
  }

  return 0;
}

int kr_wl_test_key_cb(void *user, kr_wayland_event *event) {

  kr_wayland_test_window *wayland_test_window;
  float rot;

  if (user == NULL) {
    return -1;
  }

  wayland_test_window = (kr_wayland_test_window *)user;

 // printf("key event: %s: %d - %d %c\n", wayland_test_window->name,
 //  event->key_event.down, event->key_event.key, event->key_event.key);

  if (event->key_event.key == 'q') {
    destroy = 1;
  }
  if (event->key_event.key == 'c') {
    kr_vector_type_set(&wayland_test_window->vector[0], "circle");
  }
  if (event->key_event.key == 'x') {
    kr_vector_type_set(&wayland_test_window->vector[0], "hex");
  }
  if (event->key_event.key == 't') {
    kr_vector_type_set(&wayland_test_window->vector[0], "triangle");
  }
  if (event->key_event.key == 'a') {
    rot = wayland_test_window->vector[0].subunit.rotation -= 6.0;
    krad_compositor_subunit_set_rotation(&wayland_test_window->vector[0].subunit, rot, 6);
  }
  if (event->key_event.key == 'd') {
    rot = wayland_test_window->vector[0].subunit.rotation += 46.0;
    krad_compositor_subunit_set_rotation(&wayland_test_window->vector[0].subunit, rot, 56);
  }

  if ((event->key_event.key == 'r') && (event->key_event.down)) {
    wayland_test_window->rotin = 1;
  }
  if ((event->key_event.key == 'r') && (!event->key_event.down)) {
    wayland_test_window->rotin = 0;
  }

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

  while (!destroy) {
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

  for (i = 0; i < TEST_WINDOWS; i++) {
    printf("Destroying window %d\n", i);
    ret = kr_wayland_unlink(&wayland_test->windows[i].window);
    if (ret < 0) {
      fprintf(stderr, "Could not destroy window %dx%d\n",
       wayland_test->windows[i].info.width, wayland_test->windows[i].info.height);
      exit(1);
    }
    kr_vectors_free(wayland_test->windows[i].vector, NUM_VECTORS);
  }

  kr_wayland_destroy(&wayland_test->wayland);
  free(wayland_test);
}

kr_wayland_test *wayland_test_create(int width, int height) {

  int i;
  kr_wayland_test *wayland_test;
  kr_wayland_path_setup window_params;

  wayland_test = calloc(1, sizeof(kr_wayland_test));

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

    wayland_test->windows[i].vector = kr_vectors_create(NUM_VECTORS);
    kr_vector_type_set(&wayland_test->windows[i].vector[0], "hex");
    kr_vector_type_set(&wayland_test->windows[i].vector[1], "viper");
    kr_vector_type_set(&wayland_test->windows[i].vector[2], "triangle");
    kr_vector_type_set(&wayland_test->windows[i].vector[3], "hex");
    krad_compositor_subunit_set_red(&wayland_test->windows[i].vector[0].subunit, 0.2, 799);
    krad_compositor_subunit_set_blue(&wayland_test->windows[i].vector[0].subunit, 0.5, 555);

    krad_compositor_subunit_set_green(&wayland_test->windows[i].vector[2].subunit, 0.2, 199);
    krad_compositor_subunit_set_blue(&wayland_test->windows[i].vector[2].subunit, 0.2, 255);

    krad_compositor_subunit_set_green(&wayland_test->windows[i].vector[3].subunit, 0.4, 199);
    krad_compositor_subunit_set_red(&wayland_test->windows[i].vector[3].subunit, 0.2, 255);

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

void wayland_test_run(int width, int height) {

  kr_wayland_test *wayland_test;

  wayland_test = wayland_test_create(width, height);
  wayland_test_loop(wayland_test);
  wayland_test_destroy(wayland_test);
}

int main (int argc, char *argv[]) {

  int width;
  int height;

  width = 1280;
  height = 720;

  if (argc == 3) {
    wayland_test_run(atoi(argv[1]), atoi(argv[2]));
  } else {
    wayland_test_run(width, height);
  }

  return 0;
}
