/*
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
  kr_mixer_input_setup audio_out;
  int c;

  krad_system_set_thread_name("kr_decklink");
  krad_link->krad_decklink = krad_decklink_create (krad_link->device);

  krad_decklink_set_video_mode(krad_link->krad_decklink,
                                krad_link->capture_width, krad_link->capture_height,
                                60000, 1001);


  krad_decklink_set_audio_input(krad_link->krad_decklink, krad_link->audio_input);
  krad_decklink_set_video_input(krad_link->krad_decklink, "hdmi");

  krad_link->krad_decklink->callback_pointer = krad_link;
  krad_link->krad_decklink->audio_frames_callback = krad_link_decklink_audio_callback;
  krad_link->krad_decklink->video_frame_callback = krad_link_decklink_video_callback;
  krad_decklink_start(krad_link->krad_decklink);
}

void decklink_capture_unit_destroy (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;
  int c;

  if (krad_link->krad_decklink != NULL) {
    krad_decklink_destroy(krad_link->krad_decklink);
    krad_link->krad_decklink = NULL;
  }
}
*/
