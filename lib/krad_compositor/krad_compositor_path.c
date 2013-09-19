static void kr_compositor_path_tick(kr_compositor_path *path);

static void kr_compositor_path_tick(kr_compositor_path *path) {
  krad_compositor_subunit_tick(&path->subunit);
  if (path->crop_x_easer.active) {
    path->info.crop_x = kr_easer_process(&path->crop_x_easer,
     path->info.crop_x, NULL);
  }
  if (path->crop_y_easer.active) {
    path->info.crop_y = kr_easer_process(&path->crop_y_easer,
     path->info.crop_y, NULL);
  }
  if (path->crop_width_easer.active) {
    path->info.crop_width = kr_easer_process(&path->crop_width_easer,
     path->info.crop_width, NULL);
  }
  if (path->crop_height_easer.active) {
    path->info.crop_height = kr_easer_process(&path->crop_height_easer,
     path->info.crop_height, NULL);
  }
}

void kr_compositor_path_render(kr_compositor_path *path, cairo_t *cr) {
/*
  krad_frame_t *frame;

  frame = krad_compositor_path_pull_frame(path);
  if (frame != NULL) {
    if (path->subunit.opacity > 0.0f) {
      cairo_save (cr);
      if (path->local == 1) {
        if ((path->subunit.width != path->source_width) ||
            (path->subunit.height != path->source_height)) {
          path->subunit.xscale = (float)path->subunit.width / path->source_width;
          path->subunit.yscale = (float)path->subunit.height / path->source_height;
        } else {
          path->subunit.xscale = 1.0f;
          path->subunit.yscale = 1.0f;
        }
        if ((path->subunit.xscale != 1.0f) || (path->subunit.yscale != 1.0f)) {
          cairo_translate(cr, path->subunit.x, path->subunit.y);
          cairo_translate(cr, ((path->subunit.width / 2) * path->subunit.xscale),
           ((path->subunit.height / 2) * path->subunit.yscale));
          cairo_scale(cr, path->subunit.xscale, path->subunit.yscale);
          cairo_translate(cr, path->subunit.width / -2, path->subunit.height / -2);
          cairo_translate(cr, path->subunit.x * -1, path->subunit.y * -1);
        }
      }
      if (path->subunit.rotation != 0.0f) {
        cairo_translate(cr, path->subunit.x, path->subunit.y);
        cairo_translate(cr, frame->width / 2, frame->height / 2);
        cairo_rotate(cr, path->subunit.rotation * (M_PI/180.0));
        cairo_translate(cr, frame->width / -2, frame->height / -2);
        cairo_translate(cr, -path->subunit.x, -path->subunit.y);
      }
      cairo_set_source_surface(cr, frame->cst, path->subunit.x - path->crop_x,
       path->subunit.y - path->crop_y);
      cairo_rectangle(cr, path->subunit.x, path->subunit.y, frame->width,
       frame->height);
      cairo_clip(cr);
      if (path->subunit.opacity == 1.0f) {
        cairo_paint(cr);
      } else {
        cairo_paint_with_alpha(cr, path->subunit.opacity);
      }
      cairo_restore(cr);
    }
    if (path->local != 1) {
      krad_framepool_unref_frame(frame);
    }
    frame = NULL;
  }
  */
}

int path_setup_check(kr_compositor_path_setup *setup) {

  kr_compositor_path_info *info;
  info = &setup->info;

  if ((setup->user == NULL) || (setup->frame_cb == NULL)) {
    /* FIXME HRMMM */
  }

  if ((info->width == 0) || (info->height == 0)) {
    return -1;
  }
  if ((info->type != KR_CMP_OUTPUT) && (info->type != KR_CMP_OUTPUT)) {
    return -2;
  }
  /* FIXME check more things out */
  return 0;
}

static void path_create(kr_compositor_path *path,
 kr_compositor_path_setup *setup) {

  path->info = setup->info;
  path->user = setup->user;
  path->frame_cb = setup->frame_cb;
  krad_compositor_subunit_reset(&path->subunit);
}

kr_compositor_path *kr_compositor_mkpath(kr_compositor *compositor,
 kr_compositor_path_setup *setup) {

  kr_compositor_path *path;

  if ((compositor == NULL) || (setup == NULL)) return NULL;
  if (path_setup_check(setup)) {
    printke("compositor mkpath failed setup check");
    return NULL;
  }
  /*
  path = kr_mixer_find(mixer, setup->info.name);
  if (path != NULL) {
    printke("mixer mkpath path with that name already exists");
    return NULL;
  }
  */
  path = kr_pool_slice(compositor->path_pool);
  if (path == NULL) {
    printke("compositor mkpath could not slice new path");
    return NULL;
  }
  path->compositor = compositor;
  path_create(path, setup);
  return path;
}

void cmper_path_release(kr_compositor *compositor, kr_compositor_path *path) {
  if (path->perspective != NULL) {
    kr_perspective_destroy(&path->perspective);
  }
  if (path->converter != NULL) {
    sws_freeContext(path->converter);
    path->converter = NULL;
  }
  if (path->info.type == KR_CMP_INPUT) {
    compositor->active_input_paths--;
  }
  if (path->info.type == KR_CMP_OUTPUT) {
    compositor->active_output_paths--;
  }
  kr_pool_recycle(path->compositor->path_pool, path);
  compositor->active_paths--;
}

int kr_compositor_unlink(kr_compositor_path *path) {
  if (path == NULL) return -1;
  if (path->subunit.active != 1) return -2;
  path->subunit.active = 2;
  return 0;
}
