#include "krad_wayland.h"

struct kr_wayland_window_st {
  int width;
  int height;
  int pointer_x;
  int pointer_y;
  int click;
  int mousein;
  struct wl_surface *surface;
  struct wl_shell_surface *shell_surface;
  struct wl_buffer *buffer;
  void *shm_data;
  struct wl_callback *callback;
  struct wl_shell_surface_listener surface_listener;
  struct wl_callback_listener frame_listener;
  int current_buffer;
  int frame_size;
  struct wl_buffer *buffers[KR_WL_BUFFER_COUNT];
  int (*user_callback)(void *, kr_wayland_event *);
  void *user;
  kr_wayland *wayland;
};

struct kr_wayland_st {
  int display_fd;
  kr_wayland_window *window[KR_WL_MAX_WINDOWS];
  struct wl_display *display;
  struct wl_registry *registry;
  struct wl_compositor *compositor;
  struct wl_shell *shell;
  struct wl_shm *shm;
  uint32_t formats;
  uint32_t mask;
  struct wl_shm_listener shm_listener;
  struct wl_seat *seat;
  struct wl_pointer *pointer;
  struct wl_keyboard *keyboard;
  struct wl_seat_listener seat_listener;
  struct wl_pointer_listener pointer_listener;
  struct wl_keyboard_listener keyboard_listener;
  struct wl_registry_listener registry_listener;
  struct {
    struct xkb_context *context;
    struct xkb_keymap *keymap;
    struct xkb_state *state;
    xkb_mod_mask_t control_mask;
    xkb_mod_mask_t alt_mask;
    xkb_mod_mask_t shift_mask;
  } xkb;
};

static int kr_wayland_window_create_shm_buffer (kr_wayland_window *window,
 int width, int height, int frames, uint32_t format, void **data_out);
static void kr_wayland_handle_ping (void *data,
 struct wl_shell_surface *shell_surface, uint32_t serial);
static void kr_wayland_handle_configure (void *data,
 struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width,
 int32_t height);
static void kr_wayland_handle_popup_done (void *data,
 struct wl_shell_surface *shell_surface);
static void pointer_handle_enter(void *data,
 struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface,
 wl_fixed_t sx_w, wl_fixed_t sy_w);
static void pointer_handle_leave(void *data,
 struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface);
static void pointer_handle_motion(void *data,
 struct wl_pointer *pointer, uint32_t time, wl_fixed_t sx_w, wl_fixed_t sy_w);
static void pointer_handle_button(void *data,
 struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button,
 uint32_t state_w);
static void pointer_handle_axis(void *data,
 struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value);
static void kr_wayland_seat_handle_capabilities (void *data,
 struct wl_seat *seat, enum wl_seat_capability caps);
static void kr_wayland_shm_format (void *data, struct wl_shm *wl_shm,
 uint32_t format);
static void kr_wayland_handle_global (void *data, struct wl_registry *registry,
 uint32_t id, const char *interface, uint32_t version);
static void kr_wayland_frame_listener (void *data,
 struct wl_callback *callback, uint32_t time);

static void kr_wayland_handle_configure(void *data,
 struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width,
 int32_t height) {
  /* Nothing here */
}

static void kr_wayland_handle_popup_done(void *data,
 struct wl_shell_surface *shell_surface) {
  /* Nothing here */
}

static void pointer_handle_axis(void *data, struct wl_pointer *pointer,
 uint32_t time, uint32_t axis, wl_fixed_t value) {
  /* Nothing here */
}

static void kr_wayland_handle_ping(void *data,
 struct wl_shell_surface *shell_surface, uint32_t serial) {
  wl_shell_surface_pong(shell_surface, serial);
}

static void keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
 uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched,
 uint32_t mods_locked, uint32_t group) {
  /* Nothing here */
}

static void keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
 uint32_t format, int fd, uint32_t size) {

  kr_wayland *wayland = data;
  char *map_str;

  if (!data) {
    close(fd);
    return;
  }

  if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
    close(fd);
    return;
  }

  map_str = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
  if (map_str == MAP_FAILED) {
    close(fd);
    return;
  }

  wayland->xkb.keymap = xkb_map_new_from_string(wayland->xkb.context,
   map_str, XKB_KEYMAP_FORMAT_TEXT_V1, 0);

  munmap(map_str, size);
  close(fd);

  if (!wayland->xkb.keymap) {
    fprintf(stderr, "failed to compile keymap\n");
    return;
  }

  wayland->xkb.state = xkb_state_new(wayland->xkb.keymap);
  if (!wayland->xkb.state) {
    fprintf(stderr, "failed to create XKB state\n");
    xkb_map_unref(wayland->xkb.keymap);
    wayland->xkb.keymap = NULL;
    return;
  }

  wayland->xkb.control_mask =
    1 << xkb_map_mod_get_index(wayland->xkb.keymap, "Control");
  wayland->xkb.alt_mask =
    1 << xkb_map_mod_get_index(wayland->xkb.keymap, "Mod1");
  wayland->xkb.shift_mask =
    1 << xkb_map_mod_get_index(wayland->xkb.keymap, "Shift");
}

static void keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
 uint32_t serial, struct wl_surface *surface, struct wl_array *keys) {
  /* Nothing here */
}

static void keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
 uint32_t serial, struct wl_surface *surface) {
  /* Nothing here */
}

static void keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
 uint32_t serial, uint32_t time, uint32_t key, uint32_t state_w) {

  kr_wayland *wayland = data;
  //struct window *window = input->keyboard_focus;
  uint32_t code, num_syms;
  enum wl_keyboard_key_state state = state_w;
  const xkb_keysym_t *syms;
  xkb_keysym_t sym;
  //struct itimerspec its;

  //input->display->serial = serial;
  code = key + 8;
  if (!wayland->xkb.state)
    return;

  num_syms = xkb_key_get_syms(wayland->xkb.state, code, &syms);

  sym = XKB_KEY_NoSymbol;
  if (num_syms == 1) {
    sym = syms[0];
  }

/*
  if (sym == XKB_KEY_F5 && input->modifiers == MOD_ALT_MASK) {
    if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
      window_set_maximized(window,
               window->type != TYPE_MAXIMIZED);
  } else if (sym == XKB_KEY_F11 &&
       window->fullscreen_handler &&
       state == WL_KEYBOARD_KEY_STATE_PRESSED) {
    window->fullscreen_handler(window, window->user_data);
  } else if (sym == XKB_KEY_F4 &&
       input->modifiers == MOD_ALT_MASK &&
       state == WL_KEYBOARD_KEY_STATE_PRESSED) {
    if (window->close_handler)
      window->close_handler(window->parent,
                window->user_data);
    else
      display_exit(window->display);
  } else if (window->key_handler) {
    (*window->key_handler)(window, input, time, key,
               sym, state, window->user_data);
  }

  if (state == WL_KEYBOARD_KEY_STATE_RELEASED &&
      key == input->repeat_key) {
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 0;
    timerfd_settime(input->repeat_timer_fd, 0, &its, NULL);
  } else if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
    input->repeat_sym = sym;
    input->repeat_key = key;
    input->repeat_time = time;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 25 * 1000 * 1000;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 400 * 1000 * 1000;
    timerfd_settime(input->repeat_timer_fd, 0, &its, NULL);
  }
*/
  if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
    if (sym == XKB_KEY_q) {
      printf("you hit the q key!\n");
    } else {
      if (sym == XKB_KEY_f) {
        printf("you hit the f key!\n");
      } else {
        //printf("you hit some other key!\n");
      }
    }
  }
}

static void pointer_handle_enter(void *data, struct wl_pointer *pointer,
 uint32_t serial, struct wl_surface *surface, wl_fixed_t x, wl_fixed_t y) {

  kr_wayland_window *window = data;

  window->pointer_x = wl_fixed_to_int(x);
  window->pointer_y = wl_fixed_to_int(y);

  window->mousein = 1;

  wl_pointer_set_cursor(pointer, serial, NULL, 0, 0);

  /*
  if (!surface) {
    // enter event for a window we've just destroyed 
    return;
  }

  input->display->serial = serial;
  input->pointer_enter_serial = serial;
  input->pointer_focus = wl_surface_get_user_data(surface);
  window = input->pointer_focus;

  if (window->pool) {
    shm_pool_destroy(window->pool);
    window->pool = NULL;
    //Schedule a redraw to free the pool 
    window_schedule_redraw(window);
  }

  input->sx = sx;
  input->sy = sy;

  widget = widget_find_widget(window->widget, sx, sy);
  input_set_focus_widget(input, widget, sx, sy);
  */
}

static void pointer_handle_leave(void *data, struct wl_pointer *pointer,
 uint32_t serial, struct wl_surface *surface) {

  kr_wayland_window *window = data;

  window->pointer_x = -1;
  window->pointer_y = -1;
  window->mousein = 0;
}

static void pointer_handle_motion(void *data, struct wl_pointer *pointer,
 uint32_t time, wl_fixed_t x, wl_fixed_t y) {

  kr_wayland_window *window = data;

  window->pointer_x = wl_fixed_to_int(x);
  window->pointer_y = wl_fixed_to_int(y);

  /*
  input->sx = sx;
  input->sy = sy;

  if (!(input->grab && input->grab_button)) {
    widget = widget_find_widget(window->widget, sx, sy);
    input_set_focus_widget(input, widget, sx, sy);
  }

  if (input->grab)
    widget = input->grab;
  else
    widget = input->focus_widget;
  if (widget && widget->motion_handler)
    cursor = widget->motion_handler(input->focus_widget,
            input, time, sx, sy,
            widget->user_data);

  input_set_pointer_image(input, cursor);
  */
}

static void pointer_handle_button(void *data, struct wl_pointer *pointer,
 uint32_t serial, uint32_t time, uint32_t button, uint32_t state_w) {

  kr_wayland_window *window = data;
  
  enum wl_pointer_button_state state = state_w;

  if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
    window->click = 1;
  }
  
  if (state == WL_POINTER_BUTTON_STATE_RELEASED) {
    window->click = 0;    
  }
}

static void kr_wayland_seat_handle_capabilities(void *data,
 struct wl_seat *seat, enum wl_seat_capability caps) {

  kr_wayland *wayland = data;

  if ((caps & WL_SEAT_CAPABILITY_POINTER) && !wayland->pointer) {
    wayland->pointer = wl_seat_get_pointer(seat);
    /*wl_pointer_set_user_data (wayland->pointer, wayland);*/
    wl_pointer_add_listener (wayland->pointer, &wayland->pointer_listener,
     wayland);
  } else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && wayland->pointer) {
    wl_pointer_destroy(wayland->pointer);
    wayland->pointer = NULL;
  }

  if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !wayland->keyboard) {
    wayland->keyboard = wl_seat_get_keyboard(seat);
    /*wl_keyboard_set_user_data(wayland->keyboard, wayland);*/
    wl_keyboard_add_listener(wayland->keyboard, &wayland->keyboard_listener,
     wayland);
  } else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && wayland->keyboard) {
    wl_keyboard_destroy(wayland->keyboard);
    wayland->keyboard = NULL;
  }
}

static void kr_wayland_shm_format(void *data, struct wl_shm *wl_shm,
 uint32_t format) {
  kr_wayland *wayland = data;
  wayland->formats |= (1 << format);
}

static void kr_wayland_handle_global(void *data, struct wl_registry *registry,
 uint32_t id, const char *interface, uint32_t version) {

  kr_wayland *wayland = data;

  if (strcmp(interface, "wl_compositor") == 0) {
    wayland->compositor =
      wl_registry_bind (wayland->registry, id,
       &wl_compositor_interface, 1);
  } else if (strcmp(interface, "wl_shell") == 0) {
    wayland->shell = wl_registry_bind(wayland->registry, id,
     &wl_shell_interface, 1);
  } else if (strcmp(interface, "wl_seat") == 0) {
    wayland->seat = wl_registry_bind(wayland->registry, id,
     &wl_seat_interface, 1);
    wl_seat_add_listener(wayland->seat,
     &wayland->seat_listener, wayland);
  } else if (strcmp(interface, "wl_shm") == 0) {
    wayland->shm = wl_registry_bind(wayland->registry, id,
     &wl_shm_interface, 1);
    wl_shm_add_listener(wayland->shm, &wayland->shm_listener,
     wayland);
  }
}

static int kr_wayland_window_create_shm_buffer(kr_wayland_window *window,
 int width, int height, int frames, uint32_t format, void **data_out) {

  char filename[] = "/tmp/wayland-shm-XXXXXX";
  struct wl_shm_pool *pool;
  int fd;
  int size;
  int stride;
  void *data;
  int b;
  
  b = 0;

  fd = mkstemp(filename);
  if (fd < 0) {
    fprintf(stderr, "open %s failed: %m\n", filename);
    return -1;
  }
  stride = width * 4;
  window->frame_size = stride * height;
  size = window->frame_size * frames;
  if (ftruncate(fd, size) < 0) {
    fprintf (stderr, "ftruncate failed: %m\n");
    close(fd);
    return -1;
  }

  data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  unlink(filename);

  if (data == MAP_FAILED) {
    fprintf(stderr, "mmap failed: %m\n");
    close(fd);
    return -1;
  }

  pool = wl_shm_create_pool(window->wayland->shm, fd, size);
  for (b = 0; b < KR_WL_BUFFER_COUNT; b++) {
    window->buffers[b] = wl_shm_pool_create_buffer(pool,
     b * window->frame_size, width, height, stride, format);
  }
  wl_shm_pool_destroy(pool);
  close(fd);
  *data_out = data;
  return 0;
}

static void kr_wayland_frame_listener (void *data,
 struct wl_callback *callback, uint32_t time) {

  kr_wayland_window *window;
  kr_wayland_event wayland_event;
  int updated;

  window = data;
  updated = 0;
  memset(&wayland_event, 0, sizeof(kr_wayland_event));

  if (window->user_callback != NULL) {
    wayland_event.type = KR_WL_FRAME;
    wayland_event.buffer = (uint8_t *)window->shm_data;
    updated = window->user_callback(window->user, &wayland_event);
  }

  wl_surface_attach(window->surface, window->buffer, 0, 0);

  if (updated) {
    wl_surface_damage(window->surface, 0, 0, window->width, window->height);
  } else {
    wl_surface_damage(window->surface, 0, 0, 10, 10);
  }
  
  if (callback) {
    wl_callback_destroy(callback);
  }

  window->callback = wl_surface_frame(window->surface);
  window->frame_listener.done = kr_wayland_frame_listener;
  wl_callback_add_listener(window->callback, &window->frame_listener,
   window);
  wl_surface_commit(window->surface);
}

kr_wayland_window *kr_wayland_window_create(kr_wayland *wayland,
 kr_wayland_window_params *params) {

  kr_wayland_window *window;
  struct wl_region *opaque;
  int ret;
  int i;

  if ((wayland == NULL) || (params == NULL)) {
    return NULL;
  }

  if ((params->width == 0) || (params->height == 0) ||
      (params->width > 8192) || (params->height > 8192)
      || params->callback == NULL) {
    return NULL;
  }

  for (i = 0; i < KR_WL_MAX_WINDOWS; i++) {
    if (wayland->window[i] == NULL) {
      break;
    }
  }
  if (i == KR_WL_MAX_WINDOWS) {
    return NULL;
  }

  window = calloc(1, sizeof(kr_wayland_window));
  wayland->window[i] = window;

  window->wayland = wayland;
  window->user_callback = params->callback;
  window->user = params->user;
  window->width = params->width;
  window->height = params->height;

  ret = kr_wayland_window_create_shm_buffer(window, window->width,
   window->height, KR_WL_BUFFER_COUNT, WL_SHM_FORMAT_XRGB8888,
   &window->shm_data);

  if (ret != 0) {
    free(wayland->window[i]);
    wayland->window[i] = NULL;
    return NULL;
  }

  window->current_buffer = 0;
  window->buffer = window->buffers[window->current_buffer];

  window->surface_listener.ping = kr_wayland_handle_ping;
  window->surface_listener.configure = kr_wayland_handle_configure;
  window->surface_listener.popup_done = kr_wayland_handle_popup_done;
  window->surface = wl_compositor_create_surface(wayland->compositor);
  window->shell_surface = wl_shell_get_shell_surface(wayland->shell, 
   window->surface);

  opaque = wl_compositor_create_region(wayland->compositor);
  wl_region_add(opaque, 0, 0, window->width, window->height);
  wl_surface_set_opaque_region(window->surface, opaque);
  wl_region_destroy(opaque);

  /*wl_shell_surface_set_title(wayland->window->shell_surface,
   wayland->window->title);*/

  if (window->shell_surface) {
    wl_shell_surface_add_listener(window->shell_surface, 
     &window->surface_listener, window);
  }
  wl_shell_surface_set_toplevel(window->shell_surface);

  kr_wayland_frame_listener(window, NULL, 0);
  wl_display_roundtrip(wayland->display);

  return window;
}

int kr_wayland_window_destroy(kr_wayland_window **win) {

  int i;
  kr_wayland_window *window;
  kr_wayland *wayland;

  if ((win == NULL) || (*win == NULL)) {
    return -1;
  }

  window = *win;
  wayland = window->wayland;

  for (i = 0; i < KR_WL_MAX_WINDOWS; i++) {
    if (wayland->window[i] == window) {
      wayland->window[i] = NULL;
      break;
    }
  }
  wl_display_sync(wayland->display);
  if (window->callback) {
    wl_callback_destroy(window->callback);
  }
  wl_display_sync(wayland->display);
  wl_buffer_destroy(window->buffer);
  wl_shell_surface_destroy(window->shell_surface);
  wl_surface_destroy(window->surface);
  wl_display_sync(wayland->display);
  free(*win);
  wayland->window[i] = NULL;
  *win = NULL;    
  return 0;
}

int kr_wayland_get_fd(kr_wayland *wayland) {
  if (wayland == NULL) {
    return -1;
  }
  return wayland->display_fd;
}

int kr_wayland_process(kr_wayland *wayland) {
  /* TODO Check for disconnect etc */
  wl_display_dispatch(wayland->display);
  wl_display_roundtrip(wayland->display);
  return 0;
}

int kr_wayland_destroy(kr_wayland **wl) {

  int i;
  kr_wayland *wayland;

  if ((wl == NULL) || (*wl == NULL)) {
    return -1;
  }

  wayland = *wl;

  for (i = 0; i < KR_WL_MAX_WINDOWS; i++) {
    if (wayland->window[i] != NULL) {
      //kr_wayland_window_destroy(&wayland->window[i]);
    }
  }

  if (wayland->xkb.state) {
    xkb_state_unref(wayland->xkb.state);
  }
  if (wayland->xkb.keymap) {
    xkb_map_unref(wayland->xkb.keymap);
  }
  if (wayland->xkb.context) {
    xkb_context_unref(wayland->xkb.context);
  }
  if (wayland->shm) {
    wl_shm_destroy (wayland->shm);
  }
  if (wayland->shell) {
    wl_shell_destroy (wayland->shell);
  }
  if (wayland->compositor) {
    wl_compositor_destroy (wayland->compositor);
  }
  if (wayland->seat) {
    wl_seat_destroy (wayland->seat);
  }
  wl_registry_destroy (wayland->registry);
  wl_display_flush (wayland->display);
  wl_display_disconnect (wayland->display);
  free(wayland);
  *wl = NULL;
  return 0;
}

kr_wayland *kr_wayland_create() {
  return kr_wayland_create_for_server(NULL);
}

kr_wayland *kr_wayland_create_for_server(char *server) {
  kr_wayland *wayland = calloc(1, sizeof(kr_wayland));
  wayland->display = wl_display_connect(server);
  if (wayland->display == NULL) {
    free(wayland);
    return NULL;
  }

  wayland->xkb.context = xkb_context_new(0);

  wayland->pointer_listener.enter = pointer_handle_enter;
  wayland->pointer_listener.leave = pointer_handle_leave;
  wayland->pointer_listener.motion = pointer_handle_motion;
  wayland->pointer_listener.button = pointer_handle_button;
  wayland->pointer_listener.axis = pointer_handle_axis;

  wayland->keyboard_listener.keymap = keyboard_handle_keymap;
  wayland->keyboard_listener.enter = keyboard_handle_enter;
  wayland->keyboard_listener.leave = keyboard_handle_leave;
  wayland->keyboard_listener.key = keyboard_handle_key;
  wayland->keyboard_listener.modifiers = keyboard_handle_modifiers;

  wayland->seat_listener.capabilities = kr_wayland_seat_handle_capabilities;
  wayland->shm_listener.format = kr_wayland_shm_format;
  wayland->formats = 0;
  wayland->registry_listener.global = kr_wayland_handle_global;
  wayland->registry = wl_display_get_registry (wayland->display);
  wl_registry_add_listener(wayland->registry, &wayland->registry_listener,
   wayland);
  wl_display_roundtrip(wayland->display);
  wayland->display_fd = wl_display_get_fd(wayland->display);
  return wayland;
}
