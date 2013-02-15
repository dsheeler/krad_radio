#include "krad_compositor.h"

static void krad_compositor_port_destroy_actual (krad_compositor_t *krad_compositor, krad_compositor_port_t *krad_compositor_port);
static void *krad_compositor_snapshot_thread (void *arg);
static void krad_compositor_aspect_scale (int width, int height,
                                          int avail_width, int avail_height,
                                          int *new_width, int *new_heigth);
static void krad_compositor_free_framepool (krad_compositor_t *compositor);
static void krad_compositor_alloc_framepool (krad_compositor_t *compositor);
static void krad_compositor_prepare_subunits (krad_compositor_t *compositor);
static void krad_compositor_deactivate_subunits (krad_compositor_t *compositor);
static void *krad_compositor_ticker_thread (void *arg);
static void krad_compositor_set_resolution (krad_compositor_t *krad_compositor, int width, int height);
static void krad_compositor_set_frame_rate (krad_compositor_t *krad_compositor,
                                            int fps_numerator, int fps_denominator);

#ifdef KRAD_USE_WAYLAND

static void *krad_compositor_display_thread (void *arg);

typedef struct krad_compositor_wayland_display_St krad_compositor_wayland_display_t;

struct krad_compositor_wayland_display_St {
  krad_wayland_t *krad_wayland;
  krad_compositor_port_t *krad_compositor_port;
  void *buffer;
  int w;
  int h;
};

int krad_compositor_wayland_display_render_callback (void *pointer, uint32_t time) {

  krad_compositor_wayland_display_t *krad_compositor_wayland_display = (krad_compositor_wayland_display_t *)pointer;

  int ret;
  char buffer[1];
  int updated;
  krad_frame_t *krad_frame;
  
  updated = 0;

  krad_frame = krad_compositor_port_pull_frame (krad_compositor_wayland_display->krad_compositor_port);

  if (krad_frame != NULL) {
  
    //FIXME do this first etc
    ret = read (krad_compositor_wayland_display->krad_compositor_port->socketpair[1], buffer, 1);
    if (ret != 1) {
      if (ret == 0) {
        printk ("Krad OTransponder: port read got EOF");
        return updated;
      }
      printk ("Krad OTransponder: port read unexpected read return value %d", ret);
    }
  
  
    memcpy (krad_compositor_wayland_display->buffer,
        krad_frame->pixels,
        krad_compositor_wayland_display->w * krad_compositor_wayland_display->h * 4);
    krad_framepool_unref_frame (krad_frame);
    updated = 1;
  } else {
    
  }
  return updated;
}

static void *krad_compositor_display_thread (void *arg) {

  krad_compositor_t *krad_compositor = (krad_compositor_t *)arg;

  krad_system_set_thread_name ("kr_wayland");

  krad_compositor_wayland_display_t *krad_compositor_wayland_display;
  
  krad_compositor_wayland_display = calloc (1, sizeof (krad_compositor_wayland_display_t));

  krad_compositor_wayland_display->krad_wayland = krad_wayland_create ();

  krad_compositor_get_resolution (krad_compositor,
                &krad_compositor_wayland_display->w,
                &krad_compositor_wayland_display->h);
  
  krad_compositor_wayland_display->krad_compositor_port = krad_compositor_port_create (krad_compositor, "WLOut", OUTPUT,
                                             krad_compositor_wayland_display->w,
                                             krad_compositor_wayland_display->h);

  //krad_wayland->render_test_pattern = 1;

  krad_wayland_set_frame_callback (krad_compositor_wayland_display->krad_wayland,
                   krad_compositor_wayland_display_render_callback,
                   krad_compositor_wayland_display);

  krad_wayland_prepare_window (krad_compositor_wayland_display->krad_wayland,
                 krad_compositor_wayland_display->w,
                 krad_compositor_wayland_display->h,
                 &krad_compositor_wayland_display->buffer);

  printk("Wayland display prepared");

  krad_wayland_open_window (krad_compositor_wayland_display->krad_wayland);

  printk("Wayland display running");

  while (krad_compositor->display_open == 1) {
    krad_wayland_iterate (krad_compositor_wayland_display->krad_wayland);
  }

  krad_wayland_close_window (krad_compositor_wayland_display->krad_wayland);

  krad_wayland_destroy (krad_compositor_wayland_display->krad_wayland);
  
  krad_compositor_port_destroy (krad_compositor, krad_compositor_wayland_display->krad_compositor_port);

  krad_compositor->display_open = 0;

  free (krad_compositor_wayland_display);

  return NULL;
}

#endif

void krad_compositor_open_display (krad_compositor_t *krad_compositor) {

#ifdef KRAD_USE_WAYLAND
  if (krad_compositor->display_open == 1) {
    krad_compositor_close_display (krad_compositor);
  }

  krad_compositor->display_open = 1;
  pthread_create (&krad_compositor->display_thread, NULL, krad_compositor_display_thread, (void *)krad_compositor);

#else
  printk("Wayland disabled: not opening a display");
#endif
}

void krad_compositor_close_display (krad_compositor_t *krad_compositor) {

#ifdef KRAD_USE_WAYLAND
  if (krad_compositor->display_open == 1) {
    printk("Wayland display closing");  
    krad_compositor->display_open = 2;
    pthread_join (krad_compositor->display_thread, NULL);
    krad_compositor->display_open = 0;
    printk("Wayland display closed");
  } else {
    printk("Wayland display wasn't open");
  }
#else
  printk("Wayland disabled: no need to close display");
#endif
}

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

int krad_compositor_get_background_name (krad_compositor_t *krad_compositor, char *filename) {
  
  if ((filename == NULL) ||
      (krad_compositor->background->subunit.active != 1)) {
    return 0;
  }
  
  strncpy (filename, krad_compositor->background->filename, sizeof(krad_compositor->background->filename));
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
  if ((compositor->frames % 30) > 15) {
    cairo_save (compositor->cr);
    cairo_set_source_rgb (compositor->cr, RED);
    cairo_select_font_face (compositor->cr,
                            "monospace",
                            CAIRO_FONT_SLANT_NORMAL,
                            CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (compositor->cr, 88.0);
    cairo_move_to (compositor->cr, compositor->width/4.0, compositor->height/2);
    cairo_show_text (compositor->cr, "KR: NO INPUT");
    cairo_restore (compositor->cr);
  }
}

void krad_compositor_clear_frame (krad_compositor_t *compositor) {
  cairo_save (compositor->cr);
  cairo_set_source_rgba (compositor->cr, BGCOLOR_CLR);
  cairo_set_operator (compositor->cr, CAIRO_OPERATOR_SOURCE);  
  cairo_paint (compositor->cr);
  cairo_restore (compositor->cr);
}

void krad_compositor_videoport_render (krad_compositor_port_t *port,
                                       cairo_t *cr) {

  krad_frame_t *frame;
  
  frame = NULL;

  frame = krad_compositor_port_pull_frame (port);    

  if (frame != NULL) {

    if (port->subunit.opacity > 0.0f) {

      cairo_save (cr);

      if (port->subunit.rotation != 0.0f) {
        cairo_translate (cr, port->crop_width / 2, port->crop_height / 2);
        cairo_rotate (cr, port->subunit.rotation * (M_PI/180.0));
        cairo_translate (cr, port->crop_width / -2, port->crop_height / -2);
      }

      cairo_set_source_surface (cr, frame->cst,
                                port->subunit.x - port->crop_x,
                                port->subunit.y - port->crop_y);

      cairo_rectangle (cr,
                       port->subunit.x, port->subunit.y,
                       port->crop_width, port->crop_height);

      cairo_clip (cr);

      if (port->subunit.opacity == 1.0f) {
        cairo_paint (cr);
      } else {
        cairo_paint_with_alpha (cr, port->subunit.opacity);
      }

      cairo_restore (cr);
    }
    
    krad_compositor_subunit_tick (&port->subunit);
  
    krad_framepool_unref_frame (frame);
    frame = NULL;
  }
}

static void krad_compositor_prepare (krad_compositor_t *compositor) {

  while (compositor->frame == NULL) {
    compositor->frame = krad_framepool_getframe (compositor->framepool);
    if (compositor->frame == NULL) {
      printke ("This is very bad! Compositor wanted a frame but could not get one right away!");
      usleep (5000);
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
      (compositor->active_vectors == 0)) {
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

  for (i = 0; i < KC_MAX_TEXTS; i++) {
    if (compositor->text[i].subunit.active == 1) {
      krad_text_render (&compositor->text[i], compositor->cr);
    }
  }
  
  for (i = 0; i < KC_MAX_VECTORS; i++) {
    if (compositor->vector[i].subunit.active == 1) {
      krad_vector_render (&compositor->vector[i], compositor->cr);
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

  if (compositor->snapshot > 0) {
    krad_compositor_take_snapshot (compositor, compositor->frame, SNAPPNG);
  }
  if (compositor->snapshot_jpeg > 0) {
    krad_compositor_take_snapshot (compositor, compositor->frame, SNAPJPEG);
  }  

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

void krad_compositor_get_last_snapshot_name (krad_compositor_t *krad_compositor, char *filename) {
  if (filename == NULL) {
    return;
  }
  strcpy (filename, krad_compositor->last_snapshot_name);
}

void krad_compositor_set_last_snapshot_name (krad_compositor_t *krad_compositor, char *filename) {
  strcpy (krad_compositor->last_snapshot_name, filename);
}

static void *krad_compositor_snapshot_thread (void *arg) {

  krad_compositor_snapshot_t *krad_compositor_snapshot = (krad_compositor_snapshot_t *)arg;
  
  tjhandle jpeg;
  int jpeg_fd;  
  unsigned char *jpeg_buf;
  long unsigned int jpeg_size;
  int ret;

  jpeg_buf = NULL;

  if (krad_compositor_snapshot->jpeg == 1) {

    jpeg = tjInitCompress ();

    ret = tjCompress2 (jpeg,
               (unsigned char *)krad_compositor_snapshot->krad_frame->pixels,
               krad_compositor_snapshot->width,
               krad_compositor_snapshot->width * 4,
               krad_compositor_snapshot->height,
               TJPF_BGRA,
               &jpeg_buf,
               &jpeg_size,
               TJSAMP_444,
               90,
               0);
    if (ret == 0) {
      jpeg_fd = open (krad_compositor_snapshot->filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      if (jpeg_fd > 0) {
        while (ret != jpeg_size) {
          // FIXME need ability to fail here
          ret += write (jpeg_fd, jpeg_buf + ret, jpeg_size - ret);
        }
        close (jpeg_fd);
      }
      tjFree (jpeg_buf);
    } else {
      printke("Krad Compositor: JPEG Snapshot error: %s", tjGetErrorStr());
    }
    tjDestroy ( jpeg );
  } else {
    //FIXME need to monitor for fail
    cairo_surface_write_to_png (krad_compositor_snapshot->krad_frame->cst, krad_compositor_snapshot->filename);
  }
  krad_framepool_unref_frame (krad_compositor_snapshot->krad_frame);
  krad_compositor_set_last_snapshot_name (krad_compositor_snapshot->krad_compositor, krad_compositor_snapshot->filename);
  free (krad_compositor_snapshot);

  return NULL;
}

void krad_compositor_take_snapshot (krad_compositor_t *krad_compositor, krad_frame_t *krad_frame, krad_snapshot_fmt_t format) {
    
  krad_compositor_snapshot_t *krad_compositor_snapshot;
  char *ext;

  if (krad_compositor->dir == NULL) {  
    return;
  }

  krad_framepool_ref_frame (krad_frame);
    
  krad_compositor_snapshot = calloc (1, sizeof (krad_compositor_snapshot_t));

  if (format == SNAPPNG) {
    krad_compositor->snapshot--;
    ext = "png";
  } else {
    krad_compositor->snapshot_jpeg--;
    krad_compositor_snapshot->jpeg = 1;
    ext = "jpg";
  }
  
  krad_compositor_snapshot->krad_frame = krad_frame;
  krad_compositor_snapshot->krad_compositor = krad_compositor;
  krad_compositor_snapshot->width = krad_compositor->width;
  krad_compositor_snapshot->height = krad_compositor->height;  
  
  sprintf (krad_compositor_snapshot->filename,
           "%s/snapshot_%"PRIu64"_%"PRIu64".%s",
           krad_compositor->dir,
           ktime(),
           krad_compositor->frames,
           ext);
  
  pthread_create (&krad_compositor->snapshot_thread,
                  NULL,
                  krad_compositor_snapshot_thread,
                  (void *)krad_compositor_snapshot);

  pthread_detach (krad_compositor->snapshot_thread);
}

void krad_compositor_port_set_comp_params (krad_compositor_port_t *krad_compositor_port,
                                           int x, int y, int width, int height, 
                                           int crop_x, int crop_y,
                                           int crop_width, int crop_height, float opacity, float rotation) {
                       
  printkd ("comp compy params func called");
 
  if ((x != krad_compositor_port->subunit.x) || (y != krad_compositor_port->subunit.y)) {
    krad_compositor_subunit_set_xy (&krad_compositor_port->subunit, x, y, 0);
  }

  krad_compositor_port->subunit.width = width;
  krad_compositor_port->subunit.height = height;

  krad_compositor_port->crop_x = crop_x;
  krad_compositor_port->crop_y = crop_y;

  krad_compositor_port->crop_width = crop_width;
  krad_compositor_port->crop_height = crop_height;

  if (opacity != krad_compositor_port->subunit.opacity) {
    krad_compositor_subunit_set_opacity (&krad_compositor_port->subunit, opacity, 0);
  }

  if (rotation != krad_compositor_port->subunit.rotation) {
    krad_compositor_subunit_set_rotation (&krad_compositor_port->subunit, rotation, 0);
  }  

  krad_compositor_port->comp_params_updated = 1;
}

void krad_compositor_port_set_io_params (krad_compositor_port_t *krad_compositor_port,
                                         int width, int height) {
  
  int x;
  int y;
    
  x = 0;
  y = 0;
  
  printkd ("comp io params func called");
                     
  krad_compositor_port->source_width = width;
  krad_compositor_port->source_height = height;

  krad_compositor_aspect_scale (krad_compositor_port->source_width, krad_compositor_port->source_height,
                  krad_compositor_port->krad_compositor->width, krad_compositor_port->krad_compositor->height,
                  &krad_compositor_port->subunit.width, &krad_compositor_port->subunit.height);

  krad_compositor_port->crop_width = krad_compositor_port->subunit.width;
  krad_compositor_port->crop_height = krad_compositor_port->subunit.height;

  if (krad_compositor_port->subunit.width < krad_compositor_port->krad_compositor->width) {
    x = (krad_compositor_port->krad_compositor->width - krad_compositor_port->subunit.width) / 2;
  }
  
  if (krad_compositor_port->subunit.height < krad_compositor_port->krad_compositor->height) {
    y = (krad_compositor_port->krad_compositor->height - krad_compositor_port->subunit.height) / 2;
  }

  krad_compositor_subunit_set_xy (&krad_compositor_port->subunit, x, y, 0);
  krad_compositor_port->io_params_updated = 1;
}

void krad_compositor_port_push_yuv_frame (krad_compositor_port_t *krad_compositor_port, krad_frame_t *krad_frame) {

  int dststride;
  
  if (krad_compositor_port->krad_compositor->width > krad_compositor_port->subunit.width) {
    dststride = krad_compositor_port->krad_compositor->width;
  } else {
    dststride = krad_compositor_port->subunit.width;
  }

  int rgb_stride_arr[3] = {4*dststride, 0, 0};
  unsigned char *dst[4];
  
  if ((krad_compositor_port->io_params_updated) || (krad_compositor_port->comp_params_updated)) {
    if (krad_compositor_port->sws_converter != NULL) {
      sws_freeContext ( krad_compositor_port->sws_converter );
      krad_compositor_port->sws_converter = NULL;
    }
    krad_compositor_port->io_params_updated = 0;
    krad_compositor_port->comp_params_updated = 0;
    printkd ("I knew about the update");
  }
  
  if (krad_compositor_port->yuv_color_depth != krad_frame->format) {
    if (krad_compositor_port->sws_converter != NULL) {
      sws_freeContext ( krad_compositor_port->sws_converter );
      krad_compositor_port->sws_converter = NULL;
    }
    krad_compositor_port->yuv_color_depth = krad_frame->format;
  }  
  
  if (krad_compositor_port->sws_converter == NULL) {

    krad_compositor_port->sws_converter =
      sws_getContext ( krad_compositor_port->source_width,
               krad_compositor_port->source_height,
               krad_compositor_port->yuv_color_depth,
               krad_compositor_port->subunit.width,
               krad_compositor_port->subunit.height,
               PIX_FMT_RGB32, 
               SWS_BICUBIC,
               NULL, NULL, NULL);
      
    printk ("compositor port scaling now: %dx%d [%dx%d]-> %dx%d",
        krad_compositor_port->source_width,
        krad_compositor_port->source_height,
        krad_compositor_port->crop_width,
        krad_compositor_port->crop_height,        
        krad_compositor_port->subunit.width,
        krad_compositor_port->subunit.height);         
  }                   

  dst[0] = (unsigned char *)krad_frame->pixels;
  sws_scale (krad_compositor_port->sws_converter,
        (const uint8_t * const*)krad_frame->yuv_pixels,
         krad_frame->yuv_strides, 0, krad_compositor_port->source_height, dst, rgb_stride_arr);
  krad_compositor_port_push_frame (krad_compositor_port, krad_frame);
}

krad_frame_t *krad_compositor_port_pull_yuv_frame (krad_compositor_port_t *krad_compositor_port,
                           uint8_t *yuv_pixels[4], int yuv_strides[4], int color_depth) {

  krad_frame_t *krad_frame;  
  
  if (krad_ringbuffer_read_space (krad_compositor_port->frame_ring) >= sizeof(krad_frame_t *)) {

    krad_ringbuffer_read (krad_compositor_port->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));

    int rgb_stride_arr[3] = {4*krad_compositor_port->krad_compositor->width, 0, 0};
    unsigned char *src[4];
    
    if (krad_compositor_port->io_params_updated) {
      if (krad_compositor_port->sws_converter != NULL) {
        sws_freeContext ( krad_compositor_port->sws_converter );
        krad_compositor_port->sws_converter = NULL;
      }
      krad_compositor_port->io_params_updated = 0;
    }
    
    if (krad_compositor_port->yuv_color_depth != color_depth) {
      if (krad_compositor_port->sws_converter != NULL) {
        sws_freeContext ( krad_compositor_port->sws_converter );
        krad_compositor_port->sws_converter = NULL;
      }
      krad_compositor_port->yuv_color_depth = color_depth;
    }
    
    if (krad_compositor_port->sws_converter == NULL) {

      krad_compositor_port->sws_converter =
        sws_getContext ( krad_compositor_port->krad_compositor->width,
                 krad_compositor_port->krad_compositor->height,
                 PIX_FMT_RGB32,
                 krad_compositor_port->subunit.width,
                 krad_compositor_port->subunit.height,
                 krad_compositor_port->yuv_color_depth, 
                 SWS_BICUBIC,
                 NULL, NULL, NULL);
    }                   

    src[0] = (unsigned char *)krad_frame->pixels;

    sws_scale (krad_compositor_port->sws_converter, (const uint8_t * const*)src,
           rgb_stride_arr, 0, krad_compositor_port->krad_compositor->height, yuv_pixels, yuv_strides);
      return krad_frame;
  }

  return NULL;
}

void krad_compositor_port_push_rgba_frame (krad_compositor_port_t *krad_compositor_port, krad_frame_t *krad_frame) {


  int output_rgb_stride_arr[4] = {4*krad_compositor_port->krad_compositor->width, 0, 0, 0};
  int input_rgb_stride_arr[4] = {4*krad_compositor_port->source_width, 0, 0, 0};  
  unsigned char *dst[4];
  unsigned char *src[4];  
  krad_frame_t *scaled_frame;  
  
  krad_frame->format = PIX_FMT_RGB32;
  
  if ((krad_compositor_port->source_width != krad_compositor_port->subunit.width) ||
    (krad_compositor_port->source_height != krad_compositor_port->subunit.height)) {
  
    if ((krad_compositor_port->io_params_updated) || (krad_compositor_port->comp_params_updated)) {
      if (krad_compositor_port->sws_converter != NULL) {
        sws_freeContext ( krad_compositor_port->sws_converter );
        krad_compositor_port->sws_converter = NULL;
      }
      krad_compositor_port->io_params_updated = 0;
      krad_compositor_port->comp_params_updated = 0;
      printkd ("I knew about the rgb update");
    }
  
    if (krad_compositor_port->sws_converter == NULL) {

      krad_compositor_port->sws_converter =
        sws_getContext ( krad_compositor_port->source_width,
                 krad_compositor_port->source_height,
                 krad_frame->format,
                 krad_compositor_port->subunit.width,
                 krad_compositor_port->subunit.height,
                 PIX_FMT_RGB32, 
                 SWS_BICUBIC,
                 NULL, NULL, NULL);
        
      if (krad_compositor_port->sws_converter == NULL) {
        failfast ("Krad Compositor: fraked");
      }
                 
      printk ("set scaling to w %d h %d sw %d sh %d",
          krad_compositor_port->subunit.width,
          krad_compositor_port->subunit.height,
          krad_compositor_port->source_width,
          krad_compositor_port->source_height);               
    }

    while (krad_ringbuffer_read_space (krad_compositor_port->frame_ring) >= (sizeof(krad_frame_t *) * 30)) {
      usleep (18000);
      //kludge to not buffer more than 1 handfull? of frames ahead for fast sources
    }
    
    scaled_frame = krad_framepool_getframe (krad_compositor_port->krad_compositor->framepool);           

    if (scaled_frame == NULL) {
      failfast ("Krad Compositor: out of frames");
    }

    src[0] = (unsigned char *)krad_frame->pixels;
    dst[0] = (unsigned char *)scaled_frame->pixels;

    sws_scale (krad_compositor_port->sws_converter, (const uint8_t * const*)src,
           input_rgb_stride_arr, 0, krad_compositor_port->source_height, dst, output_rgb_stride_arr);

    krad_compositor_port_push_frame (krad_compositor_port, scaled_frame);
    krad_framepool_unref_frame (scaled_frame);
  } else {
    krad_compositor_port_push_frame (krad_compositor_port, krad_frame);
  }
}

void krad_compositor_port_push_frame (krad_compositor_port_t *krad_compositor_port, krad_frame_t *krad_frame) {

  int wrote;

  krad_framepool_ref_frame (krad_frame);
  krad_ringbuffer_write (krad_compositor_port->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));

  if (krad_compositor_port->direction == OUTPUT) {
    wrote = write (krad_compositor_port->socketpair[0], "0", 1);
    if (wrote != 1) {
      printk ("Krad Compositor: port write unexpected write return value %d", wrote);
    }
  }
}

krad_frame_t *krad_compositor_port_pull_frame_local (krad_compositor_port_t *krad_compositor_port) {

  int ret;
  int wrote;
  char buf[1];
  
  static int frames = 0;
  
  ret = 0;
  wrote = 0;
  buf[0] = 0;

  cairo_surface_flush (krad_compositor_port->local_frame->cst);
  wrote = write (krad_compositor_port->msg_sd, buf, 1);

  if (wrote == 1) {
    ret = read (krad_compositor_port->msg_sd, buf, 1);
    if (ret == 1) {
      frames++;
      cairo_surface_mark_dirty (krad_compositor_port->local_frame->cst);
      if (frames == 5) {
        cairo_surface_write_to_png (krad_compositor_port->local_frame->cst, "/home/oneman/kode/kaf2.png");
      }
      return krad_compositor_port->local_frame;
    }
  }
  return NULL;
}

krad_frame_t *krad_compositor_port_pull_frame (krad_compositor_port_t *krad_compositor_port) {

  krad_frame_t *krad_frame;
  
  if (krad_compositor_port->local == 1) {
    return krad_compositor_port_pull_frame_local (krad_compositor_port);
  }
  
  if (krad_compositor_port->direction == INPUT) {

    if (krad_compositor_port->last_frame != NULL) {

      if ((krad_compositor_port->start_timecode + krad_compositor_port->last_frame->timecode) < 
        krad_compositor_port->krad_compositor->timecode) {

        if (krad_ringbuffer_read_space (krad_compositor_port->frame_ring) >= sizeof(krad_frame_t *)) {
          krad_ringbuffer_read (krad_compositor_port->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));

          krad_framepool_unref_frame (krad_compositor_port->last_frame);  
          krad_framepool_ref_frame (krad_frame);
          krad_compositor_port->last_frame = krad_frame;
    
          return krad_frame;
        } else {
          krad_framepool_ref_frame (krad_compositor_port->last_frame);
          return krad_compositor_port->last_frame;
        }
      } else {
        krad_framepool_ref_frame (krad_compositor_port->last_frame);
        return krad_compositor_port->last_frame;
      }
    } else {
      if (krad_ringbuffer_read_space (krad_compositor_port->frame_ring) >= sizeof(krad_frame_t *)) {
        krad_ringbuffer_read (krad_compositor_port->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));
  
        krad_compositor_port->start_timecode = krad_compositor_port->krad_compositor->timecode;
  
        krad_framepool_ref_frame (krad_frame);
        krad_compositor_port->last_frame = krad_frame;
  
        return krad_frame;
      }
    }
  } else {
    if (krad_ringbuffer_read_space (krad_compositor_port->frame_ring) >= sizeof(krad_frame_t *)) {
      krad_ringbuffer_read (krad_compositor_port->frame_ring, (char *)&krad_frame, sizeof(krad_frame_t *));      
      return krad_frame;
    }
  }

  return NULL;
}

int krad_compositor_port_frames_avail (krad_compositor_port_t *krad_compositor_port) {
  
  int frames;
  
  frames = krad_ringbuffer_read_space (krad_compositor_port->frame_ring);
  frames = frames / sizeof (krad_frame_t *);

  return frames;
}

static void krad_compositor_free_framepool (krad_compositor_t *compositor) {

  if (compositor->framepool != NULL) {
    krad_framepool_destroy ( compositor->framepool );
    compositor->framepool = NULL;
  }
  printk ("Krad Compositor: Free'd Resources");
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

int krad_compositor_port_get_fd (krad_compositor_port_t *krad_compositor_port) {
  return krad_compositor_port->socketpair[1];
}

krad_compositor_port_t *krad_compositor_port_create (krad_compositor_t *krad_compositor, char *sysname, int direction,
                                                     int width, int height) {
              
  return krad_compositor_port_create_full (krad_compositor, sysname, direction, width, height, 0, 0);
}

krad_compositor_port_t *krad_compositor_port_create_full (krad_compositor_t *krad_compositor, char *sysname, int direction,
                           int width, int height, int holdlock, int local) {

  krad_compositor_port_t *krad_compositor_port;

  int p;
  int x;
  int y;
  
  x = 0;
  y = 0;
  krad_compositor_port = NULL; 
  
  for (p = 0; p < KC_MAX_PORTS; p++) {
    if (krad_compositor->port[p].subunit.active == 0) {
      krad_compositor_port = &krad_compositor->port[p];
      krad_compositor_port->subunit.address.path.unit = KR_COMPOSITOR;
      krad_compositor_port->subunit.address.path.subunit.compositor_subunit = KR_VIDEOPORT;
      krad_compositor_port->subunit.address.id.number = p;
      krad_compositor_port->subunit.active = 2;
      break;
    }
  }
  
  if (krad_compositor_port == NULL) {
    return NULL;
  }
  
  krad_compositor_port->krad_compositor = krad_compositor;
  krad_compositor_port->local = local;
  krad_compositor_port->direction = direction;  
  strcpy (krad_compositor_port->sysname, sysname);  
  krad_compositor_port->start_timecode = 1;

  krad_compositor_port->crop_x = 0;
  krad_compositor_port->crop_y = 0;
  
  if (krad_compositor_port->direction == INPUT) {
    krad_compositor_port->source_width = width;
    krad_compositor_port->source_height = height;
    krad_compositor_aspect_scale (krad_compositor_port->source_width, krad_compositor_port->source_height,
                                  krad_compositor->width, krad_compositor->height,
                                  &krad_compositor_port->subunit.width, &krad_compositor_port->subunit.height);
                    
    if (krad_compositor_port->subunit.width < krad_compositor_port->krad_compositor->width) {
      x = (krad_compositor_port->krad_compositor->width - krad_compositor_port->subunit.width) / 2;
    }
  
    if (krad_compositor_port->subunit.height < krad_compositor_port->krad_compositor->height) {
      y = (krad_compositor_port->krad_compositor->height - krad_compositor_port->subunit.height) / 2;
    }
    
    krad_compositor_subunit_set_xy (&krad_compositor_port->subunit, x, y, 0);  
    
    krad_compositor_port->crop_width = krad_compositor_port->subunit.width;
    krad_compositor_port->crop_height = krad_compositor_port->subunit.height;
      
  } else {
  
    //OUTPUT
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, krad_compositor_port->socketpair)) {
      printk ("Krad Compositor: subunit could not create socketpair errno: %d", errno);
      return NULL;
    }
  
    krad_compositor_port->source_width = krad_compositor->width;
    krad_compositor_port->source_height = krad_compositor->height;
    krad_compositor_port->subunit.width = width;
    krad_compositor_port->subunit.height = height;
    krad_compositor_port->crop_width = krad_compositor->width;
    krad_compositor_port->crop_height = krad_compositor->height;
    krad_compositor_port->yuv_color_depth = PIX_FMT_YUV420P;
  }

  krad_compositor_port->frame_ring = 
  krad_ringbuffer_create ( DEFAULT_COMPOSITOR_BUFFER_FRAMES * sizeof(krad_frame_t *) );

  if (krad_compositor_port->local == 0) {
    krad_compositor_port->frame_ring = 
    krad_ringbuffer_create ( DEFAULT_COMPOSITOR_BUFFER_FRAMES * sizeof(krad_frame_t *) );
    if (krad_compositor_port->frame_ring == NULL) {
      failfast ("oh dearring im out of mem");
    }
  } else {
    krad_compositor_port->frame_ring = NULL;
  }

  krad_compositor->active_ports++;
  if (krad_compositor_port->direction == INPUT) {
    krad_compositor->active_input_ports++;
  }
  if (krad_compositor_port->direction == OUTPUT) {
    krad_compositor->active_output_ports++;
  }

  if (holdlock == 0) {
    krad_compositor_port->subunit.active = 1;
  }
  
  return krad_compositor_port;
}

krad_compositor_port_t *krad_compositor_local_port_create (krad_compositor_t *krad_compositor,
                               char *sysname, int direction, int shm_sd, int msg_sd) {

  krad_compositor_port_t *krad_compositor_port;
  krad_compositor_port = krad_compositor_port_create_full (krad_compositor, sysname, direction,
                            krad_compositor->width, krad_compositor->height, 1, 1);

  krad_compositor_port->shm_sd = 0;
  krad_compositor_port->msg_sd = 0;  
  krad_compositor_port->local_buffer = NULL;
  krad_compositor_port->local_buffer_size = 960 * 540 * 4 * 2;
  
  krad_compositor_port->shm_sd = shm_sd;
  krad_compositor_port->msg_sd = msg_sd;
  
  krad_compositor_port->local_buffer = mmap (NULL, krad_compositor_port->local_buffer_size,
                         PROT_READ | PROT_WRITE, MAP_SHARED,
                         krad_compositor_port->shm_sd, 0);

  if ((krad_compositor_port->local_buffer != NULL) && (krad_compositor_port->shm_sd != 0) &&
    (krad_compositor_port->msg_sd != 0)) {
    
    krad_compositor_port->local_frame = calloc (1, sizeof(krad_frame_t));
    if (krad_compositor_port->local_frame == NULL) {
      failfast ("oh dear im out of mem");
    }
    
    krad_compositor_port->local_frame->pixels = (int *)krad_compositor_port->local_buffer;
    
    krad_compositor_port->local_frame->cst =
      cairo_image_surface_create_for_data ((unsigned char *)krad_compositor_port->local_buffer,
                         CAIRO_FORMAT_ARGB32,
                         960,
                         540,
                         960 * 4);
  
    krad_compositor_port->local_frame->cr = cairo_create (krad_compositor_port->local_frame->cst);    
    
    
    
    krad_compositor_port->subunit.active = 1;
  } else {
    printke ("Krad Compositor: failed to create local port");
    krad_compositor_port_destroy (krad_compositor, krad_compositor_port);
    return NULL;
  }

  return krad_compositor_port;  
}

void krad_compositor_port_destroy (krad_compositor_t *krad_compositor, krad_compositor_port_t *krad_compositor_port) {
  krad_compositor_port->subunit.active = 2;
}

static void krad_compositor_port_destroy_actual (krad_compositor_t *krad_compositor, krad_compositor_port_t *krad_compositor_port) {

  if (krad_compositor_port->direction == INPUT) {
    krad_compositor->active_input_ports--;
  }
  if (krad_compositor_port->direction == OUTPUT) {
    krad_compositor->active_output_ports--;
    close (krad_compositor_port->socketpair[0]);
    close (krad_compositor_port->socketpair[1]);      
  }
   
   if (krad_compositor_port->local == 1) {
    if (krad_compositor_port->msg_sd != 0) {
      close (krad_compositor_port->msg_sd);
      krad_compositor_port->msg_sd = 0;  
    }
    if (krad_compositor_port->shm_sd != 0) {
      close (krad_compositor_port->shm_sd);
      krad_compositor_port->shm_sd = 0;
    }
    if (krad_compositor_port->local_buffer != NULL) {
      munmap (krad_compositor_port->local_buffer, krad_compositor_port->local_buffer_size);
      krad_compositor_port->local_buffer_size = 0;
      krad_compositor_port->local_buffer = NULL;
    }
    
    if (krad_compositor_port->local_frame != NULL) {
      cairo_destroy (krad_compositor_port->local_frame->cr);
      cairo_surface_destroy (krad_compositor_port->local_frame->cst);
      free (krad_compositor_port->local_frame);
    }
    krad_compositor_port->local = 0;
   }

  if (krad_compositor_port->frame_ring != NULL) {
    krad_ringbuffer_free ( krad_compositor_port->frame_ring );
    krad_compositor_port->frame_ring = NULL;
  }
  krad_compositor_port->start_timecode = 0;

  if (krad_compositor_port->sws_converter != NULL) {
    sws_freeContext ( krad_compositor_port->sws_converter );
    krad_compositor_port->sws_converter = NULL;
  }

  if (krad_compositor_port->last_frame != NULL) {
    krad_framepool_unref_frame (krad_compositor_port->last_frame);
    krad_compositor_port->last_frame = NULL;
  }
  krad_compositor->active_ports--;
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

void krad_compositor_unset_pusher (krad_compositor_t *krad_compositor) {
  if (krad_compositor->ticker_running == 1) {
    krad_compositor_stop_ticker (krad_compositor);
  }
  krad_compositor_start_ticker (krad_compositor);
  krad_compositor->pusher = 0;
}

void krad_compositor_set_pusher (krad_compositor_t *krad_compositor, krad_display_api_t pusher) {
  if (pusher == 0) {
    krad_compositor_unset_pusher (krad_compositor);
  } else {
    if (krad_compositor->ticker_running == 1) {
      krad_compositor_stop_ticker (krad_compositor);
    }  
    krad_compositor->pusher = pusher;
  }
}

int krad_compositor_has_pusher (krad_compositor_t *krad_compositor) {
  if (krad_compositor->pusher == 0) {
    return 0;
  } else {
    return 1;
  }
}

krad_display_api_t krad_compositor_get_pusher (krad_compositor_t *krad_compositor) {
  return krad_compositor->pusher;
}

void krad_compositor_set_dir (krad_compositor_t *krad_compositor, char *dir) {

  if (krad_compositor->dir != NULL) {
    free (krad_compositor->dir);
  }

  krad_compositor->dir = strdup (dir);
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


  if ((compositor == NULL) || (uc == NULL) || (uc->address.id.number < 0)) {
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
              case KR_GREEN:
                krad_compositor_subunit_set_green (&compositor->sprite[uc->address.id.number].subunit,
                                                   uc->value.real, uc->duration);
              case KR_BLUE:
                krad_compositor_subunit_set_blue (&compositor->sprite[uc->address.id.number].subunit,
                                                  uc->value.real, uc->duration);
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
              case KR_GREEN:
                krad_compositor_subunit_set_green (&compositor->text[uc->address.id.number].subunit,
                                                   uc->value.real, uc->duration);
              case KR_BLUE:
                krad_compositor_subunit_set_blue (&compositor->text[uc->address.id.number].subunit,
                                                  uc->value.real, uc->duration);
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
              case KR_GREEN:
                krad_compositor_subunit_set_green (&compositor->vector[uc->address.id.number].subunit,
                                                   uc->value.real, uc->duration);
              case KR_BLUE:
                krad_compositor_subunit_set_blue (&compositor->vector[uc->address.id.number].subunit,
                                                  uc->value.real, uc->duration);
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
                krad_compositor_subunit_set_width (&compositor->port[uc->address.id.number].subunit,
                                               uc->value.integer, uc->duration);                
                break;
              case KR_HEIGHT:
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
              case KR_GREEN:
                krad_compositor_subunit_set_green (&compositor->port[uc->address.id.number].subunit,
                                                   uc->value.real, uc->duration);
              case KR_BLUE:
                krad_compositor_subunit_set_blue (&compositor->port[uc->address.id.number].subunit,
                                                  uc->value.real, uc->duration);
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

void krad_compositor_subunit_create (krad_compositor_t *compositor,
                                     kr_compositor_subunit_t type,
                                     char *option) {

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
          }
          break;
        }
      }
      return;
    case KR_TEXT:
      for (i = 0; i < KC_MAX_TEXTS; i++) {
        if (compositor->text[i].subunit.active == 0) {
          krad_text_set_text (&compositor->text[i], option);
          compositor->text[i].subunit.active = 1;
          compositor->active_texts++;
          break;
        }
      }
      return;
    case KR_VECTOR:
      for (i = 0; i < KC_MAX_VECTORS; i++) {
        if (compositor->vector[i].subunit.active == 0) {
          if (krad_string_to_vector_type(option) != NOTHING) {
            krad_vector_set_type (&compositor->vector[i], option);
            compositor->vector[i].subunit.active = 1;
            compositor->active_vectors++;
          }
          break;
        }
      }
      return;
    case KR_VIDEOPORT:
      return;
  }
}

void krad_compositor_subunit_destroy (krad_compositor_t *compositor, kr_address_t *address) {

  if ((compositor == NULL) || (address == NULL) || (address->id.number < 0)) {
    return;
  }

  if (address->path.unit == KR_COMPOSITOR) {
    switch ( address->path.subunit.compositor_subunit ) {
      case KR_SPRITE:
        if (address->id.number < KC_MAX_SPRITES) {
          if (compositor->sprite[address->id.number].subunit.active == 1) {
            compositor->sprite[address->id.number].subunit.active = 2;
          }
        }
        return;
      case KR_TEXT:
        if (address->id.number < KC_MAX_TEXTS) {
          if (compositor->text[address->id.number].subunit.active == 1) {
            compositor->text[address->id.number].subunit.active = 2;
          }
        }
        return;
      case KR_VECTOR:
        if (address->id.number < KC_MAX_VECTORS) {
          if (compositor->vector[address->id.number].subunit.active == 1) {
            compositor->vector[address->id.number].subunit.active = 2;
          }
        }
        return;
      case KR_VIDEOPORT:
        if (address->id.number < KC_MAX_PORTS) {
          if (compositor->port[address->id.number].subunit.active == 1) {
            compositor->port[address->id.number].subunit.active = 2;
          }
        }
        return;
      default:
        return;
    }
  }
}

static void krad_compositor_set_resolution (krad_compositor_t *krad_compositor, int width, int height) {

  krad_compositor->width = width;
  krad_compositor->height = height;
  krad_compositor->frame_byte_size = krad_compositor->width * krad_compositor->height * 4;
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
  compositor->text = krad_text_create_arr (KC_MAX_TEXTS);
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

  if (compositor->display_open == 1) {
    krad_compositor_close_display (compositor);
  }

  krad_compositor_ports_destroy_all (compositor);
  krad_compositor_stop_ticker (compositor);
  krad_compositor_free_framepool (compositor);
  krad_sprite_destroy (compositor->background);
  krad_sprite_destroy_arr (compositor->sprite, KC_MAX_SPRITES);
  krad_text_destroy_arr (compositor->text, KC_MAX_TEXTS);
  krad_vector_destroy_arr (compositor->vector, KC_MAX_VECTORS);
  free (compositor);

  printk ("Krad Compositor: Destroy Complete");
}

krad_compositor_t *krad_compositor_create (int width, int height, int fps_numerator, int fps_denominator) {

  krad_compositor_t *compositor = calloc (1, sizeof(krad_compositor_t));

  printk ("Krad Compositor: Cairo Version: %s", cairo_version_string());

  krad_compositor_set_resolution (compositor, width, height);
  krad_compositor_set_frame_rate (compositor, fps_numerator, fps_denominator);

  compositor->address.path.unit = KR_COMPOSITOR;
  compositor->address.path.subunit.compositor_subunit = KR_UNIT;
  compositor->background = krad_sprite_create ();
  krad_compositor_prepare_subunits (compositor);
  //krad_compositor_alloc_framepool (compositor);

  return compositor;
}
