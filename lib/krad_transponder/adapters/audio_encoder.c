void audio_encoding_unit_create (void *arg) {

  //krad_system_set_thread_name ("kr_audio_enc");

  krad_link_t *krad_link = (krad_link_t *)arg;

  int c;

  printk ("Audio unit create");

  if (krad_link->codec != VORBIS) {
    krad_link->au_buffer = malloc (300000);
  }

  krad_link->au_interleaved_samples = malloc (8192 * 4 * KR_MXR_MAX_CHANNELS);

  for (c = 0; c < krad_link->channels; c++) {
    krad_link->au_samples[c] = malloc (8192 * 4);
    krad_link->samples[c] = malloc (8192 * 4);
    krad_link->audio_input_ringbuffer[c] = krad_ringbuffer_create (2000000);
  }

  if (socketpair(AF_UNIX, SOCK_STREAM, 0, krad_link->socketpair)) {
    printk ("Krad Compositor: subunit could not create socketpair errno: %d", errno);
    return;
  }

//  krad_link->mixer_portgroup = kr_mixer_path_create(krad_link->krad_radio->mixer,
//   krad_link->sysname, KR_AOUT, 0, krad_link->channels, 0.0f,
//   krad_link->krad_radio->mixer->master, 0, krad_link, 0);

  switch (krad_link->codec) {
    case VORBIS:
      krad_link->krad_vorbis = krad_vorbis_encoder_create (krad_link->channels,
                                 krad_link->krad_radio->mixer->sample_rate,
                                 krad_link->vorbis_quality);
      //krad_link->au_framecnt = KRAD_DEFAULT_VORBIS_FRAME_SIZE;
      break;
    case FLAC:
      krad_link->krad_flac = krad_flac_encoder_create (krad_link->channels,
                               krad_link->krad_radio->mixer->sample_rate,
                               krad_link->flac_bit_depth);
      krad_link->au_framecnt = KRAD_DEFAULT_FLAC_FRAME_SIZE;
      break;
    case OPUS:
      krad_link->krad_opus = krad_opus_encoder_create (krad_link->channels,
                               krad_link->krad_radio->mixer->sample_rate,
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
      return &krad_link->krad_vorbis->header;
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
  int s;
  int bytes;
  int frames;
  int ret;
  char buffer[1];
  kr_slice_t *kr_slice;

  bytes = 0;
  kr_slice = NULL;

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
    /*


      medium = kr_medium_kludge_create ();
      codeme = kr_codeme_kludge_create ();


      for (c = 0; c < krad_link->channels; c++) {
        krad_ringbuffer_read (krad_link->audio_input_ringbuffer[c], (char *)float_buffer[c], krad_link->au_framecnt * 4);
      }

                ret = kr_vorbis_encode (vorbis_enc, codeme, medium);
          if (ret == 1) {
            kr_mkv_add_audio (new_mkv, 2, codeme->data, codeme->sz, codeme->count);
          }
          kr_medium_kludge_destroy (&medium);
          kr_codeme_kludge_destroy (&codeme);

      bytes =
    */
    }

    while (bytes > 0) {
      if (krad_link->subunit != NULL) {
        kr_slice = kr_slice_create_with_data (krad_link->au_buffer, bytes);
        kr_slice->frames = frames;
        kr_slice->codec = krad_link->codec;
        kr_xpdr_slice_broadcast (krad_link->subunit, &kr_slice);
        kr_slice_unref (kr_slice);
      }
      bytes = 0;
      if (krad_link->codec == VORBIS) {
      //  bytes = krad_vorbis_encoder_read (krad_link->krad_vorbis, &frames, &krad_link->au_buffer);
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

  kr_mixer_path_unlink(krad_link->krad_radio->mixer, krad_link->mixer_portgroup);

  int c;
  //unsigned char *vorbis_buffer;
  //int bytes;
  //int frames;

  if (krad_link->krad_vorbis != NULL) {

//    krad_vorbis_encoder_finish (krad_link->krad_vorbis);
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
    krad_vorbis_encoder_destroy (&krad_link->krad_vorbis);
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
