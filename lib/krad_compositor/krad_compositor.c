#include "krad_compositor.h"

#include "background.c"
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

static void tick(kr_compositor *compositor) {
  compositor->timecode = round(1000000000 * compositor->frames
   / compositor->fps_numerator * compositor->fps_denominator / 1000000);
  compositor->frames++;
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
  if (krad_compositor_has_background(compositor)) {
    krad_compositor_render_background(compositor);
  } else {
    krad_compositor_clear_frame(compositor);
    if ((compositor->active_input_paths == 0) &&
        (compositor->active_sprites == 0) &&
        (compositor->active_texts == 0) &&
        (compositor->active_vectors == 0) &&
        (compositor->background->subunit.active == 0)) {
        krad_compositor_render_no_input(compositor);
    }
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
  int i;
  /*
  for (i = 0; i < KC_MAX_PORTS; i++) {
    if (compositor->path[i].subunit.active == 2) {
        cmper_path_release(compositor, &compositor->path[i]);
        krad_compositor_subunit_reset(&compositor->path[i].subunit);
        compositor->path[i].subunit.active = 0;
    }
  }
  */
  for (i = 0; i < KC_MAX_SPRITES; i++) {
    if (compositor->sprite[i].subunit.active == 2) {
      krad_sprite_reset(&compositor->sprite[i]);
      compositor->active_sprites--;
      compositor->sprite[i].subunit.active = 0;
    }
  }
  for (i = 0; i < KC_MAX_TEXTS; i++) {
    if (compositor->text[i].subunit.active == 2) {
      krad_text_reset(&compositor->text[i]);
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
}

void krad_compositor_subunit_update(kr_compositor *compositor, kr_unit_control_t *uc) {

  if ((compositor == NULL) || (uc == NULL) || (uc->address.id.number >= KC_MAX_ANY)) {
    return;
  }

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
                krad_sprite_set_tickrate(&compositor->sprite[uc->address.id.number],
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
        /*
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
      */
      default:
        return;
    }
  }
}

int krad_compositor_subunit_create(kr_compositor *compositor,
 kr_compositor_subunit_t type, char *option, char *option2) {

  int i;

  i = 0;

  printk("Krad Compositor: Subunit create type: %s and option: %s",
   kr_compositor_subunit_type_to_string(type), option);

  switch (type) {
    case KR_SPRITE:
      for (i = 0; i < KC_MAX_SPRITES; i++) {
        if (compositor->sprite[i].subunit.active == 0) {
          if (krad_sprite_open_file(&compositor->sprite[i], option)) {
            compositor->sprite[i].subunit.active = 1;
            compositor->active_sprites++;
            return i;
          }
          break;
        }
      }
      return -1;
    case KR_TEXT:
      for (i = 0; i < KC_MAX_TEXTS; i++) {
        if (compositor->text[i].subunit.active == 0) {
          krad_text_set_text(&compositor->text[i], option, option2);
          compositor->text[i].subunit.active = 1;
          compositor->active_texts++;
          return i;
        }
      }
      return -1;
    case KR_VECTOR:
      for (i = 0; i < KC_MAX_VECTORS; i++) {
        if (compositor->vector[i].subunit.active == 0) {
          if (krad_string_to_vector_type(option) != NOTHING) {
            kr_vector_type_set(&compositor->vector[i], option);
            compositor->vector[i].subunit.active = 1;
            compositor->active_vectors++;
            return i;
          }
          break;
        }
      }
      return -1;
    case KR_VIDEOPORT:
      return -1;
  }

  return -1;
}

int krad_compositor_subunit_destroy(kr_compositor *compositor, kr_address_t *address) {

  if ((compositor == NULL) || (address == NULL) || (address->id.number >= KC_MAX_ANY)) {
    return 0;
  }

  if (address->path.unit == KR_COMPOSITOR) {
    switch (address->path.subunit.compositor_subunit) {
      case KR_SPRITE:
        if (address->id.number < KC_MAX_SPRITES) {
          if (compositor->sprite[address->id.number].subunit.active == 1) {
            compositor->sprite[address->id.number].subunit.active = 2;
            return 1;
          }
        }
        return 0;
      case KR_TEXT:
        if (address->id.number < KC_MAX_TEXTS) {
          if (compositor->text[address->id.number].subunit.active == 1) {
            compositor->text[address->id.number].subunit.active = 2;
            return 1;
          }
        }
        return 0;
      case KR_VECTOR:
        if (address->id.number < KC_MAX_VECTORS) {
          if (compositor->vector[address->id.number].subunit.active == 1) {
            compositor->vector[address->id.number].subunit.active = 2;
            return 1;
          }
        }
        return 0;
      case KR_VIDEOPORT:
        /*
        if (address->id.number < KC_MAX_PORTS) {
          if (compositor->path[address->id.number].subunit.active == 1) {
            compositor->path[address->id.number].subunit.active = 2;
            return 1;
          }
        }
        return 0;
        */
      default:
        return 0;
    }
  }
  return 0;
}

void kr_compositor_resolution_set(kr_compositor *comp, uint32_t width,
 uint32_t height) {
  comp->width = width;
  comp->height = height;
  comp->frame_byte_size = comp->width * comp->height * 4;
}

void krad_compositor_set_frame_rate(kr_compositor *cmpr, int num, int den) {
  cmpr->fps_numerator = num;
  cmpr->fps_denominator = den;
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
  krad_sprite_destroy_arr(compositor->sprite, KC_MAX_SPRITES);
  kr_vectors_free(compositor->vector, KC_MAX_VECTORS);
  krad_text_destroy_arr(compositor->text, KC_MAX_TEXTS);
  FT_Done_FreeType(compositor->ftlib);
}

static void subunits_init(kr_compositor *compositor) {

  int i;

  for (i = 0; i < KC_MAX_SUBUNITS; i++) {
    if (i < KC_MAX_PORTS) {
      /* compositor->subunit[i] = &compositor->path[i].subunit; */
    } else {
      if (i < (KC_MAX_SPRITES + KC_MAX_PORTS)) {
        compositor->subunit[i] = &compositor->sprite[i - KC_MAX_PORTS].subunit;
      } else {
        if (i < (KC_MAX_TEXTS + KC_MAX_SPRITES + KC_MAX_PORTS)) {
          compositor->subunit[i] = &compositor->text[i - (KC_MAX_PORTS + KC_MAX_SPRITES)].subunit;
        } else {
          if (i < (KC_MAX_VECTORS + KC_MAX_TEXTS + KC_MAX_SPRITES + KC_MAX_PORTS)) {
            compositor->subunit[i] = &compositor->vector[i - (KC_MAX_PORTS + KC_MAX_SPRITES + KC_MAX_TEXTS)].subunit;
          }
        }
      }
    }
  }
}

static void subunits_create(kr_compositor *compositor) {
  paths_create(compositor);
  FT_Init_FreeType(&compositor->ftlib);
  compositor->text = krad_text_create_arr(&compositor->ftlib, KC_MAX_TEXTS);
  compositor->sprite = krad_sprite_create_arr(KC_MAX_SPRITES);
  compositor->vector = kr_vectors_create(KC_MAX_VECTORS);
  subunits_init(compositor);
}

int kr_compositor_destroy(kr_compositor *compositor) {

  if (compositor == NULL) return -1;

  printk("Krad Compositor: Destroy Started");

  subunits_free(compositor);
  krad_framepool_destroy(&compositor->framepool);
  krad_sprite_destroy(compositor->background);
  free(compositor);

  printk("Krad Compositor: Destroy Complete");

  return 0;
}

kr_compositor *kr_compositor_create(kr_compositor_setup *setup) {

  kr_compositor *compositor;

  if (setup == NULL) return NULL;

  compositor = calloc(1, sizeof(kr_compositor));
  kr_compositor_resolution_set(compositor, setup->width, setup->height);
  krad_compositor_set_frame_rate(compositor, setup->fps_num, setup->fps_den);
  subunits_create(compositor);
  compositor->background = krad_sprite_create();
  compositor->framepool = krad_framepool_create(compositor->width,
   compositor->height, DEFAULT_COMPOSITOR_BUFFER_FRAMES);

  return compositor;
}

void kr_compositor_setup_init(kr_compositor_setup *setup) {
  if (setup == NULL) return;
  setup->fps_num = KR_COMPOSITOR_FPS_NUM_DEF;
  setup->fps_den = KR_COMPOSITOR_FPS_DEN_DEF;
  setup->width = KR_COMPOSITOR_WIDTH_DEF;
  setup->height = KR_COMPOSITOR_HEIGHT_DEF;
}
