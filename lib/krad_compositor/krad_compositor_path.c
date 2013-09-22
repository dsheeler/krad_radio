static void path_tick(kr_compositor_path *path);

static void path_tick(kr_compositor_path *path) {
  krad_compositor_subunit_tick(&path->subunit);
  if (kr_easer_active(&path->crop_x_easer)) {
    path->info.crop_x = kr_easer_process(&path->crop_x_easer,
     path->info.crop_x, NULL);
  }
  if (kr_easer_active(&path->crop_y_easer)) {
    path->info.crop_y = kr_easer_process(&path->crop_y_easer,
     path->info.crop_y, NULL);
  }
  if (kr_easer_active(&path->crop_width_easer)) {
    path->info.crop_width = kr_easer_process(&path->crop_width_easer,
     path->info.crop_width, NULL);
  }
  if (kr_easer_active(&path->crop_height_easer)) {
    path->info.crop_height = kr_easer_process(&path->crop_height_easer,
     path->info.crop_height, NULL);
  }
}

void path_output(kr_compositor_path *path, krad_frame_t *frame) {
  kr_compositor_path_frame_cb_arg cb_arg;
  cb_arg.user = path->user;
  path->frame_cb(&cb_arg);
  memcpy(cb_arg.image.px, frame->pixels, frame->width * frame->height * 4);
}

void kr_compositor_path_render(kr_compositor_path *path, cairo_t *cr) {
  path_tick(path);
//  if (path->subunit.opacity > 0.0f) {
    cairo_save(cr);
/*     cairo_set_source_surface(cr, frame->cst, path->subunit.x - path->crop_x,
     path->subunit.y - path->crop_y);
*/
    cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    cairo_rectangle(cr, path->subunit.x, path->subunit.y, path->info.width,
     path->info.height);
//    cairo_clip(cr);
    cairo_fill(cr);
    printk("eek!");
    /*
    if (path->subunit.opacity == 1.0f) {
      cairo_paint(cr);
    } else {
      cairo_paint_with_alpha(cr, path->subunit.opacity);
    }
    */
    cairo_restore(cr);
 // }
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
  if ((info->type != KR_CMP_INPUT) && (info->type != KR_CMP_OUTPUT)) {
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
  if (path->info.type == KR_CMP_INPUT) {
    path->compositor->active_input_paths++;
  }
  if (path->info.type == KR_CMP_OUTPUT) {
    path->compositor->active_output_paths++;
  }
  path->compositor->active_paths++;
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
