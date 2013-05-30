#include "krad_compositor.h"
static int krad_compositor_local_videoport_notify (krad_compositor_port_t *port);
static void krad_compositor_port_destroy_actual (krad_compositor_t *krad_compositor, krad_compositor_port_t *port);
static void krad_compositor_aspect_scale (int width, int height,
                                          int avail_width, int avail_height,
                                          int *new_width, int *new_heigth);
static void krad_compositor_free_framepool (krad_compositor_t *compositor);
static void krad_compositor_alloc_framepool (krad_compositor_t *compositor);
static void krad_compositor_prepare_subunits (krad_compositor_t *compositor);
static void krad_compositor_deactivate_subunits (krad_compositor_t *compositor);
static void *krad_compositor_ticker_thread (void *arg);
static void krad_compositor_set_frame_rate (krad_compositor_t *krad_compositor,
                                            int fps_numerator, int fps_denominator);

void krad_compositor_unset_background (krad_compositor_t *krad_compositor) {
  if (krad_compositor->background->subunit.active != 1) {
    return;
  }
  krad_compositor->background->subunit.active = 2;
  usleep (100000);
  krad_sprite_reset (krad_compositor->background);
  krad_compositor->background->subunit.active = 0;
}

void krad_compositor_set_background (krad_compositor_t *krad_compositor, char *filename) {

  krad_compositor_unset_background (krad_compositor);

  if ((filename == NULL) || (strlen(filename) == 0)) {
    return;
  }
  
  if (krad_sprite_open_file (krad_compositor->background, filename)) {
    krad_compositor->background->subunit.active = 1;
  } else {
    krad_compositor->background->subunit.active = 0;    
  }
}

int krad_compositor_get_background_name (krad_compositor_t *krad_compositor, char **filename) {
  
  if ((filename == NULL) ||
      (krad_compositor->background->subunit.active != 1)) {
    return 0;
  }
  
  *filename = krad_compositor->background->filename;
  return 1;
}

static void krad_compositor_render_background (krad_compositor_t *krad_compositor) {

  if (krad_compositor->background->subunit.active != 1) {
    return;
  }
  cairo_save (krad_compositor->cr);
  if ((krad_compositor->background->subunit.width != krad_compositor->width) || (krad_compositor->background->subunit.height != krad_compositor->height)) {
    cairo_set_source (krad_compositor->cr, krad_compositor->background->sprite_pattern);
  } else {
    cairo_set_source_surface ( krad_compositor->cr, krad_compositor->background->sprite, 0, 0 );
  }
  cairo_paint ( krad_compositor->cr );
  cairo_restore (krad_compositor->cr);
}

int krad_compositor_has_background (krad_compositor_t *krad_compositor) {
  if (krad_compositor->background->subunit.active == 1) {
    return 1;
  }
  return 0;
}

void krad_compositor_render_no_input (krad_compositor_t *compositor) {
  cairo_save (compositor->cr);
  if ((compositor->frames % 24) < 12) {
    cairo_set_source_rgba (compositor->cr, RED, 0.0f + ((compositor->frames % 12) * 0.09f));
  } else {
    cairo_set_source_rgba (compositor->cr, RED, 1.0f + ((compositor->frames % 12) * -0.09f));
  }
  cairo_select_font_face (compositor->cr,
                          "",
                          CAIRO_FONT_SLANT_NORMAL,
                          CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size (compositor->cr, 42.0);
  cairo_move_to (compositor->cr, 64, 64 + 42);
  cairo_show_text (compositor->cr, "KR: No Input!");
  cairo_stroke (compositor->cr);
  cairo_restore (compositor->cr);
}

void krad_compositor_clear_frame (krad_compositor_t *compositor) {
  cairo_save (compositor->cr);
  cairo_set_source_rgba (compositor->cr, BGCOLOR_CLR);
  cairo_set_operator (compositor->cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (compositor->cr);
  cairo_restore (compositor->cr);
}

void krad_compositor_port_tick (krad_compositor_port_t *port) {

  krad_compositor_subunit_tick (&port->subunit);

  if (port->crop_width_easing.active) {
    port->crop_width = krad_easing_process (&port->crop_width_easing,
                                            port->crop_width, NULL);
  }
  if (port->crop_height_easing.active) {
    port->crop_height = krad_easing_process (&port->crop_height_easing,
                                             port->crop_height, NULL);
  }
}

void krad_compositor_videoport_render (krad_compositor_port_t *port,
                                       cairo_t *cr) {

  krad_frame_t *frame;
  
  frame = NULL;

  frame = krad_compositor_port_pull_frame (port);

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
          cairo_translate (cr, port->subunit.x, port->subunit.y);
          cairo_translate (cr, ((port->subunit.width / 2) * port->subunit.xscale),
                  ((port->subunit.height / 2) * port->subunit.yscale));
          cairo_scale (cr, port->subunit.xscale, port->subunit.yscale);
          cairo_translate (cr, port->subunit.width / -2, port->subunit.height / -2);    
          cairo_translate (cr, port->subunit.x * -1, port->subunit.y * -1);    
        }
      }

      if (port->subunit.rotation != 0.0f) {
        cairo_translate (cr, port->subunit.x, port->subunit.y);
        cairo_translate (cr, frame->width / 2, frame->height / 2);
        cairo_rotate (cr, port->subunit.rotation * (M_PI/180.0));
        cairo_translate (cr, frame->width / -2, frame->height / -2);
        cairo_translate (cr, -port->subunit.x, -port->subunit.y);
      }

      cairo_set_source_surface (cr, frame->cst,
                                port->subunit.x - port->crop_x,
                                port->subunit.y - port->crop_y);

      cairo_rectangle (cr,
                       port->subunit.x, port->subunit.y,
                       frame->width, frame->height);

      cairo_clip (cr);

      if (port->subunit.opacity == 1.0f) {
        cairo_paint (cr);
      } else {
        cairo_paint_with_alpha (cr, port->subunit.opacity);
      }

      cairo_restore (cr);
    }

      if (port->local != 1) {
        krad_framepool_unref_frame (frame);
      }
    frame = NULL;
  }
}

static void krad_compositor_prepare (krad_compositor_t *compositor) {

  int i;
  int ret;

  while (compositor->frame == NULL) {
    compositor->frame = krad_framepool_getframe (compositor->framepool);
    if (compositor->frame == NULL) {
      printke ("Compositor wanted a frame but could not get one right away!");
      usleep (5000);
    }
  }
  
  for (i = 0; i < KC_MAX_PORTS; i++) {
    if ((compositor->port[i].subunit.active == 1) &&
        (compositor->port[i].direction == INPUT) &&
        (compositor->port[i].local == 1)) {
      ret = krad_compositor_local_videoport_notify (&compositor->port[i]);
      if (ret == -2) {
        krad_compositor_port_destroy (compositor, &compositor->port[i]);
      }
    }
  }
  
  compositor->cr = cairo_create (compositor->frame->cst);

  /* Render background if exists */
  if (krad_compositor_has_background (compositor)) {
    krad_compositor_render_background (compositor);
  } else {
    /* No background, so clear frame */
    krad_compositor_clear_frame (compositor);
  }
}

static void krad_compositor_composite (krad_compositor_t *compositor) {

  int i;
  
  i = 0;

  /* Handle situation of maybe having absolutly no input */  
  if ((compositor->active_input_ports == 0) &&
      (compositor->active_sprites == 0) &&
      (compositor->active_texts == 0) &&
      (compositor->active_vectors == 0) && 
      (compositor->background->subunit.active == 0)) {
      krad_compositor_render_no_input (compositor);
  }

  for (i = 0; i < KC_MAX_PORTS; i++) {
    if ((compositor->port[i].subunit.active == 1) &&
        (compositor->port[i].direction == INPUT)) {
        krad_compositor_videoport_render (&compositor->port[i], compositor->cr);
    }
  }

  for (i = 0; i < KC_MAX_SPRITES; i++) {
    if (compositor->sprite[i].subunit.active == 1) {
      krad_sprite_render (&compositor->sprite[i], compositor->cr);
    }
  }

  for (i = 0; i < KC_MAX_VECTORS; i++) {
    if (compositor->vector[i].subunit.active == 1) {
      krad_vector_render (&compositor->vector[i], compositor->cr);
    }
  }

  for (i = 0; i < KC_MAX_TEXTS; i++) {
    if (compositor->text[i].subunit.active == 1) {
      krad_text_render (&compositor->text[i], compositor->cr);
    }
  }
}

static void krad_compositor_output (krad_compositor_t *compositor) {

  int p;

  p = 0;

  for (p = 0; p < KC_MAX_PORTS; p++) {
    if ((compositor->port[p].subunit.active == 1) &&
        (compositor->port[p].direction == OUTPUT)) {
      krad_compositor_port_push_frame (&compositor->port[p], compositor->frame);
    }
  }
}

static void krad_compositor_deactivate_subunits (krad_compositor_t *compositor) {

  int i;

  i = 0;

  for (i = 0; i < KC_MAX_PORTS; i++) {
    if (compositor->port[i].subunit.active == 2) {
        krad_compositor_port_destroy_actual (compositor, &compositor->port[i]);
        krad_compositor_subunit_reset (&compositor->port[i].subunit);
        compositor->port[i].subunit.active = 0;
    }
  }

  for (i = 0; i < KC_MAX_SPRITES; i++) {
    if (compositor->sprite[i].subunit.active == 2) {
      krad_sprite_reset (&compositor->sprite[i]);
      compositor->active_sprites--;
      compositor->sprite[i].subunit.active = 0;
    }
  }

  for (i = 0; i < KC_MAX_TEXTS; i++) {
    if (compositor->text[i].subunit.active == 2) {
      krad_text_reset (&compositor->text[i]);
      compositor->active_texts--;
      compositor->text[i].subunit.active = 0;
    }
  }
  
  for (i = 0; i < KC_MAX_VECTORS; i++) {
    if (compositor->vector[i].subunit.active == 2) {
      krad_vector_reset (&compositor->vector[i]);
      compositor->active_vectors--;
      compositor->vector[i].subunit.active = 0;
    }
  }
}

static void krad_compositor_finish (krad_compositor_t *compositor) {

  krad_framepool_unref_frame (compositor->frame);

  if (compositor->cr != NULL) {
    cairo_destroy (compositor->cr);
  }
  
  compositor->frame = NULL;
  
  krad_compositor_deactivate_subunits (compositor); 
}

static inline void krad_compositor_tick (krad_compositor_t *compositor) {
  compositor->timecode = round (1000000000 *
                                compositor->frames /
                                compositor->fps_numerator *
                                compositor->fps_denominator /
                                1000000);
  compositor->frames++;
}

void krad_compositor_process (krad_compositor_t *compositor) {
  krad_compositor_tick (compositor);
  
  if (compositor->had_a_subunit == 1) {
    krad_compositor_alloc_framepool (compositor);
    compositor->had_a_subunit = 2;
  }
  
  if (compositor->had_a_subunit == 2) {
    krad_compositor_prepare (compositor);
    krad_compositor_composite (compositor);
    krad_compositor_output (compositor);
    krad_compositor_finish (compositor);
  }
}

void krad_compositor_port_set_comp_params (krad_compositor_port_t *port,
                                           int x, int y,
                                           int width, int height, 
                                           int crop_x, int crop_y,
                                           int crop_width, int crop_height,
                                           float opacity, float rotation) {

  printk ("comp params func called");

  if ((x != port->subunit.x) || (y != port->subunit.y)) {
    krad_compositor_subunit_set_xy (&port->subunit, x, y, 0);
  }

  port->subunit.width = width;
  port->subunit.height = height;

  port->crop_x = crop_x;
  port->crop_y = crop_y;

  port->crop_width = crop_width;
  port->crop_height = crop_height;

  if (opacity != port->subunit.opacity) {
    krad_compositor_subunit_set_opacity (&port->subunit, opacity, 0);
  }

  if (rotation != port->subunit.rotation) {
    krad_compositor_subunit_set_rotation (&port->subunit, rotation, 0);
  }

  port->comp_params_updated = 1;
}

void krad_compositor_port_set_source_size (krad_compositor_port_t *port,
                                           int width, int height) {

  int x;
  int y;

  x = 0;
  y = 0;

  printk ("comp set_source_size func called %p %dx%d",
          port, width, height);

  port->source_width = width;
  port->source_height = height;

  krad_compositor_aspect_scale (port->source_width, port->source_height,
                                port->krad_compositor->width,
                                port->krad_compositor->height,
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

krad_frame_t *krad_compositor_port_pull_yuv_frame (krad_compositor_port_t *port,
                           uint8_t *yuv_pixels[4], int yuv_strides[4], int color_depth) {

  krad_frame_t *krad_frame;  
  
  if (krad_ringbuffer_read_space (port->frame_ring) >= sizeof(krad_frame_t *)) {

    krad_ringbuffer_read (port->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));

    int rgb_stride_arr[3] = {4*port->krad_compositor->width, 0, 0};
    unsigned char *src[4];
    
    if (port->yuv_color_depth != color_depth) {
      port->yuv_color_depth = color_depth;
    }
    
    port->sws_converter = sws_getCachedContext ( port->sws_converter,
                                                 port->krad_compositor->width,
                                                 port->krad_compositor->height,
                                                 PIX_FMT_RGB32,
                                                 port->subunit.width,
                                                 port->subunit.height,
                                                 port->yuv_color_depth, 
                                                 port->sws_algorithm,
                                                 NULL, NULL, NULL);

    if (port->sws_converter == NULL) {
      failfast ("Krad Compositor: could not sws_getCachedContext");
    }

    src[0] = (unsigned char *)krad_frame->pixels;

    sws_scale (port->sws_converter, (const uint8_t * const*)src,
           rgb_stride_arr, 0, port->krad_compositor->height, yuv_pixels, yuv_strides);
      return krad_frame;
  }

  return NULL;
}

void inport_push_with_perspective (krad_compositor_port_t *port,
                                   krad_frame_t *inframe) {

  krad_frame_t *frame;

  frame = krad_framepool_getframe (port->krad_compositor->framepool);

  frame->width = inframe->width;
  frame->height = inframe->height;
  frame->timecode = inframe->timecode;
  frame->format = inframe->format;

  //printk("the answer here is %u - %u\n", frame->width, frame->height);

  kr_perspective_argb (port->perspective,
                      (uint8_t *)frame->pixels,
                      (uint8_t *)inframe->pixels);
  krad_compositor_port_push_frame (port, frame);
  krad_framepool_unref_frame (frame);
}

void krad_compositor_port_push_yuv_frame (krad_compositor_port_t *port, krad_frame_t *krad_frame) {

  int dststride;
  
  if (port->krad_compositor->width > port->subunit.width) {
    dststride = port->krad_compositor->width;
  } else {
    dststride = port->subunit.width;
  }

  int rgb_stride_arr[3] = {4*dststride, 0, 0};
  unsigned char *dst[4];
  
  krad_compositor_port_tick (port);
  
  if (port->yuv_color_depth != krad_frame->format) {
    port->yuv_color_depth = krad_frame->format;
  }  
  
  port->sws_converter = sws_getCachedContext ( port->sws_converter,
                                               port->source_width,
                                               port->source_height,
                                               port->yuv_color_depth,
                                               port->subunit.width,
                                               port->subunit.height,
                                               PIX_FMT_RGB32, 
                                               port->sws_algorithm,
                                               NULL, NULL, NULL);

  if (port->sws_converter == NULL) {
    failfast ("Krad Compositor: could not sws_getCachedContext");
  }

  /*    
  printk ("compositor port scaling now: %dx%d [%dx%d]-> %dx%d",
          port->source_width,
          port->source_height,
          port->crop_width,
          port->crop_height,        
          port->subunit.width,
          port->subunit.height);         
  */
  dst[0] = (unsigned char *)krad_frame->pixels;
  sws_scale (port->sws_converter,
        (const uint8_t * const*)krad_frame->yuv_pixels,
         krad_frame->yuv_strides, 0, port->source_height, dst, rgb_stride_arr);
         
         
  krad_frame->width = port->subunit.width;
  krad_frame->height = port->subunit.height;

  if (port->perspective != NULL) {
    inport_push_with_perspective (port, krad_frame);
  } else {
    krad_compositor_port_push_frame (port, krad_frame);
  }
}

void krad_compositor_port_push_rgba_frame (krad_compositor_port_t *port, krad_frame_t *krad_frame) {


  int output_rgb_stride_arr[4] = {4*port->krad_compositor->width, 0, 0, 0};
  int input_rgb_stride_arr[4] = {4*port->source_width, 0, 0, 0};  
  unsigned char *dst[4];
  unsigned char *src[4];  
  krad_frame_t *scaled_frame;  
  
  krad_compositor_port_tick (port);
  
  krad_frame->format = PIX_FMT_RGB32;
  
  if ((port->source_width != port->subunit.width) ||
      (port->source_height != port->subunit.height)) {

  port->sws_converter = sws_getCachedContext ( port->sws_converter,
                                               port->source_width,
                                               port->source_height,
                                               krad_frame->format,
                                               port->subunit.width,
                                               port->subunit.height,
                                               PIX_FMT_RGB32, 
                                               port->sws_algorithm,
                                               NULL, NULL, NULL);
        
    if (port->sws_converter == NULL) {
      failfast ("Krad Compositor: could not sws_getCachedContext");
    }
      
    /*      
    printk ("set scaling to w %d h %d sw %d sh %d",
        port->subunit.width,
        port->subunit.height,
        port->source_width,
        port->source_height);               
    */

    while (krad_ringbuffer_read_space (port->frame_ring) >= (sizeof(krad_frame_t *) * 30)) {
      usleep (18000);
      //kludge to not buffer more than 1 handfull? of frames ahead for fast sources
    }
    
    scaled_frame = krad_framepool_getframe (port->krad_compositor->framepool);           

    if (scaled_frame == NULL) {
      failfast ("Krad Compositor: out of frames");
    }

    src[0] = (unsigned char *)krad_frame->pixels;
    dst[0] = (unsigned char *)scaled_frame->pixels;

    sws_scale (port->sws_converter, (const uint8_t * const*)src,
           input_rgb_stride_arr, 0, port->source_height, dst, output_rgb_stride_arr);

    krad_frame->width = port->subunit.width;
    krad_frame->height = port->subunit.height;

    if (port->perspective != NULL) {
      inport_push_with_perspective (port, scaled_frame);
    } else {
      krad_compositor_port_push_frame (port, scaled_frame);
    }
    krad_framepool_unref_frame (scaled_frame);
  } else {
  
    krad_frame->width = port->subunit.width;
    krad_frame->height = port->subunit.height;
  
    if (port->perspective != NULL) {
      inport_push_with_perspective (port, krad_frame);
    } else {
      krad_compositor_port_push_frame (port, krad_frame);
    }
  }
}

void krad_compositor_port_push_frame (krad_compositor_port_t *port, krad_frame_t *frame) {

  int ret;
  char buf[8];
  struct pollfd pollfds[1];
  buf[0] = 0;
  ret = 0;

  if (port->local != 1) {
    krad_framepool_ref_frame (frame);
    krad_ringbuffer_write (port->frame_ring,
                           (char *)&frame,
                            sizeof(krad_frame_t *));

    if (port->direction == OUTPUT) {
      ret = write (port->socketpair[0], "0", 1);
      if (ret != 1) {
        printk ("Compositor: port_push_frame unexpected write return value %d",
                ret);
      }
    }
  } else {

    memcpy (port->local_frame->pixels,
            frame->pixels, 
            port->krad_compositor->width * port->krad_compositor->height * 4);

    pollfds[0].events = POLLOUT;
    pollfds[0].fd = port->msg_sd;
    
    ret = poll (pollfds, 1, 3);

    if (ret < 0) {
      printke ("krad compositor poll failure %d", ret);
      ret = -1;
    }
    
    if (ret == 0) {
      printke ("krad compositor : videoport poll write timeout", ret);
      ret = -1;
    }

    if (ret == 1) {
      if (pollfds[0].revents & POLLHUP) {
        printke ("krad compositor: videoport poll hangup", ret);
        ret = -4;
      }
      if (pollfds[0].revents & POLLERR) {
        printke ("krad compositor: videoport poll error", ret);
        ret = -5;
      }  
      if (pollfds[0].revents & POLLOUT) {
        ret = write (port->msg_sd, buf, 1);
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
    
    ret = poll (pollfds, 1, 0);

    if (ret < 0) {
      printke ("krad compositor poll failure %d", ret);
      //return -1;
    }
    
    if (ret == 0) {
      printke ("krad compositor : videoport poll readr timeout", ret);
      //return -1;
    }

    if (ret == 1) {
      if (pollfds[0].revents & POLLHUP) {
        printke ("krad compositor: videoport poll hangup", ret);
        ret = -7;
      }
      if (pollfds[0].revents & POLLERR) {
        printke ("krad compositor: videoport poll error", ret);
        ret = -6;
      }  
      if (pollfds[0].revents & POLLIN) {
        ret = read (port->msg_sd, buf, 8);
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
    krad_compositor_port_destroy (port->krad_compositor, port);
  }
}

static int krad_compositor_local_videoport_notify (krad_compositor_port_t *port) {

  int ret;
  int wrote;
  char buf[1];
  struct pollfd pollfds[1];
  
  ret = 0;
  wrote = 0;
  buf[0] = 0;

  krad_compositor_port_tick (port);

  if (port->localframe_state == 1) {
    return 0;
  }

  port->local_frame->width = port->source_width;
  port->local_frame->height = port->source_height;

  cairo_surface_flush (port->local_frame->cst);

  pollfds[0].events = POLLOUT;
  pollfds[0].fd = port->msg_sd;
  
  ret = poll (pollfds, 1, 0);

  if (ret < 0) {
    printke ("krad compositor poll failure %d", ret);
    return -2;
  }
  
  if (ret == 0) {
    printke ("krad compositor : videoport poll write2 timeout", ret);
    return -1;
  }

  if (ret == 1) {
    if (pollfds[0].revents & POLLHUP) {
      printke ("krad compositor: videoport poll hangup", ret);
      return -2;
    }
    if (pollfds[0].revents & POLLERR) {
      printke ("krad compositor: videoport poll error", ret);
      return -2;
    }  
    if (pollfds[0].revents & POLLOUT) {
      wrote = write (port->msg_sd, buf, 1);
      if (wrote == 1) {
        port->localframe_state = 1;
      }
      return wrote;
    }
  }
  
  return ret;
}

krad_frame_t *krad_compositor_port_pull_frame_local (krad_compositor_port_t *port) {

  int ret;
  char buf[1];
  struct pollfd pollfds[1];
  
  ret = 0;
  buf[0] = 0;

  pollfds[0].events = POLLIN;
  pollfds[0].fd = port->msg_sd;
  
  ret = poll (pollfds, 1, 3);

  if (ret < 0) {
    printke ("krad compositor poll failure %d", ret);
    return NULL;
  }
  
  if (ret == 0) {
    printke ("krad compositor : videoport poll read timeout", ret);
    return NULL;
  }

  if (ret == 1) {
    if (pollfds[0].revents & POLLHUP) {
      printke ("krad compositor: videoport poll hangup", ret);
      return NULL;
    }
    if (pollfds[0].revents & POLLERR) {
      printke ("krad compositor: videoport poll error", ret);
      return NULL;
    }  
    if (pollfds[0].revents & POLLIN) {
      ret = read (port->msg_sd, buf, 1);
      if (ret == 1) {
        port->localframe_state = 0;
        cairo_surface_mark_dirty (port->local_frame->cst);
        return port->local_frame;
      }
    }
  }

  return NULL;
}

krad_frame_t *krad_compositor_port_pull_frame (krad_compositor_port_t *port) {

  krad_frame_t *krad_frame;
  
  if (port->local == 1) {
    return krad_compositor_port_pull_frame_local (port);
  }
  
  if (port->direction == INPUT) {

    if (port->last_frame != NULL) {

      if ((port->start_timecode + port->last_frame->timecode) < 
        port->krad_compositor->timecode) {

        if (krad_ringbuffer_read_space (port->frame_ring) >= sizeof(krad_frame_t *)) {
          krad_ringbuffer_read (port->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));

          krad_framepool_unref_frame (port->last_frame);  
          krad_framepool_ref_frame (krad_frame);
          port->last_frame = krad_frame;
    
          return krad_frame;
        } else {
          krad_framepool_ref_frame (port->last_frame);
          return port->last_frame;
        }
      } else {
        krad_framepool_ref_frame (port->last_frame);
        return port->last_frame;
      }
    } else {
      if (krad_ringbuffer_read_space (port->frame_ring) >= sizeof(krad_frame_t *)) {
        krad_ringbuffer_read (port->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));
  
        port->start_timecode = port->krad_compositor->timecode;
  
        krad_framepool_ref_frame (krad_frame);
        port->last_frame = krad_frame;
  
        return krad_frame;
      }
    }
  } else {
    if (krad_ringbuffer_read_space (port->frame_ring) >= sizeof(krad_frame_t *)) {
      krad_ringbuffer_read (port->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));      
      return krad_frame;
    }
  }

  return NULL;
}

int krad_compositor_port_frames_avail (krad_compositor_port_t *port) {
  
  int frames;
  
  frames = krad_ringbuffer_read_space (port->frame_ring);
  frames = frames / sizeof (krad_frame_t *);

  return frames;
}

int krad_compositor_port_get_fd (krad_compositor_port_t *port) {
  return port->socketpair[1];
}

krad_compositor_port_t *krad_compositor_port_create (krad_compositor_t *krad_compositor, char *sysname, int direction,
                                                     int width, int height) {
              
  return krad_compositor_port_create_full (krad_compositor, sysname, direction, width, height, 0, 0);
}

krad_compositor_port_t *krad_compositor_port_create_full (krad_compositor_t *krad_compositor, char *sysname, int direction,
                           int width, int height, int holdlock, int local) {

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
  
  if (port->direction == INPUT) {
    port->source_width = width;
    port->source_height = height;

    port->view.top_left.x = 0;
    port->view.top_left.y = 0;
    port->view.top_right.x = port->source_width - 1;
    port->view.top_right.y = 0;
    port->view.bottom_left.x = 0;
    port->view.bottom_left.y = port->source_width - 1;
    port->view.bottom_right.x = port->source_width - 1;
    port->view.bottom_right.y = port->source_height - 1;

    krad_compositor_aspect_scale (port->source_width, port->source_height,
                                  krad_compositor->width, krad_compositor->height,
                                  &port->subunit.width, &port->subunit.height);
                    
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
      //OUTPUT
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
  if (port->direction == INPUT) {
    krad_compositor->active_input_ports++;
  }
  if (port->direction == OUTPUT) {
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

krad_compositor_port_t *krad_compositor_local_port_create (krad_compositor_t *krad_compositor,
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
  
  port->local_buffer = mmap (NULL, port->local_buffer_size,
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

void krad_compositor_port_destroy (krad_compositor_t *krad_compositor, krad_compositor_port_t *port) {
  port->subunit.active = 2;
}

static void krad_compositor_port_destroy_actual (krad_compositor_t *krad_compositor, krad_compositor_port_t *port) {

  if (port->direction == INPUT) {
    krad_compositor->active_input_ports--;
    if (port->perspective != NULL) {
      kr_perspective_destroy (&port->perspective);
    }
  }
  if (port->direction == OUTPUT) {
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

static void krad_compositor_free_framepool (krad_compositor_t *compositor) {
  if (compositor->framepool != NULL) {
    krad_framepool_destroy ( &compositor->framepool );
    printk ("Krad Compositor: Freed Framepool");
  }
}

static void krad_compositor_alloc_framepool (krad_compositor_t *compositor) {

  if (compositor->framepool == NULL) {
    printk ("Krad Compositor: Allocing Resources");
    compositor->framepool = krad_framepool_create ( compositor->width,
                                                    compositor->height,
                                                    DEFAULT_COMPOSITOR_BUFFER_FRAMES);
  }
}

static void krad_compositor_aspect_scale (int width, int height,
                                          int avail_width, int avail_height,
                                          int *new_width, int *new_heigth) {
  
  double scale_x, scale_y, scale;

  scale_x = (float)avail_width  / width;
  scale_y = (float)avail_height / height;
  scale = MIN ( scale_x, scale_y );
  
  *new_width = width * scale;
  *new_heigth = height * scale;
  
  printk ("Source: %d x %d Max: %d x %d Aspect Constrained: %d x %d",
          width, height,
          avail_width, avail_height,
          *new_width, *new_heigth);
}

void krad_compositor_ticker_thread_cleanup (void *arg) {

  krad_compositor_t *krad_compositor = (krad_compositor_t *)arg;
  
  if (krad_compositor->krad_ticker != NULL) {
    krad_ticker_destroy (krad_compositor->krad_ticker);
    krad_compositor->krad_ticker = NULL;
    printk ("Krad Compositor: Synthetic Timer Destroyed");
  }
}

void *krad_compositor_ticker_thread (void *arg) {

  krad_compositor_t *krad_compositor = (krad_compositor_t *)arg;

  krad_system_set_thread_name ("kr_compositor");
  pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, NULL);
  krad_compositor->krad_ticker = krad_ticker_create (krad_compositor->fps_numerator,
                             krad_compositor->fps_denominator);
  pthread_cleanup_push (krad_compositor_ticker_thread_cleanup, krad_compositor);
  pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);
  krad_ticker_start_at (krad_compositor->krad_ticker, krad_compositor->start_time);

  while (krad_compositor->ticker_running == 1) {
    pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, NULL);
    krad_compositor_process (krad_compositor);
    pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);
    krad_ticker_wait (krad_compositor->krad_ticker);
  }
  
  pthread_cleanup_pop (1);
  
  return NULL;
}

void krad_compositor_start_ticker (krad_compositor_t *krad_compositor) {

  if (krad_compositor->ticker_running == 1) {
    krad_compositor_stop_ticker (krad_compositor);
  }
  clock_gettime (CLOCK_MONOTONIC, &krad_compositor->start_time);
  krad_compositor->ticker_running = 1;
  pthread_create (&krad_compositor->ticker_thread, NULL, krad_compositor_ticker_thread, (void *)krad_compositor);
}

void krad_compositor_start_ticker_at (krad_compositor_t *krad_compositor, struct timespec start_time) {

  if (krad_compositor->ticker_running == 1) {
    krad_compositor_stop_ticker (krad_compositor);
  }
  memcpy (&krad_compositor->start_time, &start_time, sizeof(struct timespec));
  krad_compositor->ticker_running = 1;
  pthread_create (&krad_compositor->ticker_thread, NULL, krad_compositor_ticker_thread, (void *)krad_compositor);
}

void krad_compositor_stop_ticker (krad_compositor_t *krad_compositor) {

  if (krad_compositor->ticker_running == 1) {
    krad_compositor->ticker_running = 2;
    pthread_cancel (krad_compositor->ticker_thread);
    pthread_join (krad_compositor->ticker_thread, NULL);
    krad_compositor->ticker_running = 0;
  }
}

void krad_compositor_get_frame_rate (krad_compositor_t *krad_compositor,
                                     int *fps_numerator, int *fps_denominator) {

  *fps_numerator = krad_compositor->fps_numerator;
  *fps_denominator = krad_compositor->fps_denominator;
}

void krad_compositor_get_resolution (krad_compositor_t *krad_compositor, int *width, int *height) {

  *width = krad_compositor->width;
  *height = krad_compositor->height;
}

void krad_compositor_subunit_update (krad_compositor_t *compositor, kr_unit_control_t *uc) {


  if ((compositor == NULL) || (uc == NULL) || (uc->address.id.number >= KC_MAX_ANY)) {
    return;
  }

  if (uc->address.path.unit == KR_COMPOSITOR) {
    switch ( uc->address.path.subunit.compositor_subunit ) {
      case KR_SPRITE:
        if (uc->address.id.number < KC_MAX_SPRITES) {
          if (compositor->sprite[uc->address.id.number].subunit.active == 1) {
            switch (uc->address.control.compositor_control) {
              case KR_NO:
                break;
              case KR_X:
                krad_compositor_subunit_set_x (&compositor->sprite[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);                
                break;
              case KR_Y:
                krad_compositor_subunit_set_y (&compositor->sprite[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);                
                break;
              case KR_Z:
                krad_compositor_subunit_set_z (&compositor->sprite[uc->address.id.number].subunit,
                                               uc->value.integer);                
                break;
              case KR_WIDTH:
                krad_compositor_subunit_set_width (&compositor->sprite[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);                
                break;
              case KR_HEIGHT:
                krad_compositor_subunit_set_height (&compositor->sprite[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);                
                break;
              case KR_ROTATION:
                krad_compositor_subunit_set_rotation (&compositor->sprite[uc->address.id.number].subunit,
                                                      uc->value.real, uc->duration);
                break;
              case KR_OPACITY:
                krad_compositor_subunit_set_opacity (&compositor->sprite[uc->address.id.number].subunit,
                                                     uc->value.real, uc->duration);
                break;
              case KR_XSCALE:
                krad_compositor_subunit_set_xscale (&compositor->sprite[uc->address.id.number].subunit,
                                                    uc->value.real, uc->duration);
                break;
              case KR_YSCALE:
                krad_compositor_subunit_set_yscale (&compositor->sprite[uc->address.id.number].subunit,
                                                    uc->value.real, uc->duration);
                break;
              case KR_RED:
                krad_compositor_subunit_set_red (&compositor->sprite[uc->address.id.number].subunit,
                                                 uc->value.real, uc->duration);
                break;
              case KR_GREEN:
                krad_compositor_subunit_set_green (&compositor->sprite[uc->address.id.number].subunit,
                                                   uc->value.real, uc->duration);
                break;
              case KR_BLUE:
                krad_compositor_subunit_set_blue (&compositor->sprite[uc->address.id.number].subunit,
                                                  uc->value.real, uc->duration);
                break;
              case KR_ALPHA:
                krad_compositor_subunit_set_alpha (&compositor->sprite[uc->address.id.number].subunit,
                                                   uc->value.real, uc->duration);
                break;
              case KR_TICKRATE:
                krad_sprite_set_tickrate (&compositor->sprite[uc->address.id.number],
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
              case KR_XSCALE:
                krad_compositor_subunit_set_xscale (&compositor->text[uc->address.id.number].subunit,
                                                    uc->value.real, uc->duration);
                break;
              case KR_YSCALE:
                krad_compositor_subunit_set_yscale (&compositor->text[uc->address.id.number].subunit,
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
              case KR_XSCALE:
                krad_compositor_subunit_set_xscale (&compositor->vector[uc->address.id.number].subunit,
                                                    uc->value.real, uc->duration);
                break;
              case KR_YSCALE:
                krad_compositor_subunit_set_yscale (&compositor->vector[uc->address.id.number].subunit,
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
        if (uc->address.id.number < KC_MAX_PORTS) {
          if ((compositor->port[uc->address.id.number].subunit.active == 1) &&
              (compositor->port[uc->address.id.number].direction == INPUT)) {
            switch (uc->address.control.compositor_control) {
              case KR_NO:
              case KR_TICKRATE:
                break;
              case KR_X:
                krad_compositor_subunit_set_x (&compositor->port[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);                
                break;
              case KR_Y:
                krad_compositor_subunit_set_y (&compositor->port[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);                
                break;
              case KR_Z:
                krad_compositor_subunit_set_z (&compositor->port[uc->address.id.number].subunit,
                                               uc->value.integer);                
                break;
              case KR_WIDTH:

                //if (uc->value.integer < compositor->port[uc->address.id.number].crop_width) {
                  krad_easing_set_new_value (&compositor->port[uc->address.id.number].crop_width_easing,
                                             uc->value.integer, uc->duration, EASEINOUTSINE, NULL);
                //}
              
                krad_compositor_subunit_set_width (&compositor->port[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);                
                break;
              case KR_HEIGHT:

                //if (uc->value.integer < compositor->port[uc->address.id.number].crop_height) {
                  krad_easing_set_new_value (&compositor->port[uc->address.id.number].crop_height_easing,
                                             uc->value.integer, uc->duration, EASEINOUTSINE, NULL);
                //}

                krad_compositor_subunit_set_height (&compositor->port[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);                
                break;
              case KR_ROTATION:
                krad_compositor_subunit_set_rotation (&compositor->port[uc->address.id.number].subunit,
                                                      uc->value.real, uc->duration);
                break;
              case KR_OPACITY:
                krad_compositor_subunit_set_opacity (&compositor->port[uc->address.id.number].subunit,
                                                     uc->value.real, uc->duration);
                break;
              case KR_XSCALE:
                krad_compositor_subunit_set_xscale (&compositor->port[uc->address.id.number].subunit,
                                                    uc->value.real, uc->duration);
                break;
              case KR_YSCALE:
                krad_compositor_subunit_set_yscale (&compositor->port[uc->address.id.number].subunit,
                                                    uc->value.real, uc->duration);
                break;
              case KR_RED:
                krad_compositor_subunit_set_red (&compositor->port[uc->address.id.number].subunit,
                                                 uc->value.real, uc->duration);
                break;
              case KR_GREEN:
                krad_compositor_subunit_set_green (&compositor->port[uc->address.id.number].subunit,
                                                   uc->value.real, uc->duration);
                break;
              case KR_BLUE:
                krad_compositor_subunit_set_blue (&compositor->port[uc->address.id.number].subunit,
                                                  uc->value.real, uc->duration);
                break;
              case KR_ALPHA:
                krad_compositor_subunit_set_alpha (&compositor->port[uc->address.id.number].subunit,
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
}

int krad_compositor_subunit_create (krad_compositor_t *compositor,
                                     kr_compositor_subunit_t type,
                                     char *option,
                                     char *option2) {

  int i;
  
  i = 0;

  printk ("Krad Compositor: Subunit create type: %s and option: %s",
          kr_compositor_subunit_type_to_string (type),
          option);

  if (compositor->had_a_subunit == 0) {
    compositor->had_a_subunit = 1;
  }

  switch ( type ) {
    case KR_SPRITE:
      for (i = 0; i < KC_MAX_SPRITES; i++) {
        if (compositor->sprite[i].subunit.active == 0) {
          if (krad_sprite_open_file (&compositor->sprite[i], option)) {
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
          krad_text_set_text (&compositor->text[i], option, option2);
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
            krad_vector_set_type (&compositor->vector[i], option);
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

int krad_compositor_subunit_destroy (krad_compositor_t *compositor, kr_address_t *address) {

  if ((compositor == NULL) || (address == NULL) || (address->id.number >= KC_MAX_ANY)) {
    return 0;
  }

  if (address->path.unit == KR_COMPOSITOR) {
    switch ( address->path.subunit.compositor_subunit ) {
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
        if (address->id.number < KC_MAX_PORTS) {
          if (compositor->port[address->id.number].subunit.active == 1) {
            compositor->port[address->id.number].subunit.active = 2;
            return 1;
          }
        }
        return 0;
      default:
        return 0;
    }
  }
  return 0;  
}

void krad_compositor_set_resolution (krad_compositor_t *krad_compositor, uint32_t width, uint32_t height) {
  if (krad_compositor->had_a_subunit == 0) {
    krad_compositor->width = width;
    krad_compositor->height = height;
    krad_compositor->frame_byte_size = krad_compositor->width * krad_compositor->height * 4;
  }
}

static void krad_compositor_set_frame_rate (krad_compositor_t *krad_compositor,
                                            int fps_numerator, int fps_denominator) {

  krad_compositor->fps_numerator = fps_numerator;
  krad_compositor->fps_denominator = fps_denominator;  
}

static void krad_compositor_ports_destroy_all (krad_compositor_t *compositor) {

  int i;

  for (i = 0; i < KC_MAX_PORTS; i++) {
    if (compositor->port[i].subunit.active == 1) {
      krad_compositor_port_destroy_actual (compositor, &compositor->port[i]);
    }
  }

  free (compositor->port);
}

static void krad_compositor_ports_create_all (krad_compositor_t *compositor) {

  int i;

  compositor->port = calloc (KC_MAX_PORTS, sizeof(krad_compositor_port_t));
  
  for (i = 0; i < KC_MAX_PORTS; i++) {
    compositor->port[i].subunit.address.path.unit = KR_COMPOSITOR;
    compositor->port[i].subunit.address.path.subunit.compositor_subunit = KR_VIDEOPORT;
    compositor->port[i].subunit.address.id.number = i;
    krad_compositor_subunit_reset (&compositor->port[i].subunit);
  }
}

static void krad_compositor_prepare_subunits (krad_compositor_t *compositor) {

  int i;

  krad_compositor_ports_create_all (compositor);
  compositor->sprite = krad_sprite_create_arr (KC_MAX_SPRITES);
  compositor->text = krad_text_create_arr (&compositor->ft_library, KC_MAX_TEXTS);
  compositor->vector = krad_vector_create_arr (KC_MAX_VECTORS);

  for (i = 0; i < KC_MAX_SUBUNITS; i++) {
    if (i < KC_MAX_PORTS) {
      compositor->subunit[i] = &compositor->port[i].subunit;
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

void krad_compositor_destroy (krad_compositor_t *compositor) {

  printk ("Krad Compositor: Destroy Started");

  krad_compositor_ports_destroy_all (compositor);
  krad_compositor_stop_ticker (compositor);
  krad_compositor_free_framepool (compositor);
  krad_sprite_destroy (compositor->background);
  krad_sprite_destroy_arr (compositor->sprite, KC_MAX_SPRITES);
  krad_text_destroy_arr (compositor->text, KC_MAX_TEXTS);
  krad_vector_destroy_arr (compositor->vector, KC_MAX_VECTORS);
  
  FT_Done_FreeType (compositor->ft_library);
  free (compositor);

  printk ("Krad Compositor: Destroy Complete");
}

krad_compositor_t *
krad_compositor_create (int width, int height, int fps_num, int fps_den) {

  krad_compositor_t *compositor = calloc (1, sizeof(krad_compositor_t));

  printk ("Krad Compositor: Cairo Version: %s", cairo_version_string());
  printk ("Krad Compositor: SWScale Version: %u", swscale_version());

  FT_Init_FreeType (&compositor->ft_library);

  krad_compositor_set_resolution (compositor, width, height);
  krad_compositor_set_frame_rate (compositor, fps_num, fps_den);

  compositor->address.path.unit = KR_COMPOSITOR;
  compositor->address.path.subunit.compositor_subunit = KR_UNIT;
  compositor->background = krad_sprite_create ();

  krad_compositor_prepare_subunits (compositor);
  //krad_compositor_alloc_framepool (compositor);

  return compositor;
}
