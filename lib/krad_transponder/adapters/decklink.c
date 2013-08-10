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

void mixer_input_setup_defaults(kr_mixer_input_setup *input,
 krad_link_t *link) {
  memset(input, 0, sizeof(kr_mixer_input_setup));
  input->info.type = KR_MXR_INPUT;
  strncpy(input->info.name, link->sysname, sizeof(input->info.name));
  input->info.channels = 2;
  input->info.volume[0] = 90.0f;
  input->info.volume[1] = 90.0f;
  input->user = link;
  input->cb = link; //FIXME callback
}

void decklink_capture_unit_create (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;
  kr_mixer_input_setup audio_out;
  int c;

  krad_system_set_thread_name ("kr_decklink");
  mixer_input_setup_defaults(&audio_out, krad_link);
  krad_link->krad_decklink = krad_decklink_create (krad_link->device);

  if ((krad_link->fps_numerator == 0) || (krad_link->fps_denominator == 0)) {
    krad_compositor_get_frame_rate (krad_link->krad_radio->compositor,
                                    &krad_link->fps_numerator,
                                    &krad_link->fps_denominator);
  }

  if ((krad_link->capture_width == 0) || (krad_link->capture_height == 0)) {
    krad_compositor_get_resolution (krad_link->krad_radio->compositor,
                                    &krad_link->capture_width,
                                    &krad_link->capture_height);
  }

  //krad_decklink_set_video_mode (krad_link->krad_decklink,
  //                              krad_link->capture_width, krad_link->capture_height,
  //                              krad_link->fps_numerator, krad_link->fps_denominator);


  krad_decklink_set_video_mode (krad_link->krad_decklink,
                                krad_link->capture_width, krad_link->capture_height,
                                60000, 1001);


  krad_decklink_set_audio_input (krad_link->krad_decklink, krad_link->audio_input);
  krad_decklink_set_video_input (krad_link->krad_decklink, "hdmi");

  for (c = 0; c < krad_link->channels; c++) {
    krad_link->audio_capture_ringbuffer[c] = krad_ringbuffer_create (1000000);
  }

  krad_link->krad_framepool = krad_framepool_create ( krad_link->capture_width,
                                                      krad_link->capture_height,
                                                      DEFAULT_CAPTURE_BUFFER_FRAMES);

  krad_link->krad_mixer_portgroup = kr_mixer_mkpath(krad_link->krad_radio->mixer,
   &audio_out);

  kr_mixer_ctl(krad_link->krad_radio->mixer,
   krad_link->krad_decklink->simplename, "volume", 100.0f, 500, NULL);

  krad_link->krad_compositor_port = krad_compositor_port_create(krad_link->krad_radio->compositor,
                                                                krad_link->krad_decklink->simplename,
                                                                KR_AIN, krad_link->capture_width,
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

  kr_mixer_path_unlink(krad_link->krad_radio->mixer, krad_link->krad_mixer_portgroup);
  krad_compositor_port_destroy (krad_link->krad_radio->compositor, krad_link->krad_compositor_port);

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
    krad_link->krad_vpx_encoder->cfg.kf_min_dist = 10;
    krad_link->krad_vpx_encoder->cfg.kf_max_dist = 90;
    //}

    //if (krad_link->type == RECORD) {
    //  krad_link->krad_vpx_encoder->cfg.rc_min_quantizer = 5;
    //  krad_link->krad_vpx_encoder->cfg.rc_max_quantizer = 35;
    //}

    krad_vpx_encoder_config_set (krad_link->krad_vpx_encoder, &krad_link->krad_vpx_encoder->cfg);

    krad_vpx_encoder_deadline_set (krad_link->krad_vpx_encoder, 5000);

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

  krad_link->krad_compositor_port = krad_compositor_port_create (krad_link->krad_radio->compositor,
                                                                 "VIDEnc",
                                                                 KR_VOUT,
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
