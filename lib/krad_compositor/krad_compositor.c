#include "krad_compositor.h"
#include "krad_compositor_path.c"

static void tick(kr_compositor *compositor);
static void setup(kr_compositor *compositor);
static void composite(kr_compositor *compositor);
static void output(kr_compositor *compositor);
static void cleanup(kr_compositor *compositor);

static void paths_free(kr_compositor *compositor);
static void paths_create(kr_compositor *compositor);
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

static void composite(kr_compositor *compositor) {
  int i;
  kr_compositor_path *path;
  i = 0;
  kr_compositor_clear_frame(compositor);
  if ((compositor->active_input_paths == 0)
   && (compositor->active_sprites == 0)
   && (compositor->active_texts == 0)
   && (compositor->active_vectors == 0)) {
    kr_compositor_render_no_input(compositor);
  }
  while ((path = kr_pool_iterate_active(compositor->path_pool, &i))) {
    if (path->info.type == KR_CMP_INPUT) {
      kr_image image;
      memset(&image, 0, sizeof(kr_image));
      image.px = (uint8_t *)compositor->frame->pixels;
      image.ppx[0] = image.px;
      image.w = compositor->frame->width;
      image.h = compositor->frame->height;
      image.pps[0] = compositor->frame->width * 4;
      path_render(path, &image, compositor->cr);
    }
  }
/*
  for (i = 0; i < KC_MAX_SPRITES; i++) {
    if (compositor->sprite[i].subunit.active == 1) {
      kr_sprite_render(&compositor->sprite[i], compositor->cr);
    }
  }
  for (i = 0; i < KC_MAX_VECTORS; i++) {
    if (compositor->vector[i].subunit.active == 1) {
      kr_vector_render(&compositor->vector[i], compositor->cr);
    }
  }
  for (i = 0; i < KC_MAX_TEXTS; i++) {
    if (compositor->text[i].subunit.active == 1) {
      kr_text_render(&compositor->text[i], compositor->cr);
    }
  }
*/
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
      kr_sprite_reset(&compositor->sprite[i]);
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

void krad_compositor_subunit_update(kr_compositor *compositor, kr_unit_control_t *uc) {

  if ((compositor == NULL) || (uc == NULL) || (uc->address.id.number >= KC_MAX_ANY)) {
    return;
  }
/*
  if (uc->address.path.unit == KR_COMPOSITOR) {
    switch (uc->address.path.subunit.compositor_subunit) {
      case KR_SPRITE:
        if (uc->address.id.number < KC_MAX_SPRITES) {
          if (compositor->sprite[uc->address.id.number].subunit.active == 1) {
            switch (uc->address.control.compositor_control) {
              case KR_NO:
                break;
              case KR_X:
                krad_compositor_subunit_set_x(&compositor->sprite[uc->address.id.number].subunit,
                                              uc->value.integer, uc->duration);
                break;
              case KR_Y:
                krad_compositor_subunit_set_y(&compositor->sprite[uc->address.id.number].subunit,
                                              uc->value.integer, uc->duration);
                break;
              case KR_Z:
                krad_compositor_subunit_set_z(&compositor->sprite[uc->address.id.number].subunit,
                                              uc->value.integer);
                break;
              case KR_WIDTH:
                krad_compositor_subunit_set_width(&compositor->sprite[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);
                break;
              case KR_HEIGHT:
                krad_compositor_subunit_set_height(&compositor->sprite[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);
                break;
              case KR_ROTATION:
                krad_compositor_subunit_set_rotation(&compositor->sprite[uc->address.id.number].subunit,
                                                     uc->value.real, uc->duration);
                break;
              case KR_OPACITY:
                krad_compositor_subunit_set_opacity(&compositor->sprite[uc->address.id.number].subunit,
                                                    uc->value.real, uc->duration);
                break;
              case KR_RED:
                krad_compositor_subunit_set_red(&compositor->sprite[uc->address.id.number].subunit,
                                                uc->value.real, uc->duration);
                break;
              case KR_GREEN:
                krad_compositor_subunit_set_green(&compositor->sprite[uc->address.id.number].subunit,
                                                  uc->value.real, uc->duration);
                break;
              case KR_BLUE:
                krad_compositor_subunit_set_blue(&compositor->sprite[uc->address.id.number].subunit,
                                                 uc->value.real, uc->duration);
                break;
              case KR_ALPHA:
                krad_compositor_subunit_set_alpha(&compositor->sprite[uc->address.id.number].subunit,
                                                  uc->value.real, uc->duration);
                break;
              case KR_TICKRATE:
                kr_sprite_set_tickrate(&compositor->sprite[uc->address.id.number],
                                         uc->value.integer);
                break;
            }
          }
        }
        return;
      case KR_TEXT:
        if (uc->address.id.number < KC_MAX_TEXTS) {
          if (compositor->text[uc->address.id.number].subunit.active == 1) {
            switch (uc->address.control.compositor_control) {
              case KR_NO:
              case KR_TICKRATE:
                break;
              case KR_X:
                krad_compositor_subunit_set_x (&compositor->text[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);
                break;
              case KR_Y:
                krad_compositor_subunit_set_y (&compositor->text[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);
                break;
              case KR_Z:
                krad_compositor_subunit_set_z (&compositor->text[uc->address.id.number].subunit,
                                               uc->value.integer);
                break;
              case KR_WIDTH:
                krad_compositor_subunit_set_width (&compositor->text[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);
                break;
              case KR_HEIGHT:
                krad_compositor_subunit_set_height (&compositor->text[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);
                break;
              case KR_ROTATION:
                krad_compositor_subunit_set_rotation (&compositor->text[uc->address.id.number].subunit,
                                                      uc->value.real, uc->duration);
                break;
              case KR_OPACITY:
                krad_compositor_subunit_set_opacity (&compositor->text[uc->address.id.number].subunit,
                                                     uc->value.real, uc->duration);
                break;
              case KR_RED:
                krad_compositor_subunit_set_red (&compositor->text[uc->address.id.number].subunit,
                                                 uc->value.real, uc->duration);
                break;
              case KR_GREEN:
                krad_compositor_subunit_set_green (&compositor->text[uc->address.id.number].subunit,
                                                   uc->value.real, uc->duration);
                break;
              case KR_BLUE:
                krad_compositor_subunit_set_blue (&compositor->text[uc->address.id.number].subunit,
                                                  uc->value.real, uc->duration);
                break;
              case KR_ALPHA:
                krad_compositor_subunit_set_alpha (&compositor->text[uc->address.id.number].subunit,
                                                   uc->value.real, uc->duration);
                break;
            }
          }
        }
        return;
      case KR_VECTOR:
        if (uc->address.id.number < KC_MAX_VECTORS) {
          if (compositor->vector[uc->address.id.number].subunit.active == 1) {
            switch (uc->address.control.compositor_control) {
              case KR_NO:
              case KR_TICKRATE:
                break;
              case KR_X:
                krad_compositor_subunit_set_x (&compositor->vector[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);
                break;
              case KR_Y:
                krad_compositor_subunit_set_y (&compositor->vector[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);
                break;
              case KR_Z:
                krad_compositor_subunit_set_z (&compositor->vector[uc->address.id.number].subunit,
                                               uc->value.integer);
                break;
              case KR_WIDTH:
                krad_compositor_subunit_set_width (&compositor->vector[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);
                break;
              case KR_HEIGHT:
                krad_compositor_subunit_set_height (&compositor->vector[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);
                break;
              case KR_ROTATION:
                krad_compositor_subunit_set_rotation (&compositor->vector[uc->address.id.number].subunit,
                                                      uc->value.real, uc->duration);
                break;
              case KR_OPACITY:
                krad_compositor_subunit_set_opacity (&compositor->vector[uc->address.id.number].subunit,
                                                     uc->value.real, uc->duration);
                break;
              case KR_RED:
                krad_compositor_subunit_set_red (&compositor->vector[uc->address.id.number].subunit,
                                                 uc->value.real, uc->duration);
                break;
              case KR_GREEN:
                krad_compositor_subunit_set_green (&compositor->vector[uc->address.id.number].subunit,
                                                   uc->value.real, uc->duration);
                break;
              case KR_BLUE:
                krad_compositor_subunit_set_blue (&compositor->vector[uc->address.id.number].subunit,
                                                  uc->value.real, uc->duration);
                break;
              case KR_ALPHA:
                krad_compositor_subunit_set_alpha (&compositor->vector[uc->address.id.number].subunit,
                                                   uc->value.real, uc->duration);
                break;
            }
          }
        }
        return;
      case KR_VIDEOPORT:


      kr_compositor_path *path;
  int i;

  i = 0;

  while ((path = kr_pool_iterate_active(compositor->path_pool, &i))) {
    if (path->info.type == KR_CMP_INPUT) {
      path_output(path, compositor->frame);
    }
  }

        if (uc->address.id.number < KC_MAX_PORTS) {
          if ((compositor->path[uc->address.id.number].subunit.active == 1) &&
              (compositor->path[uc->address.id.number].type == KR_CMP_INPUT)) {
            switch (uc->address.control.compositor_control) {
              case KR_NO:
              case KR_TICKRATE:
                break;
              case KR_X:
                krad_compositor_subunit_set_x (&compositor->path[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);
                break;
              case KR_Y:
                krad_compositor_subunit_set_y (&compositor->path[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);
                break;
              case KR_Z:
                krad_compositor_subunit_set_z (&compositor->path[uc->address.id.number].subunit,
                                               uc->value.integer);
                break;
              case KR_WIDTH:

                //if (uc->value.integer < compositor->path[uc->address.id.number].crop_width) {
                  kr_easer_set(&compositor->path[uc->address.id.number].crop_width_easer,
                        uc->value.integer, uc->duration, EASEINOUTSINE, NULL);
                //}

                krad_compositor_subunit_set_width (&compositor->path[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);
                break;
              case KR_HEIGHT:

                //if (uc->value.integer < compositor->path[uc->address.id.number].crop_height) {
                  kr_easer_set(&compositor->path[uc->address.id.number].crop_height_easer,
                         uc->value.integer, uc->duration, EASEINOUTSINE, NULL);
                //}

                krad_compositor_subunit_set_height (&compositor->path[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);
                break;
              case KR_ROTATION:
                krad_compositor_subunit_set_rotation (&compositor->path[uc->address.id.number].subunit,
                                                      uc->value.real, uc->duration);
                break;
              case KR_OPACITY:
                krad_compositor_subunit_set_opacity (&compositor->path[uc->address.id.number].subunit,
                                                     uc->value.real, uc->duration);
                break;
              case KR_RED:
                krad_compositor_subunit_set_red (&compositor->path[uc->address.id.number].subunit,
                                                 uc->value.real, uc->duration);
                break;
              case KR_GREEN:
                krad_compositor_subunit_set_green (&compositor->path[uc->address.id.number].subunit,
                                                   uc->value.real, uc->duration);
                break;
              case KR_BLUE:
                krad_compositor_subunit_set_blue (&compositor->path[uc->address.id.number].subunit,
                                                  uc->value.real, uc->duration);
                break;
              case KR_ALPHA:
                krad_compositor_subunit_set_alpha (&compositor->path[uc->address.id.number].subunit,
                                                   uc->value.real, uc->duration);
                break;
            }
          }
        }
        return;
      default:
        return;
    }
  }
*/
}

int kr_compositor_path_ctl(kr_compositor_path *p, kr_compositor_path_setting *s) {

  return 0;
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

static void paths_free(kr_compositor *compositor) {
  kr_pool_destroy(compositor->path_pool);
}

static void paths_create(kr_compositor *compositor) {
  kr_pool_setup setup;
  setup.shared = 0;
  setup.size = sizeof(kr_compositor_path);
  setup.slices = KC_MAX_PORTS;
  compositor->path_pool = kr_pool_create(&setup);
}

static void subunits_free(kr_compositor *compositor) {
  paths_free(compositor);
  kr_sprite_destroy_arr(compositor->sprite, KC_MAX_SPRITES);
  kr_vectors_free(compositor->vector, KC_MAX_VECTORS);
  kr_text_destroy_arr(compositor->text, KC_MAX_TEXTS);
  FT_Done_FreeType(compositor->ftlib);
}

static void subunits_create(kr_compositor *compositor) {
  paths_create(compositor);
  FT_Init_FreeType(&compositor->ftlib);
  compositor->text = kr_text_create_arr(&compositor->ftlib, KC_MAX_TEXTS);
  compositor->sprite = kr_sprite_create_arr(KC_MAX_SPRITES);
  compositor->vector = kr_vectors_create(KC_MAX_VECTORS);
}

int kr_compositor_destroy(kr_compositor *compositor) {

  if (compositor == NULL) return -1;

  printk("Krad Compositor: Destroy Started");

  subunits_free(compositor);
  krad_framepool_destroy(&compositor->framepool);
  free(compositor);

  printk("Krad Compositor: Destroy Complete");

  return 0;
}

kr_compositor *kr_compositor_create(kr_compositor_setup *setup) {

  kr_compositor *com;

  if (setup == NULL) return NULL;

  com = calloc(1, sizeof(kr_compositor));
  resolution_set(com, setup->width, setup->height);
  frame_rate_set(com, setup->fps_num, setup->fps_den);
  subunits_create(com);
  com->framepool = krad_framepool_create(com->info.width, com->info.height,
   DEFAULT_COMPOSITOR_BUFFER_FRAMES);
  return com;
}

int kr_compositor_get_info(kr_compositor *com, kr_compositor_info *info) {
  if ((com == NULL) || (info == NULL)) return -1;
  *info = com->info;
  info->texts = com->active_texts;
  info->vectors = com->active_vectors;
  info->sprites = com->active_sprites;
  info->inputs = com->active_input_paths;
  info->outputs = com->active_output_paths;
  return 0;
}

void kr_compositor_setup_init(kr_compositor_setup *setup) {
  if (setup == NULL) return;
  setup->fps_num = KR_COMPOSITOR_FPS_NUM_DEF;
  setup->fps_den = KR_COMPOSITOR_FPS_DEN_DEF;
  setup->width = KR_COMPOSITOR_WIDTH_DEF;
  setup->height = KR_COMPOSITOR_HEIGHT_DEF;
}
