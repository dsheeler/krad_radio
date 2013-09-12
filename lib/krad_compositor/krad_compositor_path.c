static void kr_compositor_path_tick(kr_compositor_path *path);

static void kr_compositor_path_tick(kr_compositor_path *path) {
  krad_compositor_subunit_tick(&path->subunit);
  if (path->crop_x_easer.active) {
    path->crop_x = kr_easer_process(&path->crop_x_easer,
     path->crop_x, NULL);
  }
  if (path->crop_y_easer.active) {
    path->crop_y = kr_easer_process(&path->crop_y_easer,
     path->crop_y, NULL);
  }
  if (path->crop_width_easer.active) {
    path->crop_width = kr_easer_process(&path->crop_width_easer,
     path->crop_width, NULL);
  }
  if (path->crop_height_easer.active) {
    path->crop_height = kr_easer_process(&path->crop_height_easer,
     path->crop_height, NULL);
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

void path_release(kr_compositor *compositor, kr_compositor_path *path) {
  if (path->perspective != NULL) {
    kr_perspective_destroy(&path->perspective);
  }
  if (path->converter != NULL) {
    sws_freeContext(path->converter);
    path->converter = NULL;
  }
  if (path->type == KR_CMP_INPUT) {
    compositor->active_input_paths--;
  }
  if (path->type == KR_CMP_OUTPUT) {
    compositor->active_output_paths--;
  }
  compositor->active_paths--;
}

kr_compositor_path *kr_compositor_mkpath(kr_compositor *compositor,
 kr_compositor_path_setup *setup) {


  return NULL;
}

int kr_compositor_unlink(kr_compositor_path *path) {
  if (path == NULL) return -1;
  if (path->subunit.active != 1) return -2;
  path->subunit.active = 2;
  return 0;
}
