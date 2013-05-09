#include "krad_vpx.h"

static void krad_vpx_fail (vpx_codec_ctx_t *ctx, const char *s);

krad_vpx_encoder_t *krad_vpx_encoder_create (int width, int height,
                                             int fps_numerator,
                                             int fps_denominator,
                                             int bitrate) {

  krad_vpx_encoder_t *vpx;
  
  vpx = calloc (1, sizeof(krad_vpx_encoder_t));
  
  vpx->width = width;
  vpx->height = height;
  vpx->fps_numerator = fps_numerator;
  vpx->fps_denominator = fps_denominator;
  vpx->bitrate = bitrate;
  
  printk ("Krad Radio using libvpx version: %s",
          vpx_codec_version_str ());

  vpx->res = vpx_codec_enc_config_default (vpx_codec_vp8_cx(),
                                           &vpx->cfg, 0);

  if (vpx->res) {
    failfast ("Failed to get config: %s\n",
              vpx_codec_err_to_string(vpx->res));
  }

  // print default config
  krad_vpx_encoder_print_config (vpx);

  //TEMP
  //vpx->cfg.g_lag_in_frames = 1;

  vpx->cfg.g_w = vpx->width;
  vpx->cfg.g_h = vpx->height;
  /* Next two lines are really right */
  vpx->cfg.g_timebase.num = vpx->fps_denominator;
  vpx->cfg.g_timebase.den = vpx->fps_numerator;
  vpx->cfg.rc_target_bitrate = bitrate;  
  vpx->cfg.g_threads = 4;
  vpx->cfg.kf_mode = VPX_KF_AUTO;
  vpx->cfg.rc_end_usage = VPX_VBR;
  
  vpx->cfg.kf_max_dist = 120;
  
  vpx->deadline = 15 * 1000;

  vpx->min_quantizer = vpx->cfg.rc_min_quantizer;
  vpx->max_quantizer = vpx->cfg.rc_max_quantizer;

  //krad_vpx_encoder_print_config (vpx);

  if (vpx_codec_enc_init (&vpx->encoder,
                          vpx_codec_vp8_cx(),
                          &vpx->cfg, 0)) {
    krad_vpx_fail (&vpx->encoder, "Failed to initialize encoder");
  }

  krad_vpx_encoder_print_config (vpx);

  //vpx_codec_control (&vpx->encoder, VP8E_SET_ENABLEAUTOALTREF, 1);

  return vpx;
}

void krad_vpx_encoder_print_config (krad_vpx_encoder_t *vpx) {
  printk ("Krad VP8 Encoder config");
  printk ("WxH %dx%d", vpx->cfg.g_w, vpx->cfg.g_h);
  printk ("Threads: %d", vpx->cfg.g_threads);
  printk ("rc_target_bitrate: %d", vpx->cfg.rc_target_bitrate);
  printk ("kf_max_dist: %d", vpx->cfg.kf_max_dist);
  printk ("kf_min_dist: %d", vpx->cfg.kf_min_dist);  
  printk ("deadline: %d", vpx->deadline);  
  printk ("rc_buf_sz: %d", vpx->cfg.rc_buf_sz);  
  printk ("rc_buf_initial_sz: %d", vpx->cfg.rc_buf_initial_sz);  
  printk ("rc_buf_optimal_sz: %d", vpx->cfg.rc_buf_optimal_sz);
  printk ("rc_dropframe_thresh: %d", vpx->cfg.rc_dropframe_thresh);
  printk ("g_lag_in_frames: %d", vpx->cfg.g_lag_in_frames);
  printk ("g_timebase.num: %d", vpx->cfg.g_timebase.num);
  printk ("g_timebase.den: %d", vpx->cfg.g_timebase.den);

  printk ("END Krad VP8 Encoder config");
}

int krad_vpx_encoder_bitrate_get (krad_vpx_encoder_t *vpx) {
  return vpx->bitrate;
}

void krad_vpx_encoder_bitrate_set (krad_vpx_encoder_t *vpx, int bitrate) {
  vpx->bitrate = bitrate;
  vpx->cfg.rc_target_bitrate = vpx->bitrate;
  vpx->update_config = 1;
}

int krad_vpx_encoder_min_quantizer_get (krad_vpx_encoder_t *vpx) {
  return vpx->min_quantizer;
}

void krad_vpx_encoder_min_quantizer_set (krad_vpx_encoder_t *vpx,
                                         int min_quantizer) {
  vpx->min_quantizer = min_quantizer;
  vpx->cfg.rc_min_quantizer = vpx->min_quantizer;
  vpx->update_config = 1;
}

void krad_vpx_encoder_set_kf_max_dist (krad_vpx_encoder_t *vpx,
                                       uint32_t max_dist) {
  vpx->cfg.kf_max_dist = max_dist;
  vpx->update_config = 1;
}

int krad_vpx_encoder_max_quantizer_get (krad_vpx_encoder_t *vpx) {
  return vpx->max_quantizer;
}

void krad_vpx_encoder_max_quantizer_set (krad_vpx_encoder_t *vpx,
                                         int max_quantizer) {
  vpx->max_quantizer = max_quantizer;
  vpx->cfg.rc_max_quantizer = vpx->max_quantizer;
  vpx->update_config = 1;
}

void krad_vpx_encoder_deadline_set (krad_vpx_encoder_t *vpx, int deadline) {
  vpx->deadline = deadline;
}

int krad_vpx_encoder_deadline_get (krad_vpx_encoder_t *vpx) {
  return vpx->deadline;
}

void krad_vpx_encoder_config_set (krad_vpx_encoder_t *vpx,
                                  vpx_codec_enc_cfg_t *cfg) {
  int ret;

  ret = vpx_codec_enc_config_set (&vpx->encoder, cfg);

  if (ret != VPX_CODEC_OK) {
    printke ("VPX Config problem: %s\n", vpx_codec_err_to_string (ret));
  }
}

int32_t krad_vpx_encoder_destroy (krad_vpx_encoder_t **vpx) {
  if ((vpx != NULL) && (*vpx != NULL)) {
    vpx_codec_destroy (&(*vpx)->encoder);
    free (*vpx);
    *vpx = NULL;
    return 0;
  }
  return -1;
}

void krad_vpx_encoder_finish (krad_vpx_encoder_t *vpx) {
  if (vpx->image != NULL) {
    vpx_img_free (vpx->image);
    vpx->image = NULL;
  }
}

void krad_vpx_encoder_want_keyframe (krad_vpx_encoder_t *vpx) {
  vpx->flags = VPX_EFLAG_FORCE_KF;
}

int krad_vpx_encoder_write (krad_vpx_encoder_t *vpx,
                            unsigned char **packet,
                            int *keyframe) {

  if (vpx->update_config == 1) {
    krad_vpx_encoder_config_set (vpx, &vpx->cfg);
    vpx->update_config = 0;
    krad_vpx_encoder_print_config (vpx);
  }

  if (vpx_codec_encode (&vpx->encoder, vpx->image, vpx->frames,
                        1, vpx->flags, vpx->deadline)) {
    krad_vpx_fail (&vpx->encoder, "Failed to encode frame");
  }

  vpx->frames++;

  vpx->flags = 0;
  
  vpx->iter = NULL;
  while ((vpx->pkt = vpx_codec_get_cx_data (&vpx->encoder,
                                                &vpx->iter))) {
    //printkd ("Got packet\n");
    if (vpx->pkt->kind == VPX_CODEC_CX_FRAME_PKT) {
      *packet = vpx->pkt->data.frame.buf;
      *keyframe = vpx->pkt->data.frame.flags & VPX_FRAME_IS_KEY;
      if (*keyframe == 0) {
        vpx->frames_since_keyframe++;
      } else {
        vpx->frames_since_keyframe = 0;
        //printkd ("keyframe is %d pts is -%ld-\n",
        // *keyframe, vpx->pkt->data.frame.pts);
      }
      vpx->bytes += vpx->pkt->data.frame.sz;
      return vpx->pkt->data.frame.sz;
    }
  }
  return 0;
}

int32_t kr_vpx_encode (krad_vpx_encoder_t *vpx,
                       kr_codeme_t *codeme,
                       kr_medium_t *medium) {

  int ret;
  vpx_image_t image;
  memset (&image, 0, sizeof(vpx_image_t));
  vpx_codec_iter_t iter;  
  //vpx_codec_err_t err;
  //vpx_codec_cx_pkt_t *pkt;

  ret = 0;

  if (vpx->update_config == 1) {
    krad_vpx_encoder_config_set (vpx, &vpx->cfg);
    vpx->update_config = 0;
    krad_vpx_encoder_print_config (vpx);
  }

  if (medium == NULL) {
    ret = vpx_codec_encode (&vpx->encoder, NULL, vpx->frames,
                            1, vpx->flags, vpx->deadline);
  } else {

    vpx_img_wrap (&image, VPX_IMG_FMT_I420,
                  vpx->width, vpx->height, 1,
                  medium->data);

    //image.w = vpx->width;
    //image.h = vpx->height;

    //image.d_w = vpx->width;
    //image.d_h = vpx->height;

    //image.planes[0] = medium->v.ppx[0];
    //image.planes[1] = medium->v.ppx[1];
    //image.planes[2] = medium->v.ppx[2];
    
    image.stride[0] = medium->v.pps[0];
    image.stride[1] = medium->v.pps[1];
    image.stride[2] = medium->v.pps[2];
    
    ret = vpx_codec_encode (&vpx->encoder, &image, vpx->frames,
                            1, vpx->flags, vpx->deadline);
  }

  if (ret != 0) {
    printke ("oh shit");
  }

  vpx->frames++;
  vpx->flags = 0;
  
  iter = NULL;
  vpx->pkt = vpx_codec_get_cx_data (&vpx->encoder, &iter);
  
  if (vpx->pkt != NULL) {
    if (vpx->pkt->kind == VPX_CODEC_CX_FRAME_PKT) {
      codeme->sz = vpx->pkt->data.frame.sz;
      memcpy (codeme->data, vpx->pkt->data.frame.buf, codeme->sz);
      codeme->key = vpx->pkt->data.frame.flags & VPX_FRAME_IS_KEY;
      if (codeme->key == 0) {
        vpx->frames_since_keyframe++;
      } else {
        vpx->frames_since_keyframe = 0;
      }
      return 1;
    }
  }
  return ret;
}

/* decoder */

int32_t kr_vpx_decode (krad_vpx_decoder_t *vpx,
                       kr_medium_t *medium,
                       kr_codeme_t *codeme) {

  if (vpx_codec_decode (&vpx->decoder, codeme->data, codeme->sz, 0, 0)) {
    printf ("Failed to decode %zu byte frame: %s\n",
              codeme->sz, vpx_codec_error (&vpx->decoder));
  }

  if (vpx->width == 0) {

    vpx_codec_get_stream_info (&vpx->decoder, &vpx->stream_info);

    printk ("VPX Stream Info: W:%d H:%d KF:%d\n",
            vpx->stream_info.w,
            vpx->stream_info.h, 
            vpx->stream_info.is_kf);

    vpx->width = vpx->stream_info.w;
    vpx->height = vpx->stream_info.h;
  }

  vpx->iter = NULL;
  vpx->img = vpx_codec_get_frame (&vpx->decoder, &vpx->iter);

  if (vpx->img != NULL) {
  
    medium->v.pps[0] = vpx->width;
    medium->v.pps[1] = vpx->width/2;  
    medium->v.pps[2] = vpx->width/2;
      
  //printf ("VPXDEC: s %u %u %u", vpx->img->stride[0], vpx->img->stride[1], vpx->img->stride[2]);
  //memcpy (medium->data, vpx->img->img_data, vpx->img->stride[0] * vpx->height + (vpx->img->stride[1] * vpx->height * 2));
  int r;
  for (r = 0; r< vpx->height; r++) {
    memcpy (medium->data + (r * vpx->width), vpx->img->planes[0] + (r * vpx->img->stride[0]), vpx->width);
  }
  for (r = 0; r< vpx->height/2; r++) {
    memcpy (medium->data + (vpx->width * (vpx->height)) + (r * vpx->width/2), vpx->img->planes[1] + (r * vpx->img->stride[1]), vpx->width / 2);
  }
  for (r = 0; r< vpx->height/2; r++) {
    memcpy (medium->data + (vpx->width * (vpx->height)) + ((vpx->width * (vpx->height))/4) + (r * vpx->width/2),
            vpx->img->planes[2] + (r * vpx->img->stride[2]), vpx->width / 2);
  }  

      //memcpy (medium->data + vpx->img->stride[0] * vpx->height, vpx->img->planes[1], vpx->img->stride[1] * vpx->height / 2);
      //memcpy (medium->data + (vpx->img->stride[0] * vpx->height) + (vpx->img->stride[1] * (vpx->height /2)), vpx->img->planes[2], vpx->img->stride[2] * vpx->height / 2);    

    medium->v.ppx[0] = medium->data;
    medium->v.ppx[1] = medium->data + vpx->width * (vpx->height);  
    medium->v.ppx[2] = medium->data + vpx->width * (vpx->height) + ((vpx->width * (vpx->height)) /4);


    return 1;
  }

  return 0;
}

void krad_vpx_decoder_decode (krad_vpx_decoder_t *vpx,
                              void *buffer,
                              int len) {

  if (vpx_codec_decode (&vpx->decoder, buffer, len, 0, 0)) {
    printf ("Failed to decode %d byte frame: %s\n",
              len, vpx_codec_error (&vpx->decoder));
  }

  if (vpx->width == 0) {

    vpx_codec_get_stream_info (&vpx->decoder, &vpx->stream_info);

    printf ("VPX Stream Info: W:%d H:%d KF:%d\n",
            vpx->stream_info.w,
            vpx->stream_info.h, 
            vpx->stream_info.is_kf);

    vpx->width = vpx->stream_info.w;
    vpx->height = vpx->stream_info.h;
  }

  vpx->iter = NULL;
  vpx->img = vpx_codec_get_frame (&vpx->decoder, &vpx->iter);
}

void krad_vpx_decoder_decode_again (krad_vpx_decoder_t *vpx) {
  vpx->img = vpx_codec_get_frame (&vpx->decoder, &vpx->iter);
}


int32_t krad_vpx_decoder_destroy (krad_vpx_decoder_t **vpx) {
  if ((vpx != NULL) && (*vpx != NULL)) {
    vpx_codec_destroy (&(*vpx)->decoder);
    free (*vpx);
    *vpx = NULL;
    return 0;
  }
  return -1;
}

krad_vpx_decoder_t *krad_vpx_decoder_create () {

  krad_vpx_decoder_t *vpx;
  
  vpx = calloc (1, sizeof(krad_vpx_decoder_t));

  vpx->stream_info.sz = sizeof (vpx->stream_info);
  vpx->dec_flags = 0;
  vpx->cfg.threads = 3;
  
  vpx_codec_dec_init (&vpx->decoder,
                     vpx_codec_vp8_dx(),
                     &vpx->cfg,
                     vpx->dec_flags);

  //vpx->ppcfg.post_proc_flag = VP8_DEBLOCK;
  //vpx->ppcfg.deblocking_level = 1;
  //vpx->ppcfg.noise_level = 0;

  vpx->ppcfg.post_proc_flag = VP8_DEMACROBLOCK | VP8_DEBLOCK | VP8_ADDNOISE;
  vpx->ppcfg.deblocking_level = 5;
  vpx->ppcfg.noise_level = 1;

  vpx_codec_control (&vpx->decoder, VP8_SET_POSTPROC, &vpx->ppcfg);

  vpx->img = NULL;

  return vpx;
}

static void krad_vpx_fail (vpx_codec_ctx_t *ctx, const char *msg) {
    
  const char *err_details;
  
  err_details = vpx_codec_error_detail (ctx);
  
  if (err_details == NULL) {
    err_details = "None";
  }

  printf ("Krad VPX Fail: %s %s\nDetail: %s",
            msg, vpx_codec_error (ctx), err_details);
}
