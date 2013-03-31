#include "krad_transponder.h"

#ifdef KRAD_USE_WAYLAND

int wayland_display_unit_render_callback (void *pointer, uint32_t time) {

  krad_link_t *krad_link = (krad_link_t *)pointer;

  int ret;
  char buffer[1];
  int updated;
  krad_frame_t *krad_frame;
  
  updated = 0;
  
  krad_frame = krad_compositor_port_pull_frame (krad_link->krad_compositor_port2);

  if (krad_frame != NULL) {
    //FIXME do this first etc
    ret = read (krad_link->krad_compositor_port2->socketpair[1], buffer, 1);
    if (ret != 1) {
      if (ret == 0) {
        printk ("Krad OTransponder: port read got EOF");
        return updated;
      }
      printk ("Krad OTransponder: port read unexpected read return value %d", ret);
    }

    memcpy (krad_link->wl_buffer,
            krad_frame->pixels,
            krad_link->composite_width * krad_link->composite_height * 4);

    krad_framepool_unref_frame (krad_frame);
    updated = 1;
  }
  return updated;
}

void wayland_display_unit_create (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  krad_system_set_thread_name ("kr_wl_dsp");
  
  printk ("Wayland display thread begins");
  
  krad_link->krad_wayland = krad_wayland_create ();

  krad_link->krad_compositor_port2 = krad_compositor_port_create (krad_link->krad_radio->krad_compositor, "WLOut", OUTPUT,
                                                                  krad_link->composite_width,
                                                                  krad_link->composite_height);
  //krad_link->krad_wayland->render_test_pattern = 1;

  krad_wayland_set_frame_callback (krad_link->krad_wayland,
                   wayland_display_unit_render_callback,
                   krad_link);

  krad_wayland_prepare_window (krad_link->krad_wayland,
                 krad_link->composite_width,
                 krad_link->composite_height,
                 &krad_link->wl_buffer);

  printk ("Wayland display prepared");

  krad_wayland_open_window (krad_link->krad_wayland);

  printk("Wayland display running");
}

int wayland_display_unit_process (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;
  
  krad_wayland_iterate (krad_link->krad_wayland);
  
  return 0;
}

void wayland_display_unit_destroy (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;
  
  krad_wayland_close_window (krad_link->krad_wayland);
  krad_wayland_destroy (krad_link->krad_wayland);
  krad_compositor_port_destroy (krad_link->krad_radio->krad_compositor,
                                krad_link->krad_compositor_port2);

  printk ("Wayland display thread exited");
}

#endif

#ifndef __MACH__
/*
void v4l2_loopout_unit_create (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  krad_system_set_thread_name ("kr_v4l2_lo");
  
  printk ("V4L2 Loop Output thread begins");
  

}

int v4l2_loopout_unit_process (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;
  
  return 0;
}

void v4l2_loopout_unit_destroy (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;
  


  printk ("V4L2 Loop Output thread exited");
  
}
*/

void v4l2_capture_unit_create (void *arg) {
  //krad_system_set_thread_name ("kr_cap_v4l2");

  krad_link_t *krad_link = (krad_link_t *)arg;
  
  printk ("Video capture creating..");
  
  krad_link->krad_v4l2 = krad_v4l2_create ();

  if (krad_link->video_codec != NOCODEC) {
    if (krad_link->video_codec == MJPEG) {
      krad_v4l2_mjpeg_mode (krad_link->krad_v4l2);
    }
    if (krad_link->video_codec == H264) {
      krad_v4l2_h264_mode (krad_link->krad_v4l2);
    }
  }

  if ((krad_link->fps_numerator == 0) || (krad_link->fps_denominator == 0)) {
    krad_compositor_get_frame_rate (krad_link->krad_radio->krad_compositor,
                                    &krad_link->fps_numerator,
                                    &krad_link->fps_denominator);
  }
  
  if ((krad_link->capture_width == 0) || (krad_link->capture_height == 0)) {
    krad_compositor_get_resolution (krad_link->krad_radio->krad_compositor,
                                    &krad_link->capture_width,
                                    &krad_link->capture_height);
  }

  krad_v4l2_open (krad_link->krad_v4l2, krad_link->device, krad_link->capture_width, 
           krad_link->capture_height, 30);

  if ((krad_link->capture_width != krad_link->krad_v4l2->width) ||
      (krad_link->capture_height != krad_link->krad_v4l2->height)) {

    printke ("Got a different resolution from V4L2 than requested.");
    printk ("Wanted: %dx%d Got: %dx%d",
        krad_link->capture_width, krad_link->capture_height,
        krad_link->krad_v4l2->width, krad_link->krad_v4l2->height
        );
         
    krad_link->capture_width = krad_link->krad_v4l2->width;
    krad_link->capture_height = krad_link->krad_v4l2->height;
  }

  if (krad_link->video_passthru == 1) {
    krad_link->krad_framepool = krad_framepool_create_for_passthru (350000, DEFAULT_CAPTURE_BUFFER_FRAMES * 3);
  } else {

    krad_link->krad_framepool = krad_framepool_create_for_upscale ( krad_link->capture_width,
                              krad_link->capture_height,
                              DEFAULT_CAPTURE_BUFFER_FRAMES,
                              krad_link->composite_width, krad_link->composite_height);
  }

  if (krad_link->video_passthru == 1) {
    //FIXME
  } else {
    krad_link->krad_compositor_port = 
    krad_compositor_port_create (krad_link->krad_radio->krad_compositor, "V4L2In", INPUT,
                   krad_link->capture_width, krad_link->capture_height);
  }
  
  krad_v4l2_start_capturing (krad_link->krad_v4l2);

  printk ("Video capture started..");

}

int v4l2_capture_unit_process (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  void *captured_frame;
  krad_frame_t *krad_frame;
  
  captured_frame = NULL;
  krad_frame = NULL;

  captured_frame = krad_v4l2_read (krad_link->krad_v4l2);    

  if (captured_frame != NULL) {
    krad_frame = krad_framepool_getframe (krad_link->krad_framepool);
    if (krad_frame != NULL) {
      if (krad_link->video_passthru == 1) {
        memcpy (krad_frame->pixels, captured_frame, krad_link->krad_v4l2->encoded_size);
        krad_frame->encoded_size = krad_link->krad_v4l2->encoded_size;
        krad_compositor_port_push_frame (krad_link->krad_compositor_port, krad_frame);
      } else {
        if (krad_link->video_codec == MJPEG) {
          krad_v4l2_mjpeg_to_rgb (krad_link->krad_v4l2, (unsigned char *)krad_frame->pixels,
                                 captured_frame, krad_link->krad_v4l2->encoded_size);      
          krad_compositor_port_push_rgba_frame (krad_link->krad_compositor_port, krad_frame);
        } else {
          krad_frame->format = PIX_FMT_YUYV422;
          krad_frame->yuv_pixels[0] = captured_frame;
          krad_frame->yuv_pixels[1] = NULL;
          krad_frame->yuv_pixels[2] = NULL;
          krad_frame->yuv_strides[0] = krad_link->capture_width + (krad_link->capture_width/2) * 2;
          krad_frame->yuv_strides[1] = 0;
          krad_frame->yuv_strides[2] = 0;
          krad_frame->yuv_strides[3] = 0;
          krad_compositor_port_push_yuv_frame (krad_link->krad_compositor_port, krad_frame);
        }
      }
      krad_framepool_unref_frame (krad_frame);
    }
    krad_v4l2_frame_done (krad_link->krad_v4l2);
  }

  return 0;
}

void v4l2_capture_unit_destroy (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  krad_v4l2_stop_capturing (krad_link->krad_v4l2);
  krad_v4l2_close(krad_link->krad_v4l2);
  krad_v4l2_destroy(krad_link->krad_v4l2);

  krad_compositor_port_destroy (krad_link->krad_radio->krad_compositor, krad_link->krad_compositor_port);

  printk ("v4l2 capture unit destroy");
  
}

#endif

void x11_capture_unit_create (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  krad_system_set_thread_name ("kr_x11_cap");
  
  printk ("X11 capture thread begins");
  
  krad_link->krad_x11 = krad_x11_create();
  
  if (krad_link->video_source == X11) {
    krad_link->krad_framepool = krad_framepool_create ( krad_link->krad_x11->screen_width,
                              krad_link->krad_x11->screen_height,
                              DEFAULT_CAPTURE_BUFFER_FRAMES);
  }
  
  krad_x11_enable_capture (krad_link->krad_x11, krad_link->krad_x11->screen_width, krad_link->krad_x11->screen_height);
  
  krad_link->krad_compositor_port = krad_compositor_port_create (krad_link->krad_radio->krad_compositor,
                                   "X11In",
                                   INPUT,
                                   krad_link->krad_x11->screen_width, krad_link->krad_x11->screen_height);

}

int x11_capture_unit_process (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;
  
  krad_frame_t *krad_frame;

  if (krad_link->krad_ticker == NULL) {
    krad_link->krad_ticker = krad_ticker_create (krad_link->krad_radio->krad_compositor->fps_numerator,
                      krad_link->krad_radio->krad_compositor->fps_denominator);
    krad_ticker_start (krad_link->krad_ticker);
  } else {
    krad_ticker_wait (krad_link->krad_ticker);
  }
  
  krad_frame = krad_framepool_getframe (krad_link->krad_framepool);

  krad_x11_capture (krad_link->krad_x11, (unsigned char *)krad_frame->pixels);
  
  krad_compositor_port_push_rgba_frame (krad_link->krad_compositor_port, krad_frame);

  krad_framepool_unref_frame (krad_frame);

  return 0;
}

void x11_capture_unit_destroy (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;
  
  krad_compositor_port_destroy (krad_link->krad_radio->krad_compositor, krad_link->krad_compositor_port);

  krad_ticker_destroy (krad_link->krad_ticker);
  krad_link->krad_ticker = NULL;

  krad_x11_destroy (krad_link->krad_x11);

  printk ("X11 capture thread exited");
  
}


int krad_link_decklink_video_callback (void *arg, void *buffer, int length) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  int stride;
  krad_frame_t *krad_frame;
  
  stride = krad_link->capture_width + ((krad_link->capture_width/2) * 2);
  //printk ("krad link decklink frame received %d bytes", length);

  krad_frame = krad_framepool_getframe (krad_link->krad_framepool);

  if (krad_frame != NULL) {

    krad_frame->format = PIX_FMT_UYVY422;

    krad_frame->yuv_pixels[0] = buffer;
    krad_frame->yuv_pixels[1] = NULL;
    krad_frame->yuv_pixels[2] = NULL;

    krad_frame->yuv_strides[0] = stride;
    krad_frame->yuv_strides[1] = 0;
    krad_frame->yuv_strides[2] = 0;
    krad_frame->yuv_strides[3] = 0;

    krad_compositor_port_push_yuv_frame (krad_link->krad_compositor_port, krad_frame);
/*
    krad_frame->format = PIX_FMT_RGB32;
    krad_frame->pixels = buffer;    
    krad_compositor_port_push_rgba_frame (krad_link->krad_compositor_port, krad_frame);
*/
    krad_framepool_unref_frame (krad_frame);
    //krad_compositor_process (krad_link->krad_transponder->krad_radio->krad_compositor);
  } else {
    //failfast ("Krad Decklink underflow");
  }

  return 0;
}

#define SAMPLE_16BIT_SCALING 32767.0f

void krad_link_int16_to_float (float *dst, char *src, unsigned long nsamples, unsigned long src_skip) {

  const float scaling = 1.0/SAMPLE_16BIT_SCALING;
  while (nsamples--) {
    *dst = (*((short *) src)) * scaling;
    dst++;
    src += src_skip;
  }
}

int krad_link_decklink_audio_callback (void *arg, void *buffer, int frames) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  int c;

  for (c = 0; c < 2; c++) {
    krad_link_int16_to_float ( krad_link->krad_decklink->samples[c], (char *)buffer + (c * 2), frames, 4);
    krad_ringbuffer_write (krad_link->audio_capture_ringbuffer[c], (char *)krad_link->krad_decklink->samples[c], frames * 4);
  }
  krad_link->audio_frames_captured += frames;
  return 0;
}

void decklink_capture_unit_create (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;
  int c;

  krad_system_set_thread_name ("kr_decklink");

  krad_link->krad_decklink = krad_decklink_create (krad_link->device);
  
  if ((krad_link->fps_numerator == 0) || (krad_link->fps_denominator == 0)) {
    krad_compositor_get_frame_rate (krad_link->krad_radio->krad_compositor,
                                    &krad_link->fps_numerator,
                                    &krad_link->fps_denominator);
  }
  
  if ((krad_link->capture_width == 0) || (krad_link->capture_height == 0)) {
    krad_compositor_get_resolution (krad_link->krad_radio->krad_compositor,
                                    &krad_link->capture_width,
                                    &krad_link->capture_height);
  }
  
  krad_decklink_set_video_mode (krad_link->krad_decklink,
                                krad_link->capture_width, krad_link->capture_height,
                                krad_link->fps_numerator, krad_link->fps_denominator);
  krad_decklink_set_audio_input (krad_link->krad_decklink, krad_link->audio_input);
  krad_decklink_set_video_input (krad_link->krad_decklink, "hdmi");

  for (c = 0; c < krad_link->channels; c++) {
    krad_link->audio_capture_ringbuffer[c] = krad_ringbuffer_create (1000000);    
  }

  krad_link->krad_framepool = krad_framepool_create ( krad_link->capture_width,
                                                      krad_link->capture_height,
                                                      DEFAULT_CAPTURE_BUFFER_FRAMES);

  krad_link->krad_mixer_portgroup = krad_mixer_portgroup_create (krad_link->krad_radio->krad_mixer,
                                                                 krad_link->krad_decklink->simplename,
                                                                 INPUT, NOTOUTPUT, 2, 0.0f,
                                                                 krad_link->krad_radio->krad_mixer->master_mix,
                                                                 KRAD_LINK, krad_link, 0);  
  
  krad_mixer_set_portgroup_control (krad_link->krad_radio->krad_mixer,
                                    krad_link->krad_decklink->simplename,
                                    "volume", 100.0f, 500, NULL);
  
  krad_link->krad_compositor_port = krad_compositor_port_create (krad_link->krad_radio->krad_compositor,
                                                                 krad_link->krad_decklink->simplename,
                                                                 INPUT, krad_link->capture_width,
                                                                 krad_link->capture_height);

  krad_link->krad_decklink->callback_pointer = krad_link;
  krad_link->krad_decklink->audio_frames_callback = krad_link_decklink_audio_callback;
  krad_link->krad_decklink->video_frame_callback = krad_link_decklink_video_callback;
  krad_decklink_start (krad_link->krad_decklink);
}

void decklink_capture_unit_destroy (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;
  int c;

  if (krad_link->krad_decklink != NULL) {
    krad_decklink_destroy ( krad_link->krad_decklink );
    krad_link->krad_decklink = NULL;
  }

  krad_mixer_portgroup_destroy (krad_link->krad_radio->krad_mixer, krad_link->krad_mixer_portgroup);
  krad_compositor_port_destroy (krad_link->krad_radio->krad_compositor, krad_link->krad_compositor_port);
  
  for (c = 0; c < krad_link->channels; c++) {
    krad_ringbuffer_free ( krad_link->audio_capture_ringbuffer[c] );
  }  
  
}

void video_encoding_unit_create (void *arg) {
  //krad_system_set_thread_name ("kr_video_enc");

  krad_link_t *krad_link = (krad_link_t *)arg;

  //printk ("Video encoding thread started");

  krad_link->color_depth = PIX_FMT_YUV420P;

  /* CODEC SETUP */

  if (krad_link->codec == VP8) {

  krad_link->krad_vpx_encoder = krad_vpx_encoder_create (krad_link->encoding_width,
                                                         krad_link->encoding_height,
                                                         krad_link->encoding_fps_numerator,
                                                         krad_link->encoding_fps_denominator,                                 
                                                         krad_link->vp8_bitrate);

    //if (krad_link->type == TRANSMIT) {
    //  krad_link->krad_vpx_encoder->cfg.kf_max_dist = 90;
    //}

    //if (krad_link->type == RECORD) {
    //  krad_link->krad_vpx_encoder->cfg.rc_min_quantizer = 5;
    //  krad_link->krad_vpx_encoder->cfg.rc_max_quantizer = 35;          
    //}

    krad_vpx_encoder_config_set (krad_link->krad_vpx_encoder, &krad_link->krad_vpx_encoder->cfg);

    krad_vpx_encoder_deadline_set (krad_link->krad_vpx_encoder, 13333);

    krad_vpx_encoder_print_config (krad_link->krad_vpx_encoder);
  }

  if (krad_link->codec == THEORA) {
    krad_link->krad_theora_encoder = krad_theora_encoder_create (krad_link->encoding_width, 
                                                                 krad_link->encoding_height,
                                                                 krad_link->encoding_fps_numerator,
                                                                 krad_link->encoding_fps_denominator,
                                                                 krad_link->color_depth,
                                                                 krad_link->theora_quality);
  }

  if (krad_link->codec == Y4M) {
    krad_link->krad_y4m = krad_y4m_create (krad_link->encoding_width, krad_link->encoding_height, krad_link->color_depth);
  }

  if (krad_link->codec == KVHS) {
    krad_link->krad_vhs = krad_vhs_create_encoder (krad_link->encoding_width, krad_link->encoding_height);
  }

  /* COMPOSITOR CONNECTION */

  krad_link->krad_compositor_port = krad_compositor_port_create (krad_link->krad_radio->krad_compositor,
                                                                 "VIDEnc",
                                                                 OUTPUT,
                                                                 krad_link->encoding_width, 
                                                                 krad_link->encoding_height);

  krad_link->krad_compositor_port_fd = krad_compositor_port_get_fd (krad_link->krad_compositor_port);

}

krad_codec_header_t *video_encoding_unit_get_header (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  switch (krad_link->codec) {
    case THEORA:
      return &krad_link->krad_theora_encoder->krad_codec_header;
      break;
    default:
      break;
  }
  return NULL;
}

int video_encoding_unit_process (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  int ret;
  char buffer[1];
  krad_frame_t *krad_frame;
  krad_slice_t *krad_slice;  
  void *video_packet;
  int keyframe;
  int packet_size;
  unsigned char *planes[3];
  int strides[3];

  packet_size = 0;
  krad_frame = NULL;
  krad_slice = NULL;

  if (krad_link->codec == VP8) {
    planes[0] = krad_link->krad_vpx_encoder->image->planes[0];
    planes[1] = krad_link->krad_vpx_encoder->image->planes[1];
    planes[2] = krad_link->krad_vpx_encoder->image->planes[2];
    strides[0] = krad_link->krad_vpx_encoder->image->stride[0];
    strides[1] = krad_link->krad_vpx_encoder->image->stride[1];
    strides[2] = krad_link->krad_vpx_encoder->image->stride[2];
  }

  if (krad_link->codec == THEORA) {
    planes[0] = krad_link->krad_theora_encoder->ycbcr[0].data;
    planes[1] = krad_link->krad_theora_encoder->ycbcr[1].data;
    planes[2] = krad_link->krad_theora_encoder->ycbcr[2].data;
    strides[0] = krad_link->krad_theora_encoder->ycbcr[0].stride;
    strides[1] = krad_link->krad_theora_encoder->ycbcr[1].stride;
    strides[2] = krad_link->krad_theora_encoder->ycbcr[2].stride;  
  }

  if (krad_link->codec == Y4M) {
    planes[0] = krad_link->krad_y4m->planes[0];
    planes[1] = krad_link->krad_y4m->planes[1];
    planes[2] = krad_link->krad_y4m->planes[2];
    strides[0] = krad_link->krad_y4m->strides[0];
    strides[1] = krad_link->krad_y4m->strides[1];
    strides[2] = krad_link->krad_y4m->strides[2];
  }
  
  ret = read (krad_link->krad_compositor_port->socketpair[1], buffer, 1);
  if (ret != 1) {
    if (ret == 0) {
      printk ("Krad OTransponder: port read got EOF");
      return -1;
    }
    printk ("Krad OTransponder: port read unexpected read return value %d", ret);
  }

  if (krad_link->codec == KVHS) {
    krad_frame = krad_compositor_port_pull_frame (krad_link->krad_compositor_port);
  } else {
    krad_frame = krad_compositor_port_pull_yuv_frame (krad_link->krad_compositor_port, planes, strides, krad_link->color_depth);
  }

  if (krad_frame != NULL) {

    /* ENCODE FRAME */

    if (krad_link->codec == VP8) {
      packet_size = krad_vpx_encoder_write (krad_link->krad_vpx_encoder,
                                            (unsigned char **)&video_packet,
                                            &keyframe);
    }

    if (krad_link->codec == THEORA) {
      packet_size = krad_theora_encoder_write (krad_link->krad_theora_encoder,
                                               (unsigned char **)&video_packet,
                                               &keyframe);
    }

    if (krad_link->codec == KVHS) {
      packet_size = krad_vhs_encode (krad_link->krad_vhs, (unsigned char *)krad_frame->pixels);
      if (krad_link->subunit != NULL) {
        krad_slice = krad_slice_create_with_data (krad_link->krad_vhs->enc_buffer, packet_size);
        krad_slice->frames = 1;
        krad_slice->codec = krad_link->codec;
        krad_slice->keyframe = 1;
        krad_Xtransponder_slice_broadcast (krad_link->subunit, &krad_slice);
        krad_slice_unref (krad_slice);
      }
    } else {

      if (krad_link->codec == Y4M) {
        /*
        keyframe_char[0] = 1;

        packet_size = krad_link->krad_y4m->frame_size + Y4M_FRAME_HEADER_SIZE;

        krad_ringbuffer_write (krad_link->encoded_video_ringbuffer, (char *)&packet_size, 4);
        krad_ringbuffer_write (krad_link->encoded_video_ringbuffer, keyframe_char, 1);
        krad_ringbuffer_write (krad_link->encoded_video_ringbuffer, (char *)Y4M_FRAME_HEADER, Y4M_FRAME_HEADER_SIZE);
        krad_ringbuffer_write (krad_link->encoded_video_ringbuffer, (char *)krad_link->krad_y4m->planes[0], krad_link->krad_y4m->size[0]);
        krad_ringbuffer_write (krad_link->encoded_video_ringbuffer, (char *)krad_link->krad_y4m->planes[1], krad_link->krad_y4m->size[1]);
        krad_ringbuffer_write (krad_link->encoded_video_ringbuffer, (char *)krad_link->krad_y4m->planes[2], krad_link->krad_y4m->size[2]);
        */
      } else {

        if ((packet_size) || (krad_link->codec == THEORA)) {
          if (krad_link->subunit != NULL) {
            krad_slice = krad_slice_create_with_data (video_packet, packet_size);
            krad_slice->frames = 1;
            krad_slice->codec = krad_link->codec;
            krad_slice->keyframe = keyframe;
            krad_Xtransponder_slice_broadcast (krad_link->subunit, &krad_slice);
            krad_slice_unref (krad_slice);
          }
        }
      }
    }
    krad_framepool_unref_frame (krad_frame);
  }

  return 0;
}
  
void video_encoding_unit_destroy (void *arg) {
  
  krad_link_t *krad_link = (krad_link_t *)arg;  
  
//  void *video_packet;
//  int keyframe;
//  int packet_size;
  
  printk ("Video encoding unit destroying");  

  krad_compositor_port_destroy (krad_link->krad_radio->krad_compositor, krad_link->krad_compositor_port);

  if (krad_link->codec == VP8) {
    /*
    krad_vpx_encoder_finish (krad_link->krad_vpx_encoder);
    do {
      packet_size = krad_vpx_encoder_write (krad_link->krad_vpx_encoder,
                                            (unsigned char **)&video_packet,
                                            &keyframe);
      if (packet_size) {
        //FIXME goes with un needed memcpy above
        krad_ringbuffer_write (krad_link->encoded_video_ringbuffer, (char *)&packet_size, 4);
        krad_ringbuffer_write (krad_link->encoded_video_ringbuffer, keyframe_char, 1);
        krad_ringbuffer_write (krad_link->encoded_video_ringbuffer, (char *)video_packet, packet_size);
      }
    } while (packet_size);
    */
    krad_vpx_encoder_destroy (krad_link->krad_vpx_encoder);
  }

  if (krad_link->codec == THEORA) {
    krad_theora_encoder_destroy (krad_link->krad_theora_encoder);  
  }

  if (krad_link->codec == Y4M) {
    krad_y4m_destroy (krad_link->krad_y4m);  
  }

  if (krad_link->codec == KVHS) {
    krad_vhs_destroy (krad_link->krad_vhs);  
  }  

  printk ("Video encoding unit exited");
}

void audio_encoding_unit_create (void *arg) {

  //krad_system_set_thread_name ("kr_audio_enc");

  krad_link_t *krad_link = (krad_link_t *)arg;

  int c;

  printk ("Audio unit create");
  
  krad_link->channels = 2;
  
  if (krad_link->codec != VORBIS) {
    krad_link->au_buffer = malloc (300000);
  }
  
  krad_link->au_interleaved_samples = malloc (8192 * 4 * KRAD_MIXER_MAX_CHANNELS);
  
  for (c = 0; c < krad_link->channels; c++) {
    krad_link->au_samples[c] = malloc (8192 * 4);
    krad_link->samples[c] = malloc (8192 * 4);
    krad_link->audio_input_ringbuffer[c] = krad_ringbuffer_create (2000000);    
  }
  
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, krad_link->socketpair)) {
    printk ("Krad Compositor: subunit could not create socketpair errno: %d", errno);
    return;
  }
  
  krad_link->mixer_portgroup = krad_mixer_portgroup_create (krad_link->krad_radio->krad_mixer, krad_link->sysname, 
                                                            OUTPUT, DIRECT, krad_link->channels, 0.0f,
                                                            krad_link->krad_radio->krad_mixer->master_mix,
                                                            KRAD_LINK, krad_link, 0);    
    
  switch (krad_link->codec) {
    case VORBIS:
      krad_link->krad_vorbis = krad_vorbis_encoder_create (krad_link->channels,
                                 krad_link->krad_radio->krad_mixer->sample_rate,
                                 krad_link->vorbis_quality);
      krad_link->au_framecnt = KRAD_DEFAULT_VORBIS_FRAME_SIZE;
      break;
    case FLAC:
      krad_link->krad_flac = krad_flac_encoder_create (krad_link->channels,
                               krad_link->krad_radio->krad_mixer->sample_rate,
                               krad_link->flac_bit_depth);
      krad_link->au_framecnt = KRAD_DEFAULT_FLAC_FRAME_SIZE;
      break;
    case OPUS:
      krad_link->krad_opus = krad_opus_encoder_create (krad_link->channels,
                               krad_link->krad_radio->krad_mixer->sample_rate,
                               krad_link->opus_bitrate,
                               OPUS_APPLICATION_AUDIO);
      krad_link->au_framecnt = KRAD_MIN_OPUS_FRAME_SIZE;
      break;      
    default:
      failfast ("Krad Link Audio Encoder: Unknown Audio Codec");
  }
  krad_link->audio_encoder_ready = 1;
}

krad_codec_header_t *audio_encoding_unit_get_header (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  switch (krad_link->codec) {
    case VORBIS:
      return &krad_link->krad_vorbis->krad_codec_header;
      break;
    case FLAC:
      return &krad_link->krad_flac->krad_codec_header;
      break;
    case OPUS:
      return &krad_link->krad_opus->krad_codec_header;
      break;      
    default:
      failfast ("Krad Link Audio Encoder: Unknown Audio Codec");
  }
  return NULL;
}
  
int audio_encoding_unit_process (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  int c;
  //unsigned char *vorbis_buffer;
  float **float_buffer;
  int s;
  int bytes;
  int frames;
  int ret;
  char buffer[1];
  krad_slice_t *krad_slice;

  bytes = 0;
  krad_slice = NULL;
  
  ret = read (krad_link->socketpair[1], buffer, 1);
  if (ret != 1) {
    if (ret == 0) {
      printk ("Krad AU Transponder: port read got EOF");
      return -1;
    }
    printk ("Krad AU Transponder: port read unexpected read return value %d", ret);
  }
  
  if (krad_link->codec != VORBIS) {
    frames = krad_link->au_framecnt;
  }

  while (krad_ringbuffer_read_space(krad_link->audio_input_ringbuffer[krad_link->channels - 1]) >= krad_link->au_framecnt * 4) {

    if (krad_link->codec == OPUS) {
      for (c = 0; c < krad_link->channels; c++) {
        krad_ringbuffer_read (krad_link->audio_input_ringbuffer[c], (char *)krad_link->au_samples[c], (krad_link->au_framecnt * 4) );
        krad_opus_encoder_write (krad_link->krad_opus, c + 1, (char *)krad_link->au_samples[c], krad_link->au_framecnt * 4);
      }
      bytes = krad_opus_encoder_read (krad_link->krad_opus, krad_link->au_buffer, &krad_link->au_framecnt);
    }
    if (krad_link->codec == FLAC) {
      for (c = 0; c < krad_link->channels; c++) {
        krad_ringbuffer_read (krad_link->audio_input_ringbuffer[c], (char *)krad_link->au_samples[c], (krad_link->au_framecnt * 4) );
      }
      for (s = 0; s < krad_link->au_framecnt; s++) {
        for (c = 0; c < krad_link->channels; c++) {
          krad_link->au_interleaved_samples[s * krad_link->channels + c] = krad_link->au_samples[c][s];
        }
      }
      bytes = krad_flac_encode (krad_link->krad_flac, krad_link->au_interleaved_samples, krad_link->au_framecnt, krad_link->au_buffer);
    }
    if (krad_link->codec == VORBIS) {
      krad_vorbis_encoder_prepare (krad_link->krad_vorbis, krad_link->au_framecnt, &float_buffer);
      for (c = 0; c < krad_link->channels; c++) {
        krad_ringbuffer_read (krad_link->audio_input_ringbuffer[c], (char *)float_buffer[c], krad_link->au_framecnt * 4);
      }      
      krad_vorbis_encoder_wrote (krad_link->krad_vorbis, krad_link->au_framecnt);
      bytes = krad_vorbis_encoder_read (krad_link->krad_vorbis, &frames, &krad_link->au_buffer);
    }

    while (bytes > 0) {
      if (krad_link->subunit != NULL) {
        krad_slice = krad_slice_create_with_data (krad_link->au_buffer, bytes);
        krad_slice->frames = frames;
        krad_slice->codec = krad_link->codec;
        krad_Xtransponder_slice_broadcast (krad_link->subunit, &krad_slice);
        krad_slice_unref (krad_slice);
      }
      bytes = 0;
      if (krad_link->codec == VORBIS) {
        bytes = krad_vorbis_encoder_read (krad_link->krad_vorbis, &frames, &krad_link->au_buffer);
      }
      if (krad_link->codec == OPUS) {
        bytes = krad_opus_encoder_read (krad_link->krad_opus, krad_link->au_buffer, &krad_link->au_framecnt);
      }
    }
  }

  return 0;
}

void audio_encoding_unit_destroy (void *arg) {
  
  krad_link_t *krad_link = (krad_link_t *)arg;

  krad_mixer_portgroup_destroy (krad_link->krad_radio->krad_mixer, krad_link->mixer_portgroup);
  
  int c;
  //unsigned char *vorbis_buffer;
  //int bytes;
  //int frames;
  
  if (krad_link->krad_vorbis != NULL) {
  
    krad_vorbis_encoder_finish (krad_link->krad_vorbis);
/*
    // DUPEY
    bytes = krad_vorbis_encoder_read (krad_link->krad_vorbis, &frames, &vorbis_buffer);
    while (bytes > 0) {
      krad_ringbuffer_write (krad_link->encoded_audio_ringbuffer, (char *)&bytes, 4);
      krad_ringbuffer_write (krad_link->encoded_audio_ringbuffer, (char *)&frames, 4);
      krad_ringbuffer_write (krad_link->encoded_audio_ringbuffer, (char *)vorbis_buffer, bytes);
      bytes = krad_vorbis_encoder_read (krad_link->krad_vorbis, &frames, &vorbis_buffer);
    }
*/
    krad_vorbis_encoder_destroy (krad_link->krad_vorbis);
    krad_link->krad_vorbis = NULL;
  }
  
  if (krad_link->krad_flac != NULL) {
    krad_flac_encoder_destroy (krad_link->krad_flac);
    krad_link->krad_flac = NULL;
  }

  if (krad_link->krad_opus != NULL) {
    krad_opus_encoder_destroy (krad_link->krad_opus);
    krad_link->krad_opus = NULL;
  }
  
  close (krad_link->socketpair[0]);
  close (krad_link->socketpair[1]);
  
  for (c = 0; c < krad_link->channels; c++) {
    free (krad_link->samples[c]);
    free (krad_link->au_samples[c]);
    krad_ringbuffer_free (krad_link->audio_input_ringbuffer[c]);    
  }  
  
  free (krad_link->au_interleaved_samples);

  if (krad_link->codec != VORBIS) {
    free (krad_link->au_buffer);
  }
  printk ("Audio encoding thread exiting");
}

void muxer_unit_create (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  //krad_system_set_thread_name ("kr_stream_out");
  
  printk ("Output/Muxing thread starting");

  if (krad_link->host[0] != '\0') {
  
    if ((strcmp(krad_link->host, "transmitter") == 0) &&
      (krad_link->krad_transponder->krad_transmitter->listening == 1)) {
      
      krad_link->muxer_krad_transmission = krad_transmitter_transmission_create (krad_link->krad_transponder->krad_transmitter,
                                    krad_link->mount + 1,
                                    krad_link_select_mimetype(krad_link->mount + 1));

      krad_link->port = krad_link->krad_transponder->krad_transmitter->port;

      krad_link->krad_container = krad_container_open_transmission (krad_link->muxer_krad_transmission);
  
    } else {
  
      krad_link->krad_container = krad_container_open_stream (krad_link->host,
                                  krad_link->port,
                                  krad_link->mount,
                                  krad_link->password);
    }                                  
  } else {
    printk ("Outputing to file: %s", krad_link->output);
    krad_link->krad_container = krad_container_open_file (krad_link->output, KRAD_IO_WRITEONLY);
  }

  printk ("Muxing setup.");
}

static int connect_muxer_to_encoders (krad_link_t *link) {

  char *pch;
  char *save;
  int t;
  int conns;
  krad_codec_header_t *codec_header;
  int track_num;

  track_num = 0;
  codec_header = NULL;
  conns = 0;
	krad_Xtransponder_subunit_t *subunit;
  t = 0;
  save = NULL;

  pch = strtok_r (link->input, "/, ", &save);
  while (pch != NULL) {
    t = atoi (pch);
    subunit = krad_Xtransponder_get_subunit (link->krad_transponder->krad_Xtransponder, t);
    printke ("its %d--", t);
    if (subunit != NULL) {
      krad_Xtransponder_subunit_connect (link->subunit, subunit);
      conns++;
      codec_header = krad_Xtransponder_get_header (subunit);
      if (codec_header == NULL) {
        //FIXME this isn't exactly solid evidence
        track_num = krad_container_add_video_track (link->krad_container,
                                              VP8,
                                              link->encoding_fps_numerator,
                                              link->encoding_fps_denominator,
                                              link->encoding_width,
                                              link->encoding_height);
      } else {
        if (krad_codec_is_video(codec_header->codec)) {
          if (codec_header->codec == THEORA) {
            track_num = krad_container_add_video_track_with_private_data (link->krad_container,
                                                                          codec_header,
                                                                          link->encoding_fps_numerator,
                                                                          link->encoding_fps_denominator,
                                                                          link->encoding_width,
                                                                          link->encoding_height);
          }
        } else {
          if (krad_codec_is_audio(codec_header->codec)) {
            track_num = krad_container_add_audio_track (link->krad_container,
                                                        codec_header->codec,
                                                        link->krad_radio->krad_mixer->sample_rate,
                                                        2, 
                                                        codec_header);
          }
        }
      }
      link->track_sources[track_num] = subunit;
    }
    pch = strtok_r (NULL, "/ ", &save);
  }
  
  return conns;
}

int muxer_unit_process (void *arg) {

  krad_link_t *link = (krad_link_t *)arg;

  krad_slice_t *krad_slice;
  krad_slice = NULL;
  int t;
  
  krad_slice = krad_Xtransponder_get_slice (link->subunit);

  if (krad_slice == NULL) {
    return 1;
  }

  for (t = 0; t < 10; t++) {
    if (link->track_sources[t] == krad_slice->origin) {
      break;
    }
  }
  if (t == 10) {
    return 1;
  }
  
  
  if (krad_codec_is_video(krad_slice->codec)) {
    krad_container_add_video (link->krad_container, 
                              t,
                              krad_slice->data,
                              krad_slice->size,
                              krad_slice->keyframe);
  } else {
    if (krad_codec_is_audio(krad_slice->codec)) {
      krad_container_add_audio (link->krad_container,
                                t,
                                krad_slice->data,
                                krad_slice->size,
                                krad_slice->frames);
    }
  }
  krad_slice_unref (krad_slice);
  
  return 0;
}

void muxer_unit_destroy (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  krad_container_destroy (krad_link->krad_container);

  if (krad_link->muxer_krad_transmission != NULL) {
    krad_transmitter_transmission_destroy (krad_link->muxer_krad_transmission);
  }

  printk ("Muxer Destroyed");
}

void demuxer_unit_create (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  krad_system_set_thread_name ("kr_stream_in");

  printk ("Input/Demuxing thread starting");

  krad_link->demux_header_buffer = malloc (4096 * 512);
  krad_link->demux_buffer = malloc (4096 * 2048);
  
  krad_link->demux_video_packets = 0;
  krad_link->demux_audio_packets = 0;  
  krad_link->demux_current_track = -1;  
  
  if (krad_link->host[0] != '\0') {
    krad_link->krad_container = krad_container_open_stream (krad_link->host, krad_link->port, krad_link->mount, NULL);
  } else {
    krad_link->krad_container = krad_container_open_file (krad_link->input, KRAD_IO_READONLY);
  }
}

int demuxer_unit_process (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  int packet_size;
  uint64_t timecode;
  int header_size;
  int h;
  int total_header_size;
  int writeheaders;
  krad_slice_t *krad_slice;
  
  krad_slice = NULL;
  packet_size = 0;
  header_size = 0;
  total_header_size = 0;  
  writeheaders = 0;

  packet_size = krad_container_read_packet ( krad_link->krad_container,
                                             &krad_link->demux_current_track,
                                             &timecode,
                                             krad_link->demux_buffer);
  //printk ("packet track %d timecode: %zu size %d", current_track, timecode, packet_size);
  if ((packet_size <= 0) && (timecode == 0) &&
      ((krad_link->demux_video_packets + krad_link->demux_audio_packets) > 20))  {
    //printk ("stream input thread packet size was: %d", packet_size);
    return 1;
  }

  if (krad_container_track_changed (krad_link->krad_container, krad_link->demux_current_track)) {
    printk ("track %d changed! status is %d header count is %d",
            krad_link->demux_current_track, krad_container_track_active(krad_link->krad_container, krad_link->demux_current_track),
            krad_container_track_header_count(krad_link->krad_container, krad_link->demux_current_track));

    krad_link->demux_track_codecs[krad_link->demux_current_track] = krad_container_track_codec (krad_link->krad_container, krad_link->demux_current_track);

    if (krad_link->demux_track_codecs[krad_link->demux_current_track] == NOCODEC) {
      return 1;
    }
    writeheaders = 1;
    for (h = 0; h < krad_container_track_header_count (krad_link->krad_container, krad_link->demux_current_track); h++) {
      printk ("header %d is %d bytes", h, krad_container_track_header_size (krad_link->krad_container, krad_link->demux_current_track, h));
      total_header_size += krad_container_track_header_size (krad_link->krad_container, krad_link->demux_current_track, h);
    }
  }

  if ((krad_link->demux_track_codecs[krad_link->demux_current_track] == Y4M) ||
      (krad_link->demux_track_codecs[krad_link->demux_current_track] == KVHS) ||
      (krad_link->demux_track_codecs[krad_link->demux_current_track] == VP8) ||
      (krad_link->demux_track_codecs[krad_link->demux_current_track] == THEORA)) {

    krad_link->demux_video_packets++;
  }


  if ((krad_link->demux_track_codecs[krad_link->demux_current_track] == VORBIS) ||
      (krad_link->demux_track_codecs[krad_link->demux_current_track] == OPUS) ||
      (krad_link->demux_track_codecs[krad_link->demux_current_track] == FLAC)) {

    krad_link->demux_audio_packets++;
  }

  if (writeheaders == 1) {
    for (h = 0; h < krad_container_track_header_count (krad_link->krad_container, krad_link->demux_current_track); h++) {
      header_size = krad_container_track_header_size (krad_link->krad_container, krad_link->demux_current_track, h);
      krad_container_read_track_header (krad_link->krad_container, krad_link->demux_header_buffer, krad_link->demux_current_track, h);
      if (krad_link->subunit != NULL) {
        krad_slice = krad_slice_create_with_data (krad_link->demux_header_buffer, header_size);
        krad_slice->frames = 0;
        krad_slice->header = h + 1;
        krad_slice->codec = krad_link->demux_track_codecs[krad_link->demux_current_track];
        krad_Xtransponder_slice_broadcast (krad_link->subunit, &krad_slice);
        krad_slice_unref (krad_slice);
      }
    }
  }
  if (krad_link->subunit != NULL) {
    krad_slice = krad_slice_create_with_data (krad_link->demux_buffer, packet_size);
    krad_slice->frames = 1;
    krad_slice->timecode = timecode;
    krad_slice->codec = krad_link->demux_track_codecs[krad_link->demux_current_track];
    krad_Xtransponder_slice_broadcast (krad_link->subunit, &krad_slice);
    krad_slice_unref (krad_slice);
  }

  return 0;  
}

void demuxer_unit_destroy (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  printk ("Input/Demuxing thread exiting");

  krad_container_destroy (krad_link->krad_container);

  free (krad_link->demux_buffer);
  free (krad_link->demux_header_buffer);

}

void video_decoding_unit_create (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;
  
  krad_system_set_thread_name ("kr_video_dec");

  printk ("Video decoding thread starting");

  int h;
  
  for (h = 0; h < 3; h++) {
    krad_link->vu_header[h] = malloc(100000);
    krad_link->vu_header_len[h] = 0;
  }
  
  krad_link->last_video_codec = NOCODEC;
  krad_link->video_codec = NOCODEC;
  
  krad_link->krad_framepool = krad_framepool_create ( krad_link->composite_width,
                            krad_link->composite_height,
                            DEFAULT_CAPTURE_BUFFER_FRAMES);
  
  krad_link->krad_compositor_port = krad_compositor_port_create (krad_link->krad_radio->krad_compositor,
                                   "VidDecIn",
                                   INPUT,
                                   krad_link->composite_width,
                                   krad_link->composite_height);
}


int video_decoding_unit_process (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;
  
  uint64_t timecode;
  uint64_t timecode2;
  krad_frame_t *krad_frame;
  krad_slice_t *krad_slice;
  int port_updated;
  int h;
  
  timecode = 0;
  port_updated = 0;
  krad_slice = NULL;
  
  krad_slice = krad_Xtransponder_get_slice (krad_link->subunit);
  if (krad_slice == NULL) {
    return 1;
  }

  krad_link->video_codec = krad_slice->codec;

  if ((krad_link->last_video_codec != krad_link->video_codec) || (krad_link->video_codec == NOCODEC)) {
    printk ("video codec is %d", krad_link->video_codec);
    if (krad_link->last_video_codec != NOCODEC)  {
      if (krad_link->last_video_codec == VP8) {
        krad_vpx_decoder_destroy (krad_link->krad_vpx_decoder);
        krad_link->krad_vpx_decoder = NULL;
      }
      if (krad_link->last_video_codec == KVHS) {
        krad_vhs_destroy (krad_link->krad_vhs);
        krad_link->krad_vhs = NULL;
      }        
      if (krad_link->last_video_codec == THEORA) {
        krad_theora_decoder_destroy (krad_link->krad_theora_decoder);
        krad_link->krad_theora_decoder = NULL;
      }
    }

    if (krad_link->video_codec == NOCODEC) {
      krad_link->last_video_codec = krad_link->video_codec;
      return 1;
    }

    if (krad_link->video_codec == VP8) {
      krad_link->krad_vpx_decoder = krad_vpx_decoder_create();
      port_updated = 0;
    }
    
    if (krad_link->video_codec == Y4M) {
      //krad_link->krad_vpx_decoder = krad_vpx_decoder_create();
      port_updated = 0;
    }
    
    if (krad_link->video_codec == KVHS) {
      krad_link->krad_vhs = krad_vhs_create_decoder ();
      port_updated = 0;
    }            

    if (krad_link->video_codec == THEORA) {
      
      for (h = 0; h < 3; h++) {


  
        //krad_ringbuffer_read(krad_link->encoded_video_ringbuffer, (char *)krad_link->vu_header[h], krad_link->vu_header_len[h]);
      }

      printk ("Theora Header byte sizes: %d %d %d", krad_link->vu_header_len[0], krad_link->vu_header_len[1], krad_link->vu_header_len[2]);
      krad_link->krad_theora_decoder = krad_theora_decoder_create(krad_link->vu_header[0], krad_link->vu_header_len[0], krad_link->vu_header[1], krad_link->vu_header_len[1], krad_link->vu_header[2], krad_link->vu_header_len[2]);

      krad_compositor_port_set_io_params (krad_link->krad_compositor_port,
                        krad_link->krad_theora_decoder->width,
                        krad_link->krad_theora_decoder->height);
      port_updated = 1;                          
    }
  }

  krad_link->last_video_codec = krad_link->video_codec;
  
  krad_frame = krad_framepool_getframe (krad_link->krad_framepool);
  while (krad_frame == NULL) {
    usleep(10000);
    krad_frame = krad_framepool_getframe (krad_link->krad_framepool);        
  }
  
  if (krad_link->video_codec == THEORA) {
    krad_theora_decoder_decode (krad_link->krad_theora_decoder, krad_slice->data, krad_slice->size);    
    krad_theora_decoder_timecode (krad_link->krad_theora_decoder, &timecode2);      
    //printk ("timecode1: %zu timecode2: %zu", timecode, timecode2);
    timecode = timecode2;

    krad_frame->format = krad_link->krad_theora_decoder->color_depth;

    krad_frame->yuv_pixels[0] = krad_link->krad_theora_decoder->ycbcr[0].data + (krad_link->krad_theora_decoder->offset_y * krad_link->krad_theora_decoder->ycbcr[0].stride);
    krad_frame->yuv_pixels[1] = krad_link->krad_theora_decoder->ycbcr[1].data + (krad_link->krad_theora_decoder->offset_y * krad_link->krad_theora_decoder->ycbcr[1].stride);
    krad_frame->yuv_pixels[2] = krad_link->krad_theora_decoder->ycbcr[2].data + (krad_link->krad_theora_decoder->offset_y * krad_link->krad_theora_decoder->ycbcr[2].stride);

    krad_frame->yuv_strides[0] = krad_link->krad_theora_decoder->ycbcr[0].stride;
    krad_frame->yuv_strides[1] = krad_link->krad_theora_decoder->ycbcr[1].stride;
    krad_frame->yuv_strides[2] = krad_link->krad_theora_decoder->ycbcr[2].stride;
    krad_frame->timecode = timecode;
    krad_compositor_port_push_yuv_frame (krad_link->krad_compositor_port, krad_frame);
  }
    
  if (krad_link->video_codec == VP8) {

    krad_vpx_decoder_decode (krad_link->krad_vpx_decoder, krad_slice->data, krad_slice->size);
      
    if (krad_link->krad_vpx_decoder->img != NULL) {
      
      if (port_updated == 0) {
        krad_compositor_port_set_io_params (krad_link->krad_compositor_port,
                          krad_link->krad_vpx_decoder->width,
                          krad_link->krad_vpx_decoder->height);
        port_updated = 1;
      }

      krad_frame->format = PIX_FMT_YUV420P;

      krad_frame->yuv_pixels[0] = krad_link->krad_vpx_decoder->img->planes[0];
      krad_frame->yuv_pixels[1] = krad_link->krad_vpx_decoder->img->planes[1];
      krad_frame->yuv_pixels[2] = krad_link->krad_vpx_decoder->img->planes[2];

      krad_frame->yuv_strides[0] = krad_link->krad_vpx_decoder->img->stride[0];
      krad_frame->yuv_strides[1] = krad_link->krad_vpx_decoder->img->stride[1];
      krad_frame->yuv_strides[2] = krad_link->krad_vpx_decoder->img->stride[2];
      krad_frame->timecode = krad_slice->timecode;
      krad_compositor_port_push_yuv_frame (krad_link->krad_compositor_port, krad_frame);
    }
  }
  
  if (krad_link->video_codec == KVHS) {

    krad_vhs_decode (krad_link->krad_vhs, krad_slice->data, (unsigned char *)krad_frame->pixels);  
      
    if (krad_link->krad_vhs->width != 0) {
      if (port_updated == 0) {
        printk ("got vhs res: %dx%d", krad_link->krad_vhs->width, krad_link->krad_vhs->height);
        krad_compositor_port_set_io_params (krad_link->krad_compositor_port,
                          krad_link->krad_vhs->width,
                          krad_link->krad_vhs->height);
                                
        port_updated = 1;
      }

      krad_frame->format = PIX_FMT_RGB32;
      krad_frame->timecode = timecode;
      krad_compositor_port_push_rgba_frame (krad_link->krad_compositor_port, krad_frame);
    }
  }
  
  if (krad_link->video_codec == Y4M) {
    //fixme
    //if (port_updated == 0) {
    //  krad_compositor_port_set_io_params (krad_link->krad_compositor_port,
    //                    krad_link->krad_vhs->width,
    //                    krad_link->krad_vhs->height);
    //                          
    //  port_updated = 1;
    //}
    krad_frame->timecode = timecode;
    krad_compositor_port_push_frame (krad_link->krad_compositor_port, krad_frame);    
  }    

  krad_framepool_unref_frame (krad_frame);
  return 0;
}
  
void video_decoding_unit_destroy (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  int h;

  krad_compositor_port_destroy (krad_link->krad_radio->krad_compositor, krad_link->krad_compositor_port);
  
  if (krad_link->krad_vpx_decoder != NULL) {
    krad_vpx_decoder_destroy(krad_link->krad_vpx_decoder);
    krad_link->krad_vpx_decoder = NULL;
  }
  if (krad_link->krad_vhs != NULL) {
    krad_vhs_destroy (krad_link->krad_vhs);
    krad_link->krad_vhs = NULL;
  }
  if (krad_link->krad_theora_decoder != NULL) {
    krad_theora_decoder_destroy (krad_link->krad_theora_decoder);
    krad_link->krad_theora_decoder = NULL;
  }
  for (h = 0; h < 3; h++) {
    free (krad_link->vu_header[h]);
  }
  printk ("Video decoding unit exiting");
}

void audio_decoding_unit_create (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  krad_system_set_thread_name ("kr_audio_dec");

  printk ("Audio decoding thread starting");

  int c;
  int h;

  /* SET UP */
  
  krad_link->channels = 2;

  for (h = 0; h < 3; h++) {
    krad_link->au_header[h] = malloc(100000);
    krad_link->au_header_len[h] = 0;
  }

  for (c = 0; c < krad_link->channels; c++) {
    krad_link->krad_resample_ring[c] = krad_resample_ring_create (4000000, 48000,
                               krad_link->krad_transponder->krad_radio->krad_mixer->sample_rate);

    krad_link->audio_output_ringbuffer[c] = krad_link->krad_resample_ring[c]->krad_ringbuffer;
    //krad_link->audio_output_ringbuffer[c] = krad_ringbuffer_create (4000000);
    krad_link->au_samples[c] = malloc(4 * 8192);
    krad_link->samples[c] = malloc(4 * 8192);
  }

  krad_link->au_audio = calloc(1, 8192 * 4 * 4);
  
  krad_link->last_audio_codec = NOCODEC;
  krad_link->audio_codec = NOCODEC;
  
  krad_link->mixer_portgroup = krad_mixer_portgroup_create (krad_link->krad_radio->krad_mixer, krad_link->sysname, INPUT, NOTOUTPUT, 2, 
                           0.0f, krad_link->krad_radio->krad_mixer->master_mix, KRAD_LINK, krad_link, 0);
  
  krad_mixer_set_portgroup_control (krad_link->krad_radio->krad_mixer, krad_link->sysname, "volume", 100.0f, 500, NULL);
  
}

int audio_decoding_unit_process (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;
  
  int c;
  int bytes;
  int len;
  krad_slice_t *krad_slice;

  /* THE FOLLOWING IS WHERE WE ENSURE WE ARE ON THE RIGHT CODEC AND READ HEADERS IF NEED BE */

  krad_slice = NULL;
  
  krad_slice = krad_Xtransponder_get_slice (krad_link->subunit);
  if (krad_slice == NULL) {
    return 1;
  }

  krad_link->audio_codec = krad_slice->codec;

  if ((krad_link->last_audio_codec != krad_link->audio_codec) || (krad_link->audio_codec == NOCODEC)) {
    printk ("audio codec is %d", krad_link->audio_codec);
    if (krad_link->last_audio_codec != NOCODEC)  {
      if (krad_link->last_audio_codec == FLAC) {
        krad_flac_decoder_destroy (krad_link->krad_flac);
        krad_link->krad_flac = NULL;
      }
      if (krad_link->last_audio_codec == VORBIS) {
        krad_vorbis_decoder_destroy (krad_link->krad_vorbis);
        krad_link->krad_vorbis = NULL;
      }      
      if (krad_link->last_audio_codec == OPUS) {
        krad_opus_decoder_destroy (krad_link->krad_opus);
        krad_link->krad_opus = NULL;
      }
    }

    if (krad_link->audio_codec == NOCODEC) {
      krad_link->last_audio_codec = krad_link->audio_codec;
      return 1;
    }

    if (krad_link->audio_codec == FLAC) {
      krad_link->krad_flac = krad_flac_decoder_create();
      // get flac header
      krad_flac_decode (krad_link->krad_flac, krad_link->au_header[0], krad_link->au_header_len[0], NULL);
      for (c = 0; c < krad_link->channels; c++) {
        krad_resample_ring_set_input_sample_rate (krad_link->krad_resample_ring[c], krad_link->krad_flac->sample_rate);
      }
    }

    if (krad_link->audio_codec == VORBIS) {
       // get vorbis headers krad_link->au_header[h], krad_link->au_header_len[h]);
      printk ("Vorbis Header byte sizes: %d %d %d", krad_link->au_header_len[0], krad_link->au_header_len[1], krad_link->au_header_len[2]);
      krad_link->krad_vorbis = krad_vorbis_decoder_create (krad_link->au_header[0], krad_link->au_header_len[0], krad_link->au_header[1], krad_link->au_header_len[1], krad_link->au_header[2], krad_link->au_header_len[2]);

      for (c = 0; c < krad_link->channels; c++) {
        krad_resample_ring_set_input_sample_rate (krad_link->krad_resample_ring[c], krad_link->krad_vorbis->sample_rate);
      }
    }

    if (krad_link->audio_codec == OPUS) {
      // get opus header krad_link->au_header[h], krad_link->au_header_len[h]);
      printk ("Opus Header size: %d", krad_link->au_header_len[0]);
      krad_link->krad_opus = krad_opus_decoder_create (krad_link->au_header[0], krad_link->au_header_len[0], krad_link->krad_radio->krad_mixer->sample_rate);
      for (c = 0; c < krad_link->channels; c++) {
        krad_resample_ring_set_input_sample_rate (krad_link->krad_resample_ring[c], krad_link->krad_radio->krad_mixer->sample_rate);
      }
    }
  }

  krad_link->last_audio_codec = krad_link->audio_codec;

  /* DECODING HAPPENS HERE */
  
  if (krad_link->audio_codec == VORBIS) {
    krad_vorbis_decoder_decode (krad_link->krad_vorbis, krad_slice->data, krad_slice->size);
    len = 1;
    while (len ) {
      len = krad_vorbis_decoder_read_audio(krad_link->krad_vorbis, 0, (char *)krad_link->au_samples[0], 512);
      if (len) {
        while (krad_resample_ring_write_space (krad_link->krad_resample_ring[0]) < len) {
          usleep(25000);
        }
        krad_resample_ring_write (krad_link->krad_resample_ring[0], (unsigned char *)krad_link->au_samples[0], len);                    
      }
      len = krad_vorbis_decoder_read_audio (krad_link->krad_vorbis, 1, (char *)krad_link->au_samples[1], 512);
      if (len) {
        while (krad_resample_ring_write_space (krad_link->krad_resample_ring[1]) < len) {
          //printk ("wait!");
          usleep(25000);
        }
        krad_resample_ring_write (krad_link->krad_resample_ring[1], (unsigned char *)krad_link->au_samples[1], len);          
      }
    }
  }

  if (krad_link->audio_codec == FLAC) {
    len = krad_flac_decode (krad_link->krad_flac, krad_slice->data, krad_slice->size, krad_link->au_samples);
    for (c = 0; c < krad_link->channels; c++) {
      krad_resample_ring_write (krad_link->krad_resample_ring[c], (unsigned char *)krad_link->au_samples[c], len * 4);
    }
  }
    
  if (krad_link->audio_codec == OPUS) {
    krad_opus_decoder_write (krad_link->krad_opus, krad_slice->data, krad_slice->size);
    bytes = -1;
    while (bytes != 0) {
      for (c = 0; c < 2; c++) {
        bytes = krad_opus_decoder_read (krad_link->krad_opus, c + 1, (char *)krad_link->au_audio, 120 * 4);
        if (bytes) {
          if ((bytes / 4) != 120) {
            failfast ("uh oh crazyto");
          }
          while (krad_resample_ring_write_space (krad_link->krad_resample_ring[c]) < bytes) {
              usleep(20000);
          }
          //krad_ringbuffer_write (krad_link->audio_output_ringbuffer[c], (char *)audio, bytes);
          krad_resample_ring_write (krad_link->krad_resample_ring[c], (unsigned char *)krad_link->au_audio, bytes);
        }
      }
    }
  }

  krad_slice_unref (krad_slice);

  return 0;
}

void audio_decoding_unit_destroy (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;
  
  int c;
  int h;
  
  /* ITS ALL OVER */
  
  krad_mixer_portgroup_destroy (krad_link->krad_radio->krad_mixer, krad_link->mixer_portgroup);

  if (krad_link->krad_vorbis != NULL) {
    krad_vorbis_decoder_destroy (krad_link->krad_vorbis);
    krad_link->krad_vorbis = NULL;
  }
  
  if (krad_link->krad_flac != NULL) {
    krad_flac_decoder_destroy (krad_link->krad_flac);
    krad_link->krad_flac = NULL;
  }

  if (krad_link->krad_opus != NULL) {
    krad_opus_decoder_destroy(krad_link->krad_opus);
    krad_link->krad_opus = NULL;
  }

  free (krad_link->au_audio);
  
  for (c = 0; c < krad_link->channels; c++) {
    free(krad_link->samples[c]);
    free(krad_link->au_samples[c]);
    krad_resample_ring_destroy ( krad_link->krad_resample_ring[c] );    
  }  

  for (h = 0; h < 3; h++) {
    free (krad_link->au_header[h]);
  }

  printk ("Audio decoding unit exiting");
}

void krad_link_audio_samples_callback (int frames, void *userdata, float **samples) {

  krad_link_t *link = (krad_link_t *)userdata;
  
  int c;
  int wrote;
  
  /*
  if ((krad_link->type == RECEIVE) || (krad_link->type == PLAYBACK)) {
    if (((krad_link->av_mode == AUDIO_ONLY) || (1)) && 
       (krad_ringbuffer_read_space (krad_link->audio_output_ringbuffer[0]) >= frames * 4) && 
       (krad_ringbuffer_read_space (krad_link->audio_output_ringbuffer[1]) >= frames * 4)) {
         krad_ringbuffer_read (krad_link->audio_output_ringbuffer[0], (char *)samples[0], frames * 4);
        krad_ringbuffer_read (krad_link->audio_output_ringbuffer[1], (char *)samples[1], frames * 4);
    } else {
      memset(samples[0], '0', frames * 4);
      memset(samples[1], '0', frames * 4);
    }
  }
  */

  if (link->type == ENCODE) {
    for (c = 0; c < link->channels; c++ ) {
      if (krad_ringbuffer_write_space (link->audio_input_ringbuffer[c]) < frames * 4) {
        return;
      }
    }

    for (c = 0; c < link->channels; c++ ) {
      krad_ringbuffer_write (link->audio_input_ringbuffer[c], (char *)samples[c], frames * 4);
    }

    wrote = write (link->socketpair[0], "0", 1);
    if (wrote != 1) {
      printk ("Krad Transponder: au port write unexpected write return value %d", wrote);
    }
  }
  /*
  if (krad_link->type == CAPTURE) {

    krad_ringbuffer_read (krad_link->audio_capture_ringbuffer[0], (char *)samples[0], frames * 4);
    krad_ringbuffer_read (krad_link->audio_capture_ringbuffer[1], (char *)samples[1], frames * 4);
  }
  */
}

krad_tags_t *krad_link_get_tags (krad_link_t *krad_link) {
  return krad_link->krad_tags;
}

void krad_link_destroy (krad_link_t *krad_link) {

  printk ("Link shutting down");

  krad_Xtransponder_subunit_remove (krad_link->krad_transponder->krad_Xtransponder, krad_link->graph_id);
  
  if (krad_link->krad_framepool != NULL) {
    krad_framepool_destroy (&krad_link->krad_framepool);
  }

  krad_tags_destroy (krad_link->krad_tags);  
  
  printk ("Krad Link Closed Clean");
  
  free (krad_link);
}

krad_link_t *krad_link_prepare (int linknum) {

  krad_link_t *krad_link;
  
  krad_link = calloc (1, sizeof(krad_link_t));

  krad_link->capture_buffer_frames = DEFAULT_CAPTURE_BUFFER_FRAMES;
  krad_link->vp8_bitrate = DEFAULT_VPX_BITRATE;
  strncpy (krad_link->device, DEFAULT_V4L2_DEVICE, sizeof(krad_link->device));
  krad_link->vorbis_quality = DEFAULT_VORBIS_QUALITY;
  krad_link->flac_bit_depth = KRAD_DEFAULT_FLAC_BIT_DEPTH;
  krad_link->opus_bitrate = KRAD_DEFAULT_OPUS_BITRATE;
  krad_link->theora_quality = DEFAULT_THEORA_QUALITY;
  krad_link->video_source = NOVIDEO;
  krad_link->transport_mode = TCP;

  sprintf (krad_link->sysname, "link%d", linknum);
  krad_link->krad_tags = krad_tags_create (krad_link->sysname);

  return krad_link;
}

void krad_link_start (krad_link_t *link) {

  krad_transponder_watch_t watch;
  
  memset (&watch, 0, sizeof(krad_transponder_watch_t));
  watch.callback_pointer = link;
  
  if ((link->encoding_fps_numerator == 0) || (link->encoding_fps_denominator == 0)) {
    krad_compositor_get_frame_rate (link->krad_radio->krad_compositor,
                                    &link->encoding_fps_numerator,
                                    &link->encoding_fps_denominator);
  }

  if ((link->encoding_width == 0) || (link->encoding_height == 0)) {
    krad_compositor_get_resolution (link->krad_radio->krad_compositor,
                                    &link->encoding_width,
                                    &link->encoding_height);
  }
  
  krad_compositor_get_resolution (link->krad_radio->krad_compositor,
                                  &link->composite_width,
                                  &link->composite_height);

  switch ( link->type ) {
    case MUX:
      muxer_unit_create (link);
      watch.readable_callback = muxer_unit_process;
      watch.destroy_callback = muxer_unit_destroy;
      link->graph_id = krad_Xtransponder_add_muxer (link->krad_transponder->krad_Xtransponder, &watch);
      break;
    case ENCODE:
      if (link->av_mode == AUDIO_ONLY) {
        audio_encoding_unit_create (link);
        watch.fd = link->socketpair[1];
        watch.readable_callback = audio_encoding_unit_process;
        watch.encoder_header_callback = audio_encoding_unit_get_header;
        watch.destroy_callback = audio_encoding_unit_destroy;
      } else {
        video_encoding_unit_create (link);
        watch.fd = link->krad_compositor_port_fd;
        watch.encoder_header_callback = video_encoding_unit_get_header;
        watch.readable_callback = video_encoding_unit_process;
        watch.destroy_callback = video_encoding_unit_destroy;
      }
      link->graph_id = krad_Xtransponder_add_encoder (link->krad_transponder->krad_Xtransponder, &watch);
      break;  
    case RAWOUT:
#ifdef KRAD_USE_WAYLAND
      wayland_display_unit_create (link);
      watch.fd = link->krad_wayland->display_fd;
      watch.readable_callback = wayland_display_unit_process;
      watch.destroy_callback = wayland_display_unit_destroy;
      link->graph_id = krad_Xtransponder_add_capture (link->krad_transponder->krad_Xtransponder, &watch);      
#endif
      break;
    case RAWIN:
      switch ( link->video_source ) {
        case NOVIDEO:
          return;
        case X11:
          x11_capture_unit_create (link);
          watch.idle_callback_interval = 5;
          watch.readable_callback = x11_capture_unit_process;
          watch.destroy_callback = x11_capture_unit_destroy;
          break;
#ifdef KR_LINUX
        case V4L2:
          v4l2_capture_unit_create (link);
          watch.fd = link->krad_v4l2->fd;
          watch.readable_callback = v4l2_capture_unit_process;
          watch.destroy_callback = v4l2_capture_unit_destroy;
          break; 
#endif
        case DECKLINK:
          decklink_capture_unit_create (link);
          watch.readable_callback = NULL;
          watch.destroy_callback = decklink_capture_unit_destroy;
          break;
      }
      link->graph_id = krad_Xtransponder_add_capture (link->krad_transponder->krad_Xtransponder, &watch);
      break;
    default:
      return;
  }
  
  link->subunit = krad_Xtransponder_get_subunit (link->krad_transponder->krad_Xtransponder, link->graph_id);
  
  if (link->type == MUX) {
    connect_muxer_to_encoders (link);
  }
}

krad_link_t *krad_transponder_get_link_from_sysname (krad_transponder_t *krad_transponder, char *sysname) {

  int i;
  krad_link_t *krad_link;

  for (i = 0; i < KRAD_TRANSPONDER_MAX_SUBUNITS; i++) {
    krad_link = krad_transponder->krad_link[i];
    if (krad_link != NULL) {
      if (strcmp(sysname, krad_link->sysname) == 0) {
        return krad_link;
      }
    }
  }

  return NULL;
}

krad_tags_t *krad_transponder_get_tags_for_link (krad_transponder_t *krad_transponder, char *sysname) {

  krad_link_t *krad_link;
  
  krad_link = krad_transponder_get_link_from_sysname (krad_transponder, sysname);

  if (krad_link != NULL) {
    return krad_link_get_tags (krad_link);
  } else {
    return NULL;
  }
}


krad_transponder_t *krad_transponder_create (krad_radio_t *krad_radio) {

  krad_transponder_t *krad_transponder;
  
  krad_transponder = calloc (1, sizeof(krad_transponder_t));

  krad_transponder->address.path.unit = KR_TRANSPONDER;
  krad_transponder->address.path.subunit.mixer_subunit = KR_UNIT;

  krad_transponder->krad_radio = krad_radio;
  krad_transponder->krad_receiver = krad_receiver_create (krad_transponder);  
  krad_transponder->krad_transmitter = krad_transmitter_create ();
  krad_transponder->krad_Xtransponder = krad_Xtransponder_create (krad_transponder->krad_radio);

  return krad_transponder;

}

void krad_transponder_destroy (krad_transponder_t *krad_transponder) {

  int l;
  
  printk ("Krad Transponder: Destroy Started");

  for (l = 0; l < KRAD_TRANSPONDER_MAX_SUBUNITS; l++) {
    if (krad_transponder->krad_link[l] != NULL) {
      krad_link_destroy (krad_transponder->krad_link[l]);
      krad_transponder->krad_link[l] = NULL;
    }
  }

  krad_receiver_destroy (krad_transponder->krad_receiver);
  krad_transmitter_destroy (krad_transponder->krad_transmitter);
  krad_Xtransponder_destroy (&krad_transponder->krad_Xtransponder);

  free (krad_transponder);
  
  printk ("Krad Transponder: Destroy Completed");

}

