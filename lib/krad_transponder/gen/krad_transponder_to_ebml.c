#include "krad_transponder_to_ebml.h"

int kr_adapter_path_direction_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_adapter_path_direction *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_adapter_path_direction *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_adapter_api_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_adapter_api *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_adapter_api *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_adapter_api_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  uber_St *uber_actual;

  kr_adapter_api_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  uber_actual = (uber_St *)st;

  if (uber_actual->actual == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_info *)uber_actual->actual;

  switch (uber_actual->type) {
    case 0: {
      uber.actual = &(actual->jack);
      uber.type = EBML_KR_JACK_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 1: {
      uber.actual = &(actual->wayland);
      uber.type = EBML_KR_WAYLAND_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 2: {
      uber.actual = &(actual->v4l2);
      uber.type = EBML_KR_V4L2_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 3: {
      uber.actual = &(actual->decklink);
      uber.type = EBML_KR_DECKLINK_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 4: {
      uber.actual = &(actual->x11);
      uber.type = EBML_KR_X11_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
  }


  return res;
}

int kr_adapter_api_path_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  uber_St *uber_actual;

  kr_adapter_api_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  uber_actual = (uber_St *)st;

  if (uber_actual->actual == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_path_info *)uber_actual->actual;

  switch (uber_actual->type) {
    case 0: {
      uber.actual = &(actual->jack);
      uber.type = EBML_KR_JACK_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 1: {
      uber.actual = &(actual->wayland);
      uber.type = EBML_KR_WAYLAND_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 2: {
      uber.actual = &(actual->v4l2);
      uber.type = EBML_KR_V4L2_OPEN_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 3: {
      uber.actual = &(actual->decklink);
      uber.type = EBML_KR_DECKLINK_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 4: {
      uber.actual = &(actual->x11);
      uber.type = EBML_KR_X11_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 5: {
      uber.actual = &(actual->alsa);
      uber.type = EBML_KR_ALSA_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
  }


  return res;
}

int kr_adapter_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_adapter_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_adapter_info *)st;

  uber.actual = &(actual->api);
  uber.type = EBML_KR_ADAPTER_API;
  res += info_pack_to_ebml(&ebml[res],&uber);
  index = kr_adapter_api_to_index(actual->api);
  uber_sub.type = index;
  uber_sub.actual = &(actual->api_info);
  uber.actual = &(uber_sub);
  uber.type = EBML_KR_ADAPTER_API_INFO;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_adapter_path_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_adapter_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_adapter_path_info *)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->name);
  uber.actual = &(actual->dir);
  uber.type = EBML_KR_ADAPTER_PATH_DIRECTION;
  res += info_pack_to_ebml(&ebml[res],&uber);
  uber.actual = &(actual->api);
  uber.type = EBML_KR_ADAPTER_API;
  res += info_pack_to_ebml(&ebml[res],&uber);
  index = kr_adapter_api_to_index(actual->api);
  uber_sub.type = index;
  uber_sub.actual = &(actual->info);
  uber.actual = &(uber_sub);
  uber.type = EBML_KR_ADAPTER_API_PATH_INFO;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_stat_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_stat_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_stat_St *)st;

  res += kr_ebml_pack_uint64(ebml, 0xe1, actual->time_last);
  res += kr_ebml_pack_uint64(ebml, 0xe1, actual->time_total);
  res += kr_ebml_pack_uint64(ebml, 0xe1, actual->started_on);

  return res;
}

int krad_link_av_mode_t_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  krad_link_av_mode_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (krad_link_av_mode_t *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_txpdr_su_type_t_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_txpdr_su_type_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_txpdr_su_type_t *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int krad_link_transport_mode_t_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  krad_link_transport_mode_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (krad_link_transport_mode_t *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int krad_link_video_source_t_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  krad_link_video_source_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (krad_link_video_source_t *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int krad_transponder_rep_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct krad_transponder_rep_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct krad_transponder_rep_St *)st;


  return res;
}

int kr_flac_encoder_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_flac_encoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_flac_encoder_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->bit_depth);

  return res;
}

int kr_vpx_decoder_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_vpx_decoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_vpx_decoder_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->yea);

  return res;
}

int kr_theora_decoder_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_theora_decoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_theora_decoder_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->yea);

  return res;
}

int kr_daala_decoder_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_daala_decoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_daala_decoder_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->lol);

  return res;
}

int kr_vorbis_decoder_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_vorbis_decoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_vorbis_decoder_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->yea);

  return res;
}

int kr_opus_decoder_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_opus_decoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_opus_decoder_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->yea);

  return res;
}

int kr_flac_decoder_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_flac_decoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_flac_decoder_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->bit_depth);

  return res;
}

int kr_vpx_encoder_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_vpx_encoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_vpx_encoder_St *)st;

  res += kr_ebml_pack_uint64(ebml, 0xe1, actual->deadline);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->bitrate);

  return res;
}

int kr_theora_encoder_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_theora_encoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_theora_encoder_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->kf_distance);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->speed);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->quality);

  return res;
}

int kr_daala_encoder_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_daala_encoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_daala_encoder_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->lol);

  return res;
}

int kr_vorbis_encoder_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_vorbis_encoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_vorbis_encoder_St *)st;

  res += kr_ebml_pack_float(ebml, 0xe1, actual->quality);

  return res;
}

int kr_opus_encoder_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_opus_encoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_opus_encoder_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->bandwidth);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->signal);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->bitrate);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->complexity);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->frame_size);

  return res;
}

int kr_audio_codec_t_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  uber_St *uber_actual;

  kr_audio_codec_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  uber_actual = (uber_St *)st;

  if (uber_actual->actual == NULL) {
    return -1;
  }

  actual = (kr_audio_codec_t *)uber_actual->actual;

  switch (uber_actual->type) {
  }


  return res;
}

int kr_video_codec_t_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  uber_St *uber_actual;

  kr_video_codec_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  uber_actual = (uber_St *)st;

  if (uber_actual->actual == NULL) {
    return -1;
  }

  actual = (kr_video_codec_t *)uber_actual->actual;

  switch (uber_actual->type) {
  }


  return res;
}

int kr_audio_encoder_St_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_audio_encoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_audio_encoder_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->sample_rate);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->channels);
  res += kr_ebml_pack_uint64(ebml, 0xe1, actual->bytes);
  res += kr_ebml_pack_uint64(ebml, 0xe1, actual->frames);
  uber.actual = &(actual->codec);
  uber.type = EBML_KR_AUDIO_CODEC_T;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_video_decoder_St_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_video_decoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_video_decoder_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->width);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->height);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->fps_numerator);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->fps_denominator);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->color_depth);
  uber.actual = &(actual->codec);
  uber.type = EBML_KR_VIDEO_CODEC_T;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_audio_decoder_St_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_audio_decoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_audio_decoder_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->sample_rate);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->channels);
  uber.actual = &(actual->codec);
  uber.type = EBML_KR_AUDIO_CODEC_T;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_video_encoder_St_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_video_encoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_video_encoder_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->width);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->height);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->fps_numerator);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->fps_denominator);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->color_depth);
  uber.actual = &(actual->codec);
  uber.type = EBML_KR_VIDEO_CODEC_T;
  res += info_pack_to_ebml(&ebml[res],&uber);
  res += kr_ebml_pack_uint64(ebml, 0xe1, actual->bytes);
  res += kr_ebml_pack_uint64(ebml, 0xe1, actual->frames);

  return res;
}

int kr_av_encoder_t_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  uber_St *uber_actual;

  kr_av_encoder_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  uber_actual = (uber_St *)st;

  if (uber_actual->actual == NULL) {
    return -1;
  }

  actual = (kr_av_encoder_t *)uber_actual->actual;

  switch (uber_actual->type) {
  }


  return res;
}

int kr_encoder_St_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_encoder_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_encoder_St *)st;

  uber.actual = &(actual->codec);
  uber.type = EBML_KRAD_CODEC_T;
  res += info_pack_to_ebml(&ebml[res],&uber);
  index = krad_codec_t_to_index(actual->codec);
  uber_sub.type = index;
  uber_sub.actual = &(actual->av);
  uber.actual = &(uber_sub);
  uber.type = EBML_KR_AV_ENCODER_T;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_av_decoder_t_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  uber_St *uber_actual;

  kr_av_decoder_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  uber_actual = (uber_St *)st;

  if (uber_actual->actual == NULL) {
    return -1;
  }

  actual = (kr_av_decoder_t *)uber_actual->actual;

  switch (uber_actual->type) {
  }


  return res;
}

int kr_udp_muxer_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_udp_muxer_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_udp_muxer_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->ok);

  return res;
}

int kr_ogg_muxer_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_ogg_muxer_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_ogg_muxer_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->ok);

  return res;
}

int kr_mkv_muxer_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_mkv_muxer_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_mkv_muxer_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->ok);

  return res;
}

int kr_transogg_muxer_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_transogg_muxer_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_transogg_muxer_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->ok);

  return res;
}

int kr_udp_demuxer_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_udp_demuxer_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_udp_demuxer_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->ok);

  return res;
}

int kr_ogg_demuxer_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_ogg_demuxer_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_ogg_demuxer_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->ok);

  return res;
}

int kr_mkv_demuxer_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_mkv_demuxer_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_mkv_demuxer_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->ok);

  return res;
}

int kr_transogg_demuxer_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_transogg_demuxer_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_transogg_demuxer_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->ok);

  return res;
}

int kr_muxer_actual_t_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  uber_St *uber_actual;

  kr_muxer_actual_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  uber_actual = (uber_St *)st;

  if (uber_actual->actual == NULL) {
    return -1;
  }

  actual = (kr_muxer_actual_t *)uber_actual->actual;

  switch (uber_actual->type) {
  }


  return res;
}

int kr_muxer_St_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_muxer_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_muxer_St *)st;

  uber.actual = &(actual->container);
  uber.type = EBML_KR_MUXER_ACTUAL_T;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_demuxer_actual_t_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  uber_St *uber_actual;

  kr_demuxer_actual_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  uber_actual = (uber_St *)st;

  if (uber_actual->actual == NULL) {
    return -1;
  }

  actual = (kr_demuxer_actual_t *)uber_actual->actual;

  switch (uber_actual->type) {
  }


  return res;
}

int kr_rawin_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_rawin_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_rawin_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->yea);

  return res;
}

int kr_rawout_St_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_rawout_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_rawout_St *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->yea);

  return res;
}

int kr_transponder_subunit_actual_t_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  uber_St *uber_actual;

  kr_transponder_subunit_actual_t *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  uber_actual = (uber_St *)st;

  if (uber_actual->actual == NULL) {
    return -1;
  }

  actual = (kr_transponder_subunit_actual_t *)uber_actual->actual;

  switch (uber_actual->type) {
  }


  return res;
}

int krad_transponder_subunit_rep_St_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct krad_transponder_subunit_rep_St *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct krad_transponder_subunit_rep_St *)st;

  uber.actual = &(actual->type);
  uber.type = EBML_KR_TXPDR_SU_TYPE_T;
  res += info_pack_to_ebml(&ebml[res],&uber);
  index = kr_txpdr_su_type_t_to_index(actual->type);
  uber_sub.type = index;
  uber_sub.actual = &(actual->actual);
  uber.actual = &(uber_sub);
  uber.type = EBML_KR_TRANSPONDER_SUBUNIT_ACTUAL_T;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_transponder_path_io_type_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_transponder_path_io_type *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_transponder_path_io_type *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_transponder_info_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_transponder_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_transponder_info *)st;

  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->active_paths);

  return res;
}

int kr_transponder_path_io_path_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  uber_St *uber_actual;

  kr_transponder_path_io_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  uber_actual = (uber_St *)st;

  if (uber_actual->actual == NULL) {
    return -1;
  }

  actual = (kr_transponder_path_io_path_info *)uber_actual->actual;

  switch (uber_actual->type) {
    case 0: {
      uber.actual = &(actual->mixer_path_info);
      uber.type = EBML_KR_MIXER_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 1: {
      uber.actual = &(actual->compositor_path_info);
      uber.type = EBML_KR_COMPOSITOR_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
    case 2: {
      uber.actual = &(actual->adapter_path_info);
      uber.type = EBML_KR_ADAPTER_PATH_INFO;
      res += info_pack_to_ebml(&ebml[res],&uber);
      break;
    }
  }


  return res;
}

int kr_transponder_path_io_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_transponder_path_io_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_transponder_path_io_info *)st;

  uber.actual = &(actual->type);
  uber.type = EBML_KR_TRANSPONDER_PATH_IO_TYPE;
  res += info_pack_to_ebml(&ebml[res],&uber);
  index = kr_transponder_path_io_type_to_index(actual->type);
  uber_sub.type = index;
  uber_sub.actual = &(actual->info);
  uber.actual = &(uber_sub);
  uber.type = EBML_KR_TRANSPONDER_PATH_IO_PATH_INFO;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_transponder_path_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  struct kr_transponder_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_transponder_path_info *)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->name);
  uber.actual = &(actual->input);
  uber.type = EBML_KR_TRANSPONDER_PATH_IO_INFO;
  res += info_pack_to_ebml(&ebml[res],&uber);
  uber.actual = &(actual->output);
  uber.type = EBML_KR_TRANSPONDER_PATH_IO_INFO;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

