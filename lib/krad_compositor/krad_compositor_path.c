static int local_videopath_notify(kr_compositor_path *path);
static void path_destroy_actual(kr_compositor *compositor, kr_compositor_path *path);

void kr_compositor_path_tick(kr_compositor_path *path) {
  krad_compositor_subunit_tick(&path->subunit);
  if (path->crop_width_easer.active) {
    path->crop_width = kr_easer_process(&path->crop_width_easer,
     path->crop_width, NULL);
  }
  if (path->crop_height_easer.active) {
    path->crop_height = kr_easer_process(&path->crop_height_easer,
     path->crop_height, NULL);
  }
}

void kr_videopath_render(kr_compositor_path *path, cairo_t *cr) {

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
}

static void krad_compositor_notify_local_paths(kr_compositor *compositor) {

  int i;
  int ret;

  for (i = 0; i < KC_MAX_PORTS; i++) {
    if ((compositor->path[i].subunit.active == 1) &&
        (compositor->path[i].direction == KR_CMP_INPUT) &&
        (compositor->path[i].local == 1)) {
      ret = local_videopath_notify(&compositor->path[i]);
      if (ret == -2) {
        //krad_compositor_path_destroy(compositor, &compositor->path[i]);
      }
    }
  }
}

void krad_compositor_path_set_comp_params(kr_compositor_path *path, int x,
 int y, int width, int height, int crop_x, int crop_y, int crop_width,
 int crop_height, float opacity, float rotation) {

  printk ("comp params func called");

  if ((x != path->subunit.x) || (y != path->subunit.y)) {
    krad_compositor_subunit_set_xy(&path->subunit, x, y, 0);
  }
  path->subunit.width = width;
  path->subunit.height = height;
  path->crop_x = crop_x;
  path->crop_y = crop_y;
  path->crop_width = crop_width;
  path->crop_height = crop_height;
  if (opacity != path->subunit.opacity) {
    krad_compositor_subunit_set_opacity(&path->subunit, opacity, 0);
  }
  if (rotation != path->subunit.rotation) {
    krad_compositor_subunit_set_rotation(&path->subunit, rotation, 0);
  }
  path->comp_params_updated = 1;
}

void krad_compositor_path_set_source_size(kr_compositor_path *path,
                                          int width, int height) {

  int x;
  int y;

  x = 0;
  y = 0;

  printk("comp set_source_size func called %p %dx%d", path, width, height);

  path->source_width = width;
  path->source_height = height;

  kr_aspect_upscale(path->source_width, path->source_height,
   path->compositor->width, path->compositor->height,
   &path->subunit.width, &path->subunit.height);

  path->crop_width = path->subunit.width;
  path->crop_height = path->subunit.height;

  if (path->subunit.width < path->compositor->width) {
    x = (path->compositor->width - path->subunit.width) / 2;
  }

  if (path->subunit.height < path->compositor->height) {
    y = (path->compositor->height - path->subunit.height) / 2;
  }

  krad_compositor_subunit_set_xy (&path->subunit, x, y, 0);
  path->io_params_updated = 1;
}

krad_frame_t *krad_compositor_path_pull_yuv_frame(kr_compositor_path *path,
 uint8_t *yuv_pixels[4], int yuv_strides[4], int color_depth) {

  krad_frame_t *krad_frame;

  if (krad_ringbuffer_read_space(path->frame_ring) >= sizeof(krad_frame_t *)) {
    krad_ringbuffer_read (path->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));

    int rgb_stride_arr[3] = {4*path->compositor->width, 0, 0};
    unsigned char *src[4];

    if (path->yuv_color_depth != color_depth) {
      path->yuv_color_depth = color_depth;
    }

    path->sws_converter = sws_getCachedContext(path->sws_converter,
     path->compositor->width, path->compositor->height,
     PIX_FMT_RGB32, path->subunit.width, path->subunit.height,
     path->yuv_color_depth, path->sws_algorithm, NULL, NULL, NULL);

    if (path->sws_converter == NULL) {
      failfast("Krad Compositor: could not sws_getCachedContext");
    }

    src[0] = (unsigned char *)krad_frame->pixels;

    sws_scale(path->sws_converter, (const uint8_t * const*)src,
     rgb_stride_arr, 0, path->compositor->height, yuv_pixels, yuv_strides);

    return krad_frame;
  }

  return NULL;
}

void inpath_push_with_perspective(kr_compositor_path *path,
 krad_frame_t *inframe) {

  krad_frame_t *frame;

  frame = krad_framepool_getframe(path->compositor->framepool);
  frame->width = inframe->width;
  frame->height = inframe->height;
  frame->timecode = inframe->timecode;
  frame->format = inframe->format;

  kr_perspective_argb(path->perspective, (uint8_t *)frame->pixels,
   (uint8_t *)inframe->pixels);
  krad_compositor_path_push_frame(path, frame);
  krad_framepool_unref_frame(frame);
}

void krad_compositor_path_push_yuv_frame(kr_compositor_path *path,
 krad_frame_t *krad_frame) {

  int dststride;

  if (path->compositor->width > path->subunit.width) {
    dststride = path->compositor->width;
  } else {
    dststride = path->subunit.width;
  }

  int rgb_stride_arr[3] = {4*dststride, 0, 0};
  unsigned char *dst[4];

  kr_compositor_path_tick(path);

  if (path->yuv_color_depth != krad_frame->format) {
    path->yuv_color_depth = krad_frame->format;
  }

  path->sws_converter = sws_getCachedContext(path->sws_converter,
   path->source_width, path->source_height, path->yuv_color_depth,
   path->subunit.width, path->subunit.height, PIX_FMT_RGB32,
   path->sws_algorithm, NULL, NULL, NULL);

  if (path->sws_converter == NULL) {
    failfast("Krad Compositor: could not sws_getCachedContext");
  }

  dst[0] = (unsigned char *)krad_frame->pixels;
  sws_scale(path->sws_converter, (const uint8_t * const*)krad_frame->yuv_pixels,
   krad_frame->yuv_strides, 0, path->source_height, dst, rgb_stride_arr);

  krad_frame->width = path->subunit.width;
  krad_frame->height = path->subunit.height;

  if (path->perspective != NULL) {
    inpath_push_with_perspective(path, krad_frame);
  } else {
    krad_compositor_path_push_frame(path, krad_frame);
  }
}

void krad_compositor_path_push_rgba_frame(kr_compositor_path *path,
 krad_frame_t *krad_frame) {

  int output_rgb_stride_arr[4] = {4*path->compositor->width, 0, 0, 0};
  int input_rgb_stride_arr[4] = {4*path->source_width, 0, 0, 0};
  unsigned char *dst[4];
  unsigned char *src[4];
  krad_frame_t *scaled_frame;

  krad_frame->format = PIX_FMT_RGB32;

  kr_compositor_path_tick(path);

  if ((path->source_width != path->subunit.width) ||
      (path->source_height != path->subunit.height)) {

    path->sws_converter = sws_getCachedContext(path->sws_converter,
     path->source_width, path->source_height, krad_frame->format,
     path->subunit.width, path->subunit.height, PIX_FMT_RGB32,
     path->sws_algorithm, NULL, NULL, NULL);

    if (path->sws_converter == NULL) {
      failfast("Krad Compositor: could not sws_getCachedContext");
    }

    while (krad_ringbuffer_read_space(path->frame_ring) >= (sizeof(krad_frame_t *) * 30)) {
      usleep(18000);
      //kludge to not buffer more than 1 handfull? of frames ahead for fast sources
    }

    scaled_frame = krad_framepool_getframe(path->compositor->framepool);

    if (scaled_frame == NULL) {
      failfast("Krad Compositor: out of frames");
    }

    src[0] = (unsigned char *)krad_frame->pixels;
    dst[0] = (unsigned char *)scaled_frame->pixels;

    sws_scale(path->sws_converter, (const uint8_t * const*)src,
     input_rgb_stride_arr, 0, path->source_height, dst, output_rgb_stride_arr);

    krad_frame->width = path->subunit.width;
    krad_frame->height = path->subunit.height;

    if (path->perspective != NULL) {
      inpath_push_with_perspective(path, scaled_frame);
    } else {
      krad_compositor_path_push_frame(path, scaled_frame);
    }
    krad_framepool_unref_frame(scaled_frame);
  } else {
    krad_frame->width = path->subunit.width;
    krad_frame->height = path->subunit.height;
    if (path->perspective != NULL) {
      inpath_push_with_perspective(path, krad_frame);
    } else {
      krad_compositor_path_push_frame(path, krad_frame);
    }
  }
}

void krad_compositor_path_push_frame(kr_compositor_path *path,
 krad_frame_t *frame) {

  int ret;
  char buf[8];
  struct pollfd pollfds[1];
  buf[0] = 0;
  ret = 0;

  if (path->local != 1) {
    krad_framepool_ref_frame(frame);
    krad_ringbuffer_write(path->frame_ring, (char *)&frame,
     sizeof(krad_frame_t *));
    if (path->direction == KR_CMP_OUTPUT) {
      ret = write(path->socketpair[0], "0", 1);
      if (ret != 1) {
        printk("Compositor: path_push_frame unexpected write return value %d",
         ret);
      }
    }
  } else {

    memcpy(path->local_frame->pixels, frame->pixels,
     path->compositor->width * path->compositor->height * 4);

    pollfds[0].events = POLLOUT;
    pollfds[0].fd = path->msg_sd;

    ret = poll(pollfds, 1, 3);
    if (ret < 0) {
      printke("krad compositor poll failure %d", ret);
      ret = -1;
    }
    if (ret == 0) {
      printke("krad compositor : videopath poll write timeout", ret);
      ret = -1;
    }
    if (ret == 1) {
      if (pollfds[0].revents & POLLHUP) {
        printke("krad compositor: videopath poll hangup", ret);
        ret = -4;
      }
      if (pollfds[0].revents & POLLERR) {
        printke("krad compositor: videopath poll error", ret);
        ret = -5;
      }
      if (pollfds[0].revents & POLLOUT) {
        ret = write(path->msg_sd, buf, 1);
        if (ret == 1) {
          // good
          //return 0;
        }
      }
    }

    ret = 0;
    buf[0] = 0;

    pollfds[0].events = POLLIN;
    pollfds[0].fd = path->msg_sd;

    ret = poll(pollfds, 1, 0);

    if (ret < 0) {
      printke("krad compositor poll failure %d", ret);
      //return -1;
    }
    if (ret == 0) {
      printke("krad compositor : videopath poll readr timeout", ret);
      //return -1;
    }
    if (ret == 1) {
      if (pollfds[0].revents & POLLHUP) {
        printke("krad compositor: videopath poll hangup", ret);
        ret = -7;
      }
      if (pollfds[0].revents & POLLERR) {
        printke("krad compositor: videopath poll error", ret);
        ret = -6;
      }
      if (pollfds[0].revents & POLLIN) {
        ret = read(path->msg_sd, buf, 8);
        if (ret > 0) {
          // good
          //return 0;
        }
        if (ret == 0) {
          //close connection so drop teh path
          ret = -9;
        }
      }
    }
  }
  // not good, not that bad tho
  if (ret < 0) {
    //krad_compositor_path_destroy(path->compositor, path);
  }
}

static int local_videopath_notify(kr_compositor_path *path) {

  int ret;
  int wrote;
  char buf[1];
  struct pollfd pollfds[1];

  ret = 0;
  wrote = 0;
  buf[0] = 0;

  kr_compositor_path_tick(path);

  if (path->localframe_state == 1) {
    return 0;
  }

  path->local_frame->width = path->source_width;
  path->local_frame->height = path->source_height;

  cairo_surface_flush(path->local_frame->cst);

  pollfds[0].events = POLLOUT;
  pollfds[0].fd = path->msg_sd;

  ret = poll(pollfds, 1, 0);

  if (ret < 0) {
    printke("krad compositor poll failure %d", ret);
    return -2;
  }
  if (ret == 0) {
    printke("krad compositor : videopath poll write2 timeout", ret);
    return -1;
  }
  if (ret == 1) {
    if (pollfds[0].revents & POLLHUP) {
      printke("krad compositor: videopath poll hangup", ret);
      return -2;
    }
    if (pollfds[0].revents & POLLERR) {
      printke("krad compositor: videopath poll error", ret);
      return -2;
    }
    if (pollfds[0].revents & POLLOUT) {
      wrote = write(path->msg_sd, buf, 1);
      if (wrote == 1) {
        path->localframe_state = 1;
      }
      return wrote;
    }
  }
  return ret;
}

krad_frame_t *krad_compositor_path_pull_frame_local(kr_compositor_path *path) {

  int ret;
  char buf[1];
  struct pollfd pollfds[1];

  ret = 0;
  buf[0] = 0;

  if (path->localframe_state == 0) {
    return NULL;
  }

  pollfds[0].events = POLLIN;
  pollfds[0].fd = path->msg_sd;

  ret = poll(pollfds, 1, 0);
  if (ret < 0) {
    printke("krad compositor poll failure %d", ret);
    return NULL;
  }
  if (ret == 0) {
    printke("krad compositor : videopath poll read timeout", ret);
    return NULL;
  }
  if (ret == 1) {
    if (pollfds[0].revents & POLLHUP) {
      printke("krad compositor: videopath poll hangup", ret);
      return NULL;
    }
    if (pollfds[0].revents & POLLERR) {
      printke("krad compositor: videopath poll error", ret);
      return NULL;
    }
    if (pollfds[0].revents & POLLIN) {
      ret = read(path->msg_sd, buf, 1);
      if (ret == 1) {
        path->localframe_state = 0;
        cairo_surface_mark_dirty(path->local_frame->cst);
        return path->local_frame;
      }
    }
  }
  return NULL;
}

krad_frame_t *krad_compositor_path_pull_frame(kr_compositor_path *path) {

  krad_frame_t *krad_frame;

  if (path->local == 1) {
    return krad_compositor_path_pull_frame_local(path);
  }
  if (path->direction == KR_CMP_INPUT) {
    if (path->last_frame != NULL) {
      if ((path->start_timecode + path->last_frame->timecode) <
        path->compositor->timecode) {
        if (krad_ringbuffer_read_space(path->frame_ring) >= sizeof(krad_frame_t *)) {
          krad_ringbuffer_read(path->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));
          krad_framepool_unref_frame(path->last_frame);
          krad_framepool_ref_frame(krad_frame);
          path->last_frame = krad_frame;
          return krad_frame;
        } else {
          krad_framepool_ref_frame(path->last_frame);
          return path->last_frame;
        }
      } else {
        krad_framepool_ref_frame(path->last_frame);
        return path->last_frame;
      }
    } else {
      if (krad_ringbuffer_read_space(path->frame_ring) >= sizeof(krad_frame_t *)) {
        krad_ringbuffer_read(path->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));
        path->start_timecode = path->compositor->timecode;
        krad_framepool_ref_frame(krad_frame);
        path->last_frame = krad_frame;
        return krad_frame;
      }
    }
  } else {
    if (krad_ringbuffer_read_space(path->frame_ring) >= sizeof(krad_frame_t *)) {
      krad_ringbuffer_read(path->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));
      return krad_frame;
    }
  }
  return NULL;
}

int krad_compositor_path_frames_avail(kr_compositor_path *path) {

  int frames;

  frames = krad_ringbuffer_read_space(path->frame_ring);
  frames = frames / sizeof(krad_frame_t *);
  return frames;
}

int krad_compositor_path_get_fd(kr_compositor_path *path) {
  return path->socketpair[1];
}

kr_compositor_path *krad_compositor_path_create(kr_compositor *compositor,
 char *sysname, int direction, int width, int height) {

  return krad_compositor_path_create_full(compositor, sysname, direction, width, height, 0, 0);
}

kr_compositor_path *krad_compositor_path_create_full(kr_compositor *krad_compositor,
 char *sysname, int direction, int width, int height, int holdlock, int local) {

  kr_compositor_path *path;
  int p;
  int x;
  int y;

  x = 0;
  y = 0;
  path = NULL;

  for (p = 0; p < KC_MAX_PORTS; p++) {
    if (krad_compositor->path[p].subunit.active == 0) {
      path = &krad_compositor->path[p];
      path->subunit.address.path.unit = KR_COMPOSITOR;
      path->subunit.address.path.subunit.compositor_subunit = KR_VIDEOPORT;
      path->subunit.address.id.number = p;
      path->subunit.active = 2;
      break;
    }
  }

  if (path == NULL) {
    return NULL;
  }

  path->sws_algorithm = SWS_BILINEAR;
  path->compositor = krad_compositor;
  path->local = local;
  path->direction = direction;
  strncpy (path->sysname, sysname, sizeof(path->sysname));
  path->start_timecode = 1;

  path->crop_x = 0;
  path->crop_y = 0;

  if (path->direction == KR_CMP_INPUT) {
    path->source_width = width;
    path->source_height = height;

    path->view.top_left.x = 0;
    path->view.top_left.y = 0;
    path->view.top_right.x = path->source_width - 1;
    path->view.top_right.y = 0;
    path->view.bottom_left.x = 0;
    path->view.bottom_left.y = path->source_height - 1;
    path->view.bottom_right.x = path->source_width - 1;
    path->view.bottom_right.y = path->source_height - 1;

    kr_aspect_upscale(path->source_width, path->source_height,
     krad_compositor->width, krad_compositor->height, &path->subunit.width,
     &path->subunit.height);

    if (path->subunit.width < path->compositor->width) {
      x = (path->compositor->width - path->subunit.width) / 2;
    }

    if (path->subunit.height < path->compositor->height) {
      y = (path->compositor->height - path->subunit.height) / 2;
    }

    krad_compositor_subunit_set_xy (&path->subunit, x, y, 0);

    path->crop_width = path->subunit.width;
    path->crop_height = path->subunit.height;

  } else {

    if (path->local != 1) {
      //KR_CMP_OUTPUT
      if (socketpair(AF_UNIX, SOCK_STREAM, 0, path->socketpair)) {
        printk ("Krad Compositor: subunit could not create socketpair errno: %d", errno);
        return NULL;
      }
      path->yuv_color_depth = PIX_FMT_YUV420P;
    }

    path->source_width = krad_compositor->width;
    path->source_height = krad_compositor->height;
    path->subunit.width = width;
    path->subunit.height = height;
    path->crop_width = krad_compositor->width;
    path->crop_height = krad_compositor->height;
  }

  path->frame_ring =
  krad_ringbuffer_create ( DEFAULT_COMPOSITOR_BUFFER_FRAMES * sizeof(krad_frame_t *) );

  krad_compositor->active_paths++;
  if (path->direction == KR_CMP_INPUT) {
    krad_compositor->active_input_paths++;
  }
  if (path->direction == KR_CMP_OUTPUT) {
    krad_compositor->active_output_paths++;
  }

  if (holdlock == 0) {
    path->subunit.active = 1;
  }

  return path;
}

kr_compositor_path *krad_compositor_local_path_create(kr_compositor *krad_compositor,
                               char *sysname, int direction, int shm_sd, int msg_sd) {

  kr_compositor_path *path;
  path = krad_compositor_path_create_full (krad_compositor, sysname, direction,
                            krad_compositor->width, krad_compositor->height, 1, 1);

  path->localframe_state = 0;
  path->shm_sd = 0;
  path->msg_sd = 0;
  path->local_buffer = NULL;
  path->local_buffer_size = krad_compositor->width * krad_compositor->height * 4 * 2;

  path->shm_sd = shm_sd;
  path->msg_sd = msg_sd;

  path->local_buffer = mmap(NULL, path->local_buffer_size,
                         PROT_READ | PROT_WRITE, MAP_SHARED,
                         path->shm_sd, 0);

  if ((path->local_buffer != NULL) && (path->shm_sd != 0) &&
    (path->msg_sd != 0)) {

    krad_system_set_socket_nonblocking (path->msg_sd);

    path->local_frame = calloc (1, sizeof(krad_frame_t));
    if (path->local_frame == NULL) {
      failfast ("oh dear im out of mem");
    }

    path->local_frame->pixels = (int *)path->local_buffer;

    path->local_frame->cst =
      cairo_image_surface_create_for_data ((unsigned char *)path->local_buffer,
                         CAIRO_FORMAT_ARGB32,
                         krad_compositor->width,
                         krad_compositor->height,
                         krad_compositor->width * 4);

    path->local_frame->cr = cairo_create (path->local_frame->cst);

    path->subunit.active = 1;
  } else {
    printke ("Krad Compositor: failed to create local path");
    //krad_compositor_path_destroy (krad_compositor, path);
    return NULL;
  }

  printk("HIO!");

  return path;
}

static void path_destroy_actual(kr_compositor *krad_compositor, kr_compositor_path *path) {

  if (path->direction == KR_CMP_INPUT) {
    krad_compositor->active_input_paths--;
    if (path->perspective != NULL) {
      kr_perspective_destroy (&path->perspective);
    }
  }
  if (path->direction == KR_CMP_OUTPUT) {
    krad_compositor->active_output_paths--;
    if (path->local != 1) {
      close (path->socketpair[0]);
      close (path->socketpair[1]);
    }
  }

   if (path->local == 1) {
    if (path->msg_sd != 0) {
      close (path->msg_sd);
      path->msg_sd = 0;
    }
    if (path->shm_sd != 0) {
      close (path->shm_sd);
      path->shm_sd = 0;
    }
    if (path->local_buffer != NULL) {
      munmap (path->local_buffer, path->local_buffer_size);
      path->local_buffer_size = 0;
      path->local_buffer = NULL;
    }

    if (path->local_frame != NULL) {
      cairo_destroy (path->local_frame->cr);
      cairo_surface_destroy (path->local_frame->cst);
      free (path->local_frame);
    }
    path->local = 0;
   }

  if (path->frame_ring != NULL) {
    krad_ringbuffer_free ( path->frame_ring );
    path->frame_ring = NULL;
  }
  path->start_timecode = 0;

  if (path->sws_converter != NULL) {
    sws_freeContext ( path->sws_converter );
    path->sws_converter = NULL;
  }

  if (path->last_frame != NULL) {
    krad_framepool_unref_frame (path->last_frame);
    path->last_frame = NULL;
  }
  krad_compositor->active_paths--;
}

int kr_compositor_unlink(kr_compositor_path *path) {
  if (path == NULL) return -1;
  if (path->subunit.active != 1) return -2;
  path->subunit.active = 2;
  return 0;
}
