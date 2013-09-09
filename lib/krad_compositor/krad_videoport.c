static int local_videoport_notify(krad_compositor_port_t *port);
static void port_destroy_actual(kr_compositor *compositor, krad_compositor_port_t *port);

void krad_compositor_port_tick(krad_compositor_port_t *port) {
  krad_compositor_subunit_tick(&port->subunit);
  if (port->crop_width_easer.active) {
    port->crop_width = kr_easer_process(&port->crop_width_easer,
     port->crop_width, NULL);
  }
  if (port->crop_height_easer.active) {
    port->crop_height = kr_easer_process(&port->crop_height_easer,
     port->crop_height, NULL);
  }
}

void kr_videoport_render(krad_compositor_port_t *port, cairo_t *cr) {

  krad_frame_t *frame;

  frame = krad_compositor_port_pull_frame(port);
  if (frame != NULL) {
    if (port->subunit.opacity > 0.0f) {
      cairo_save (cr);
      if (port->local == 1) {
        if ((port->subunit.width != port->source_width) ||
            (port->subunit.height != port->source_height)) {
          port->subunit.xscale = (float)port->subunit.width / port->source_width;
          port->subunit.yscale = (float)port->subunit.height / port->source_height;
        } else {
          port->subunit.xscale = 1.0f;
          port->subunit.yscale = 1.0f;
        }
        if ((port->subunit.xscale != 1.0f) || (port->subunit.yscale != 1.0f)) {
          cairo_translate(cr, port->subunit.x, port->subunit.y);
          cairo_translate(cr, ((port->subunit.width / 2) * port->subunit.xscale),
           ((port->subunit.height / 2) * port->subunit.yscale));
          cairo_scale(cr, port->subunit.xscale, port->subunit.yscale);
          cairo_translate(cr, port->subunit.width / -2, port->subunit.height / -2);
          cairo_translate(cr, port->subunit.x * -1, port->subunit.y * -1);
        }
      }
      if (port->subunit.rotation != 0.0f) {
        cairo_translate(cr, port->subunit.x, port->subunit.y);
        cairo_translate(cr, frame->width / 2, frame->height / 2);
        cairo_rotate(cr, port->subunit.rotation * (M_PI/180.0));
        cairo_translate(cr, frame->width / -2, frame->height / -2);
        cairo_translate(cr, -port->subunit.x, -port->subunit.y);
      }
      cairo_set_source_surface(cr, frame->cst, port->subunit.x - port->crop_x,
       port->subunit.y - port->crop_y);
      cairo_rectangle(cr, port->subunit.x, port->subunit.y, frame->width,
       frame->height);
      cairo_clip(cr);
      if (port->subunit.opacity == 1.0f) {
        cairo_paint(cr);
      } else {
        cairo_paint_with_alpha(cr, port->subunit.opacity);
      }
      cairo_restore(cr);
    }
    if (port->local != 1) {
      krad_framepool_unref_frame(frame);
    }
    frame = NULL;
  }
}

static void krad_compositor_notify_local_ports(kr_compositor *compositor) {

  int i;
  int ret;

  for (i = 0; i < KC_MAX_PORTS; i++) {
    if ((compositor->port[i].subunit.active == 1) &&
        (compositor->port[i].direction == KR_CMP_INPUT) &&
        (compositor->port[i].local == 1)) {
      ret = local_videoport_notify(&compositor->port[i]);
      if (ret == -2) {
        krad_compositor_port_destroy(compositor, &compositor->port[i]);
      }
    }
  }
}

void krad_compositor_port_set_comp_params(krad_compositor_port_t *port, int x,
 int y, int width, int height, int crop_x, int crop_y, int crop_width,
 int crop_height, float opacity, float rotation) {

  printk ("comp params func called");

  if ((x != port->subunit.x) || (y != port->subunit.y)) {
    krad_compositor_subunit_set_xy(&port->subunit, x, y, 0);
  }
  port->subunit.width = width;
  port->subunit.height = height;
  port->crop_x = crop_x;
  port->crop_y = crop_y;
  port->crop_width = crop_width;
  port->crop_height = crop_height;
  if (opacity != port->subunit.opacity) {
    krad_compositor_subunit_set_opacity(&port->subunit, opacity, 0);
  }
  if (rotation != port->subunit.rotation) {
    krad_compositor_subunit_set_rotation(&port->subunit, rotation, 0);
  }
  port->comp_params_updated = 1;
}

void krad_compositor_port_set_source_size(krad_compositor_port_t *port,
                                          int width, int height) {

  int x;
  int y;

  x = 0;
  y = 0;

  printk("comp set_source_size func called %p %dx%d", port, width, height);

  port->source_width = width;
  port->source_height = height;

  kr_aspect_upscale(port->source_width, port->source_height,
   port->krad_compositor->width, port->krad_compositor->height,
   &port->subunit.width, &port->subunit.height);

  port->crop_width = port->subunit.width;
  port->crop_height = port->subunit.height;

  if (port->subunit.width < port->krad_compositor->width) {
    x = (port->krad_compositor->width - port->subunit.width) / 2;
  }

  if (port->subunit.height < port->krad_compositor->height) {
    y = (port->krad_compositor->height - port->subunit.height) / 2;
  }

  krad_compositor_subunit_set_xy (&port->subunit, x, y, 0);
  port->io_params_updated = 1;
}

krad_frame_t *krad_compositor_port_pull_yuv_frame(krad_compositor_port_t *port,
 uint8_t *yuv_pixels[4], int yuv_strides[4], int color_depth) {

  krad_frame_t *krad_frame;

  if (krad_ringbuffer_read_space(port->frame_ring) >= sizeof(krad_frame_t *)) {
    krad_ringbuffer_read (port->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));

    int rgb_stride_arr[3] = {4*port->krad_compositor->width, 0, 0};
    unsigned char *src[4];

    if (port->yuv_color_depth != color_depth) {
      port->yuv_color_depth = color_depth;
    }

    port->sws_converter = sws_getCachedContext(port->sws_converter,
     port->krad_compositor->width, port->krad_compositor->height,
     PIX_FMT_RGB32, port->subunit.width, port->subunit.height,
     port->yuv_color_depth, port->sws_algorithm, NULL, NULL, NULL);

    if (port->sws_converter == NULL) {
      failfast("Krad Compositor: could not sws_getCachedContext");
    }

    src[0] = (unsigned char *)krad_frame->pixels;

    sws_scale(port->sws_converter, (const uint8_t * const*)src,
     rgb_stride_arr, 0, port->krad_compositor->height, yuv_pixels, yuv_strides);

    return krad_frame;
  }

  return NULL;
}

void inport_push_with_perspective(krad_compositor_port_t *port,
 krad_frame_t *inframe) {

  krad_frame_t *frame;

  frame = krad_framepool_getframe(port->krad_compositor->framepool);
  frame->width = inframe->width;
  frame->height = inframe->height;
  frame->timecode = inframe->timecode;
  frame->format = inframe->format;

  kr_perspective_argb(port->perspective, (uint8_t *)frame->pixels,
   (uint8_t *)inframe->pixels);
  krad_compositor_port_push_frame(port, frame);
  krad_framepool_unref_frame(frame);
}

void krad_compositor_port_push_yuv_frame(krad_compositor_port_t *port,
 krad_frame_t *krad_frame) {

  int dststride;

  if (port->krad_compositor->width > port->subunit.width) {
    dststride = port->krad_compositor->width;
  } else {
    dststride = port->subunit.width;
  }

  int rgb_stride_arr[3] = {4*dststride, 0, 0};
  unsigned char *dst[4];

  krad_compositor_port_tick(port);

  if (port->yuv_color_depth != krad_frame->format) {
    port->yuv_color_depth = krad_frame->format;
  }

  port->sws_converter = sws_getCachedContext(port->sws_converter,
   port->source_width, port->source_height, port->yuv_color_depth,
   port->subunit.width, port->subunit.height, PIX_FMT_RGB32,
   port->sws_algorithm, NULL, NULL, NULL);

  if (port->sws_converter == NULL) {
    failfast("Krad Compositor: could not sws_getCachedContext");
  }

  dst[0] = (unsigned char *)krad_frame->pixels;
  sws_scale(port->sws_converter, (const uint8_t * const*)krad_frame->yuv_pixels,
   krad_frame->yuv_strides, 0, port->source_height, dst, rgb_stride_arr);

  krad_frame->width = port->subunit.width;
  krad_frame->height = port->subunit.height;

  if (port->perspective != NULL) {
    inport_push_with_perspective(port, krad_frame);
  } else {
    krad_compositor_port_push_frame(port, krad_frame);
  }
}

void krad_compositor_port_push_rgba_frame(krad_compositor_port_t *port,
 krad_frame_t *krad_frame) {

  int output_rgb_stride_arr[4] = {4*port->krad_compositor->width, 0, 0, 0};
  int input_rgb_stride_arr[4] = {4*port->source_width, 0, 0, 0};
  unsigned char *dst[4];
  unsigned char *src[4];
  krad_frame_t *scaled_frame;

  krad_frame->format = PIX_FMT_RGB32;

  krad_compositor_port_tick(port);

  if ((port->source_width != port->subunit.width) ||
      (port->source_height != port->subunit.height)) {

    port->sws_converter = sws_getCachedContext(port->sws_converter,
     port->source_width, port->source_height, krad_frame->format,
     port->subunit.width, port->subunit.height, PIX_FMT_RGB32,
     port->sws_algorithm, NULL, NULL, NULL);

    if (port->sws_converter == NULL) {
      failfast("Krad Compositor: could not sws_getCachedContext");
    }

    while (krad_ringbuffer_read_space(port->frame_ring) >= (sizeof(krad_frame_t *) * 30)) {
      usleep(18000);
      //kludge to not buffer more than 1 handfull? of frames ahead for fast sources
    }

    scaled_frame = krad_framepool_getframe(port->krad_compositor->framepool);

    if (scaled_frame == NULL) {
      failfast("Krad Compositor: out of frames");
    }

    src[0] = (unsigned char *)krad_frame->pixels;
    dst[0] = (unsigned char *)scaled_frame->pixels;

    sws_scale(port->sws_converter, (const uint8_t * const*)src,
     input_rgb_stride_arr, 0, port->source_height, dst, output_rgb_stride_arr);

    krad_frame->width = port->subunit.width;
    krad_frame->height = port->subunit.height;

    if (port->perspective != NULL) {
      inport_push_with_perspective(port, scaled_frame);
    } else {
      krad_compositor_port_push_frame(port, scaled_frame);
    }
    krad_framepool_unref_frame(scaled_frame);
  } else {
    krad_frame->width = port->subunit.width;
    krad_frame->height = port->subunit.height;
    if (port->perspective != NULL) {
      inport_push_with_perspective(port, krad_frame);
    } else {
      krad_compositor_port_push_frame(port, krad_frame);
    }
  }
}

void krad_compositor_port_push_frame(krad_compositor_port_t *port,
 krad_frame_t *frame) {

  int ret;
  char buf[8];
  struct pollfd pollfds[1];
  buf[0] = 0;
  ret = 0;

  if (port->local != 1) {
    krad_framepool_ref_frame(frame);
    krad_ringbuffer_write(port->frame_ring, (char *)&frame,
     sizeof(krad_frame_t *));
    if (port->direction == KR_CMP_OUTPUT) {
      ret = write(port->socketpair[0], "0", 1);
      if (ret != 1) {
        printk("Compositor: port_push_frame unexpected write return value %d",
         ret);
      }
    }
  } else {

    memcpy(port->local_frame->pixels, frame->pixels,
     port->krad_compositor->width * port->krad_compositor->height * 4);

    pollfds[0].events = POLLOUT;
    pollfds[0].fd = port->msg_sd;

    ret = poll(pollfds, 1, 3);
    if (ret < 0) {
      printke("krad compositor poll failure %d", ret);
      ret = -1;
    }
    if (ret == 0) {
      printke("krad compositor : videoport poll write timeout", ret);
      ret = -1;
    }
    if (ret == 1) {
      if (pollfds[0].revents & POLLHUP) {
        printke("krad compositor: videoport poll hangup", ret);
        ret = -4;
      }
      if (pollfds[0].revents & POLLERR) {
        printke("krad compositor: videoport poll error", ret);
        ret = -5;
      }
      if (pollfds[0].revents & POLLOUT) {
        ret = write(port->msg_sd, buf, 1);
        if (ret == 1) {
          // good
          //return 0;
        }
      }
    }

    ret = 0;
    buf[0] = 0;

    pollfds[0].events = POLLIN;
    pollfds[0].fd = port->msg_sd;

    ret = poll(pollfds, 1, 0);

    if (ret < 0) {
      printke("krad compositor poll failure %d", ret);
      //return -1;
    }
    if (ret == 0) {
      printke("krad compositor : videoport poll readr timeout", ret);
      //return -1;
    }
    if (ret == 1) {
      if (pollfds[0].revents & POLLHUP) {
        printke("krad compositor: videoport poll hangup", ret);
        ret = -7;
      }
      if (pollfds[0].revents & POLLERR) {
        printke("krad compositor: videoport poll error", ret);
        ret = -6;
      }
      if (pollfds[0].revents & POLLIN) {
        ret = read(port->msg_sd, buf, 8);
        if (ret > 0) {
          // good
          //return 0;
        }
        if (ret == 0) {
          //close connection so drop teh port
          ret = -9;
        }
      }
    }
  }
  // not good, not that bad tho
  if (ret < 0) {
    krad_compositor_port_destroy(port->krad_compositor, port);
  }
}

static int local_videoport_notify(krad_compositor_port_t *port) {

  int ret;
  int wrote;
  char buf[1];
  struct pollfd pollfds[1];

  ret = 0;
  wrote = 0;
  buf[0] = 0;

  krad_compositor_port_tick(port);

  if (port->localframe_state == 1) {
    return 0;
  }

  port->local_frame->width = port->source_width;
  port->local_frame->height = port->source_height;

  cairo_surface_flush(port->local_frame->cst);

  pollfds[0].events = POLLOUT;
  pollfds[0].fd = port->msg_sd;

  ret = poll(pollfds, 1, 0);

  if (ret < 0) {
    printke("krad compositor poll failure %d", ret);
    return -2;
  }
  if (ret == 0) {
    printke("krad compositor : videoport poll write2 timeout", ret);
    return -1;
  }
  if (ret == 1) {
    if (pollfds[0].revents & POLLHUP) {
      printke("krad compositor: videoport poll hangup", ret);
      return -2;
    }
    if (pollfds[0].revents & POLLERR) {
      printke("krad compositor: videoport poll error", ret);
      return -2;
    }
    if (pollfds[0].revents & POLLOUT) {
      wrote = write(port->msg_sd, buf, 1);
      if (wrote == 1) {
        port->localframe_state = 1;
      }
      return wrote;
    }
  }
  return ret;
}

krad_frame_t *krad_compositor_port_pull_frame_local(krad_compositor_port_t *port) {

  int ret;
  char buf[1];
  struct pollfd pollfds[1];

  ret = 0;
  buf[0] = 0;

  if (port->localframe_state == 0) {
    return NULL;
  }

  pollfds[0].events = POLLIN;
  pollfds[0].fd = port->msg_sd;

  ret = poll(pollfds, 1, 0);
  if (ret < 0) {
    printke("krad compositor poll failure %d", ret);
    return NULL;
  }
  if (ret == 0) {
    printke("krad compositor : videoport poll read timeout", ret);
    return NULL;
  }
  if (ret == 1) {
    if (pollfds[0].revents & POLLHUP) {
      printke("krad compositor: videoport poll hangup", ret);
      return NULL;
    }
    if (pollfds[0].revents & POLLERR) {
      printke("krad compositor: videoport poll error", ret);
      return NULL;
    }
    if (pollfds[0].revents & POLLIN) {
      ret = read(port->msg_sd, buf, 1);
      if (ret == 1) {
        port->localframe_state = 0;
        cairo_surface_mark_dirty(port->local_frame->cst);
        return port->local_frame;
      }
    }
  }
  return NULL;
}

krad_frame_t *krad_compositor_port_pull_frame(krad_compositor_port_t *port) {

  krad_frame_t *krad_frame;

  if (port->local == 1) {
    return krad_compositor_port_pull_frame_local(port);
  }
  if (port->direction == KR_CMP_INPUT) {
    if (port->last_frame != NULL) {
      if ((port->start_timecode + port->last_frame->timecode) <
        port->krad_compositor->timecode) {
        if (krad_ringbuffer_read_space(port->frame_ring) >= sizeof(krad_frame_t *)) {
          krad_ringbuffer_read(port->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));
          krad_framepool_unref_frame(port->last_frame);
          krad_framepool_ref_frame(krad_frame);
          port->last_frame = krad_frame;
          return krad_frame;
        } else {
          krad_framepool_ref_frame(port->last_frame);
          return port->last_frame;
        }
      } else {
        krad_framepool_ref_frame(port->last_frame);
        return port->last_frame;
      }
    } else {
      if (krad_ringbuffer_read_space(port->frame_ring) >= sizeof(krad_frame_t *)) {
        krad_ringbuffer_read(port->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));
        port->start_timecode = port->krad_compositor->timecode;
        krad_framepool_ref_frame(krad_frame);
        port->last_frame = krad_frame;
        return krad_frame;
      }
    }
  } else {
    if (krad_ringbuffer_read_space(port->frame_ring) >= sizeof(krad_frame_t *)) {
      krad_ringbuffer_read(port->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));
      return krad_frame;
    }
  }
  return NULL;
}

int krad_compositor_port_frames_avail(krad_compositor_port_t *port) {

  int frames;

  frames = krad_ringbuffer_read_space(port->frame_ring);
  frames = frames / sizeof(krad_frame_t *);
  return frames;
}

int krad_compositor_port_get_fd(krad_compositor_port_t *port) {
  return port->socketpair[1];
}

krad_compositor_port_t *krad_compositor_port_create(kr_compositor *compositor,
 char *sysname, int direction, int width, int height) {

  return krad_compositor_port_create_full(compositor, sysname, direction, width, height, 0, 0);
}

krad_compositor_port_t *krad_compositor_port_create_full(kr_compositor *krad_compositor,
 char *sysname, int direction, int width, int height, int holdlock, int local) {

  krad_compositor_port_t *port;
  int p;
  int x;
  int y;

  x = 0;
  y = 0;
  port = NULL;

  for (p = 0; p < KC_MAX_PORTS; p++) {
    if (krad_compositor->port[p].subunit.active == 0) {
      port = &krad_compositor->port[p];
      port->subunit.address.path.unit = KR_COMPOSITOR;
      port->subunit.address.path.subunit.compositor_subunit = KR_VIDEOPORT;
      port->subunit.address.id.number = p;
      port->subunit.active = 2;
      break;
    }
  }

  if (port == NULL) {
    return NULL;
  }

  port->sws_algorithm = SWS_BILINEAR;
  port->krad_compositor = krad_compositor;
  port->local = local;
  port->direction = direction;
  strncpy (port->sysname, sysname, sizeof(port->sysname));
  port->start_timecode = 1;

  port->crop_x = 0;
  port->crop_y = 0;

  if (port->direction == KR_CMP_INPUT) {
    port->source_width = width;
    port->source_height = height;

    port->view.top_left.x = 0;
    port->view.top_left.y = 0;
    port->view.top_right.x = port->source_width - 1;
    port->view.top_right.y = 0;
    port->view.bottom_left.x = 0;
    port->view.bottom_left.y = port->source_height - 1;
    port->view.bottom_right.x = port->source_width - 1;
    port->view.bottom_right.y = port->source_height - 1;

    kr_aspect_upscale(port->source_width, port->source_height,
     krad_compositor->width, krad_compositor->height, &port->subunit.width,
     &port->subunit.height);

    if (port->subunit.width < port->krad_compositor->width) {
      x = (port->krad_compositor->width - port->subunit.width) / 2;
    }

    if (port->subunit.height < port->krad_compositor->height) {
      y = (port->krad_compositor->height - port->subunit.height) / 2;
    }

    krad_compositor_subunit_set_xy (&port->subunit, x, y, 0);

    port->crop_width = port->subunit.width;
    port->crop_height = port->subunit.height;

  } else {

    if (port->local != 1) {
      //KR_CMP_OUTPUT
      if (socketpair(AF_UNIX, SOCK_STREAM, 0, port->socketpair)) {
        printk ("Krad Compositor: subunit could not create socketpair errno: %d", errno);
        return NULL;
      }
      port->yuv_color_depth = PIX_FMT_YUV420P;
    }

    port->source_width = krad_compositor->width;
    port->source_height = krad_compositor->height;
    port->subunit.width = width;
    port->subunit.height = height;
    port->crop_width = krad_compositor->width;
    port->crop_height = krad_compositor->height;
  }

  port->frame_ring =
  krad_ringbuffer_create ( DEFAULT_COMPOSITOR_BUFFER_FRAMES * sizeof(krad_frame_t *) );

  krad_compositor->active_ports++;
  if (port->direction == KR_CMP_INPUT) {
    krad_compositor->active_input_ports++;
  }
  if (port->direction == KR_CMP_OUTPUT) {
    krad_compositor->active_output_ports++;
  }

  if (holdlock == 0) {
    port->subunit.active = 1;
  }

  if (krad_compositor->had_a_subunit == 0) {
    krad_compositor->had_a_subunit = 1;
  }

  return port;
}

krad_compositor_port_t *krad_compositor_local_port_create(kr_compositor *krad_compositor,
                               char *sysname, int direction, int shm_sd, int msg_sd) {

  krad_compositor_port_t *port;
  port = krad_compositor_port_create_full (krad_compositor, sysname, direction,
                            krad_compositor->width, krad_compositor->height, 1, 1);

  port->localframe_state = 0;
  port->shm_sd = 0;
  port->msg_sd = 0;
  port->local_buffer = NULL;
  port->local_buffer_size = krad_compositor->width * krad_compositor->height * 4 * 2;

  port->shm_sd = shm_sd;
  port->msg_sd = msg_sd;

  port->local_buffer = mmap(NULL, port->local_buffer_size,
                         PROT_READ | PROT_WRITE, MAP_SHARED,
                         port->shm_sd, 0);

  if ((port->local_buffer != NULL) && (port->shm_sd != 0) &&
    (port->msg_sd != 0)) {

    krad_system_set_socket_nonblocking (port->msg_sd);

    port->local_frame = calloc (1, sizeof(krad_frame_t));
    if (port->local_frame == NULL) {
      failfast ("oh dear im out of mem");
    }

    port->local_frame->pixels = (int *)port->local_buffer;

    port->local_frame->cst =
      cairo_image_surface_create_for_data ((unsigned char *)port->local_buffer,
                         CAIRO_FORMAT_ARGB32,
                         krad_compositor->width,
                         krad_compositor->height,
                         krad_compositor->width * 4);

    port->local_frame->cr = cairo_create (port->local_frame->cst);

    port->subunit.active = 1;
  } else {
    printke ("Krad Compositor: failed to create local port");
    krad_compositor_port_destroy (krad_compositor, port);
    return NULL;
  }

  printk("HIO!");

  return port;
}

void krad_compositor_port_destroy(kr_compositor *krad_compositor, krad_compositor_port_t *port) {
  port->subunit.active = 2;
}

static void port_destroy_actual(kr_compositor *krad_compositor, krad_compositor_port_t *port) {

  if (port->direction == KR_CMP_INPUT) {
    krad_compositor->active_input_ports--;
    if (port->perspective != NULL) {
      kr_perspective_destroy (&port->perspective);
    }
  }
  if (port->direction == KR_CMP_OUTPUT) {
    krad_compositor->active_output_ports--;
    if (port->local != 1) {
      close (port->socketpair[0]);
      close (port->socketpair[1]);
    }
  }

   if (port->local == 1) {
    if (port->msg_sd != 0) {
      close (port->msg_sd);
      port->msg_sd = 0;
    }
    if (port->shm_sd != 0) {
      close (port->shm_sd);
      port->shm_sd = 0;
    }
    if (port->local_buffer != NULL) {
      munmap (port->local_buffer, port->local_buffer_size);
      port->local_buffer_size = 0;
      port->local_buffer = NULL;
    }

    if (port->local_frame != NULL) {
      cairo_destroy (port->local_frame->cr);
      cairo_surface_destroy (port->local_frame->cst);
      free (port->local_frame);
    }
    port->local = 0;
   }

  if (port->frame_ring != NULL) {
    krad_ringbuffer_free ( port->frame_ring );
    port->frame_ring = NULL;
  }
  port->start_timecode = 0;

  if (port->sws_converter != NULL) {
    sws_freeContext ( port->sws_converter );
    port->sws_converter = NULL;
  }

  if (port->last_frame != NULL) {
    krad_framepool_unref_frame (port->last_frame);
    port->last_frame = NULL;
  }
  krad_compositor->active_ports--;
}
