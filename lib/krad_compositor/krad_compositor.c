#include "krad_compositor.h"
#include "krad_compositor_path.c"

static void tick(kr_compositor *compositor);
static void setup(kr_compositor *compositor);
static void composite(kr_compositor *compositor);
static void output(kr_compositor *compositor);
static void cleanup(kr_compositor *compositor);

static void subunits_free(kr_compositor *compositor);
static void subunits_create(kr_compositor *compositor);
static void subunits_state_update(kr_compositor *compositor);

static void kr_compositor_render_no_input(kr_compositor *compositor) {
  int f;
  f = compositor->info.frames;
  cairo_save(compositor->cr);
  if ((compositor->info.frames % 24) < 12) {
    cairo_set_source_rgba(compositor->cr, RED, 0.0f + ((f % 12) * 0.09f));
  } else {
    cairo_set_source_rgba(compositor->cr, RED, 1.0f + ((f % 12) * -0.09f));
  }
  cairo_select_font_face(compositor->cr, "", CAIRO_FONT_SLANT_NORMAL,
   CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(compositor->cr, 42.0);
  cairo_move_to(compositor->cr, 64, 64 + 42);
  cairo_show_text(compositor->cr, "KR: No Input!");
  cairo_stroke(compositor->cr);
  cairo_restore(compositor->cr);
}

static void kr_compositor_clear_frame(kr_compositor *compositor) {
  cairo_save(compositor->cr);
  cairo_set_source_rgba(compositor->cr, BGCOLOR_CLR);
  cairo_set_operator(compositor->cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(compositor->cr);
  cairo_restore(compositor->cr);
}

static void tick(kr_compositor *com) {
  com->info.timecode = round(1000000000 * com->info.frames
   / com->info.fps_numerator * com->info.fps_denominator / 1000000);
  com->info.frames++;
}

static void setup(kr_compositor *compositor) {
  tick(compositor);
  while (compositor->frame == NULL) {
    compositor->frame = krad_framepool_getframe(compositor->framepool);
    if (compositor->frame == NULL) {
      printke("Compositor wanted a frame but could not get one right away!");
      usleep(1000);
    }
  }
  compositor->cr = cairo_create(compositor->frame->cst);
}

static void composite(kr_compositor *com) {
  int i;
  kr_compositor_path *path;
  void *overlay;
  i = 0;
  kr_compositor_clear_frame(com);
  if ((com->info.inputs == 0)
   && (com->info.sprites == 0)
   && (com->info.texts == 0)
   && (com->info.vectors == 0)) {
    kr_compositor_render_no_input(com);
  }
  while ((path = kr_pool_iterate_active(com->path_pool, &i))) {
    if (path_type_get(path) == KR_CMP_INPUT) {
      kr_image image;
      memset(&image, 0, sizeof(kr_image));
      image.px = (uint8_t *)com->frame->pixels;
      image.ppx[0] = image.px;
      image.w = com->frame->width;
      image.h = com->frame->height;
      image.pps[0] = com->frame->width * 4;
      image.fmt = PIX_FMT_RGB32;
      path_render(path, &image, com->cr);
    }
  }
  while ((overlay = kr_pool_iterate_active(com->sprite_pool, &i))) {
    kr_sprite_render(overlay, com->cr);
  }
  while ((overlay = kr_pool_iterate_active(com->text_pool, &i))) {
    kr_text_render(overlay, com->cr);
  }
  while ((overlay = kr_pool_iterate_active(com->vector_pool, &i))) {
    kr_vector_render(overlay, com->cr);
  }
}

static void output(kr_compositor *compositor) {

  kr_compositor_path *path;
  int i;

  i = 0;

  while ((path = kr_pool_iterate_active(compositor->path_pool, &i))) {
    if (path->info.type == KR_CMP_OUTPUT) {
      path_output(path, compositor->frame);
    }
  }
}

static void cleanup(kr_compositor *compositor) {
  krad_framepool_unref_frame(compositor->frame);
  compositor->frame = NULL;
  cairo_destroy(compositor->cr);
  subunits_state_update(compositor);
}

int kr_compositor_process(kr_compositor *compositor) {
  if (compositor == NULL) return -1;
  setup(compositor);
  composite(compositor);
  output(compositor);
  cleanup(compositor);
  return 0;
}

static void subunits_state_update(kr_compositor *compositor) {
  /*
  int i;
  for (i = 0; i < KC_MAX_PORTS; i++) {
    if (compositor->path[i].subunit.active == 2) {
        cmper_path_release(compositor, &compositor->path[i]);
        krad_compositor_subunit_reset(&compositor->path[i].subunit);
        compositor->path[i].subunit.active = 0;
    }
  }
  for (i = 0; i < KC_MAX_SPRITES; i++) {
    if (compositor->sprite[i].subunit.active == 2) {
      kr_sprite_clear(&compositor->sprite[i]);
      compositor->active_sprites--;
      compositor->sprite[i].subunit.active = 0;
    }
  }
  for (i = 0; i < KC_MAX_TEXTS; i++) {
    if (compositor->text[i].subunit.active == 2) {
      kr_text_reset(&compositor->text[i]);
      compositor->active_texts--;
      compositor->text[i].subunit.active = 0;
    }
  }
  for (i = 0; i < KC_MAX_VECTORS; i++) {
    if (compositor->vector[i].subunit.active == 2) {
      kr_vector_reset(&compositor->vector[i]);
      compositor->active_vectors--;
      compositor->vector[i].subunit.active = 0;
    }
  }
  */
}

int kr_compositor_ctl(kr_compositor *com, kr_compositor_setting *setting) {

  return 0;
}

static void resolution_set(kr_compositor *com, uint32_t w, uint32_t h) {
  com->info.width = w;
  com->info.height = h;
}

static void frame_rate_set(kr_compositor *cmpr, int num, int den) {
  cmpr->info.fps_numerator = num;
  cmpr->info.fps_denominator = den;
}

static void subunits_free(kr_compositor *com) {
  int i;
  i = 0;
  void *overlay;

  while ((overlay = kr_pool_iterate_active(com->sprite_pool, &i))) {
    kr_sprite_clear(overlay);
    kr_pool_recycle(com->sprite_pool, overlay);
  }
  while ((overlay = kr_pool_iterate_active(com->text_pool, &i))) {
  }
  while ((overlay = kr_pool_iterate_active(com->vector_pool, &i))) {
  }

  kr_pool_destroy(com->path_pool);
  kr_pool_destroy(com->sprite_pool);
  kr_pool_destroy(com->text_pool);
  kr_pool_destroy(com->vector_pool);
}

static void subunits_create(kr_compositor *compositor) {
  kr_pool_setup setup;
  setup.shared = 0;

  setup.size = kr_compositor_path_size();
  setup.slices = KC_MAX_PORTS;
  compositor->path_pool = kr_pool_create(&setup);

  setup.size = kr_sprite_size();
  setup.slices = KC_MAX_SPRITES;
  compositor->sprite_pool = kr_pool_create(&setup);

  setup.size = kr_text_size();
  setup.slices = KC_MAX_TEXTS;
  compositor->text_pool = kr_pool_create(&setup);

  setup.size = kr_vector_size();
  setup.slices = KC_MAX_VECTORS;
  compositor->vector_pool = kr_pool_create(&setup);

  /*
  TEMP
  char *filename;
  void *overlay;
  int ret;
  filename = "/home/oneman/images/krad_color_logo_trans2.png";
  overlay = kr_pool_slice(compositor->sprite_pool);
  if (overlay != NULL) {
    ret = kr_sprite_open(overlay, filename);
    if (ret) {
      failfast("sprite open %s did not work", filename);
    }
  }

  char *string;
  char *font;
  string = "Weee!";
  font = "/home/oneman/.local/share/fonts/Inconsolata.otf";
  //font = NULL;
  overlay = kr_pool_slice(compositor->text_pool);
  if (overlay != NULL) {
    ret = kr_text_init(overlay, string, font, &compositor->ftlib);
    if (ret) {
      failfast("text init %s did not work", string);
    }
  }

  char *vector_string;
  vector_string = "viper";
  overlay = kr_pool_slice(compositor->vector_pool);
  if (overlay != NULL) {
    ret = kr_vector_init(overlay, vector_string);
    if (ret) {
      failfast("text vector %s did not work", vector_string);
    }
  }
  */
}

int kr_compositor_destroy(kr_compositor *com) {
  if (com == NULL) return -1;
  printk("Krad Compositor: Destroy Started");
  subunits_free(com);
  FT_Done_FreeType(com->ftlib);
  krad_framepool_destroy(&com->framepool);
  free(com);
  printk("Krad Compositor: Destroy Complete");
  return 0;
}

kr_compositor *kr_compositor_create(kr_compositor_setup *setup) {
  kr_compositor *com;
  if (setup == NULL) return NULL;
  com = calloc(1, sizeof(kr_compositor));
  resolution_set(com, setup->width, setup->height);
  frame_rate_set(com, setup->fps_num, setup->fps_den);
  FT_Init_FreeType(&com->ftlib);
  subunits_create(com);
  com->framepool = krad_framepool_create(com->info.width, com->info.height,
   DEFAULT_COMPOSITOR_BUFFER_FRAMES);
  return com;
}

int kr_compositor_info_get(kr_compositor *com, kr_compositor_info *info) {
  if ((com == NULL) || (info == NULL)) return -1;
  *info = com->info;
  return 0;
}

void kr_compositor_setup_init(kr_compositor_setup *setup) {
  if (setup == NULL) return;
  setup->fps_num = KR_COMPOSITOR_FPS_NUM_DEF;
  setup->fps_den = KR_COMPOSITOR_FPS_DEN_DEF;
  setup->width = KR_COMPOSITOR_WIDTH_DEF;
  setup->height = KR_COMPOSITOR_HEIGHT_DEF;
}
