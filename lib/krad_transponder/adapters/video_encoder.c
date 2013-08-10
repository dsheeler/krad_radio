#define DEFAULT_VPX_BITRATE 1000
#define DEFAULT_THEORA_QUALITY 31

int video_encoding_unit_process (void *arg) {

  krad_link_t *krad_link = (krad_link_t *)arg;

  int ret;
  char buffer[1];
  krad_frame_t *krad_frame;
  kr_slice_t *kr_slice;
  void *video_packet;
  int keyframe;
  int packet_size;
  unsigned char *planes[3];
  int strides[3];

  packet_size = 0;
  krad_frame = NULL;
  kr_slice = NULL;

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
        kr_slice = kr_slice_create_with_data (krad_link->krad_vhs->enc_buffer, packet_size);
        kr_slice->frames = 1;
        kr_slice->codec = krad_link->codec;
        kr_slice->keyframe = 1;
        kr_xpdr_slice_broadcast (krad_link->subunit, &kr_slice);
        kr_slice_unref (kr_slice);
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
            kr_slice = kr_slice_create_with_data (video_packet, packet_size);
            kr_slice->frames = 1;
            kr_slice->codec = krad_link->codec;
            kr_slice->keyframe = keyframe;
            kr_xpdr_slice_broadcast (krad_link->subunit, &kr_slice);
            kr_slice_unref (kr_slice);
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

  krad_compositor_port_destroy (krad_link->krad_radio->compositor, krad_link->krad_compositor_port);

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
    krad_vpx_encoder_destroy (&krad_link->krad_vpx_encoder);
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
