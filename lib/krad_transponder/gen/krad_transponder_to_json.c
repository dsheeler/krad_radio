#include "krad_transponder_to_json.h"

int kr_adapter_path_direction_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_adapter_path_direction *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_adapter_path_direction *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_adapter_api_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_adapter_api *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_adapter_api *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_adapter_api_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int res;
  uber_St *uber_actual;

  kr_adapter_api_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
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
      uber.type = JSON_KR_JACK_INFO;
      res += info_pack_to_json(&json[res],&uber,max-res);
      break;
    }
    case 1: {
      uber.actual = &(actual->wayland);
      uber.type = JSON_KR_WAYLAND_INFO;
      res += info_pack_to_json(&json[res],&uber,max-res);
      break;
    }
    case 2: {
      uber.actual = &(actual->v4l2);
      uber.type = JSON_KR_V4L2_INFO;
      res += info_pack_to_json(&json[res],&uber,max-res);
      break;
    }
    case 3: {
      uber.actual = &(actual->decklink);
      uber.type = JSON_KR_DECKLINK_INFO;
      res += info_pack_to_json(&json[res],&uber,max-res);
      break;
    }
    case 4: {
      uber.actual = &(actual->x11);
      uber.type = JSON_KR_X11_INFO;
      res += info_pack_to_json(&json[res],&uber,max-res);
      break;
    }
  }


  return res;
}

int kr_adapter_api_path_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int res;
  uber_St *uber_actual;

  kr_adapter_api_path_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
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
      uber.type = JSON_KR_JACK_PATH_INFO;
      res += info_pack_to_json(&json[res],&uber,max-res);
      break;
    }
    case 1: {
      uber.actual = &(actual->wayland);
      uber.type = JSON_KR_WAYLAND_PATH_INFO;
      res += info_pack_to_json(&json[res],&uber,max-res);
      break;
    }
    case 2: {
      uber.actual = &(actual->v4l2);
      uber.type = JSON_KR_V4L2_OPEN_INFO;
      res += info_pack_to_json(&json[res],&uber,max-res);
      break;
    }
    case 3: {
      uber.actual = &(actual->decklink);
      uber.type = JSON_KR_DECKLINK_PATH_INFO;
      res += info_pack_to_json(&json[res],&uber,max-res);
      break;
    }
    case 4: {
      uber.actual = &(actual->x11);
      uber.type = JSON_KR_X11_PATH_INFO;
      res += info_pack_to_json(&json[res],&uber,max-res);
      break;
    }
    case 5: {
      uber.actual = &(actual->alsa);
      uber.type = JSON_KR_ALSA_PATH_INFO;
      res += info_pack_to_json(&json[res],&uber,max-res);
      break;
    }
  }


  return res;
}

int kr_adapter_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_adapter_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_adapter_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"api\": ");
  uber.actual = &(actual->api);
  uber.type = JSON_KR_ADAPTER_API;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  index = kr_adapter_api_to_index(actual->api);
  uber_sub.type = index;
  uber_sub.actual = &(actual->api_info);
  uber.actual = &(uber_sub);
  uber.type = JSON_KR_ADAPTER_API_INFO;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_adapter_path_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_adapter_path_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_adapter_path_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"name\" : \"%s\",",actual->name);
  res += snprintf(&json[res],max-res,"\"dir\": ");
  uber.actual = &(actual->dir);
  uber.type = JSON_KR_ADAPTER_PATH_DIRECTION;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"api\": ");
  uber.actual = &(actual->api);
  uber.type = JSON_KR_ADAPTER_API;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  index = kr_adapter_api_to_index(actual->api);
  uber_sub.type = index;
  uber_sub.actual = &(actual->info);
  uber.actual = &(uber_sub);
  uber.type = JSON_KR_ADAPTER_API_PATH_INFO;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_stat_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_stat_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_stat_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"time_last\" : %ju,",actual->time_last);
  res += snprintf(&json[res],max-res,"\"time_total\" : %ju,",actual->time_total);
  res += snprintf(&json[res],max-res,"\"started_on\" : %ju",actual->started_on);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int krad_link_av_mode_t_to_json(char *json, void *st, int32_t max) {
  int res;
  krad_link_av_mode_t *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (krad_link_av_mode_t *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_txpdr_su_type_t_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_txpdr_su_type_t *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_txpdr_su_type_t *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int krad_link_transport_mode_t_to_json(char *json, void *st, int32_t max) {
  int res;
  krad_link_transport_mode_t *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (krad_link_transport_mode_t *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int krad_link_video_source_t_to_json(char *json, void *st, int32_t max) {
  int res;
  krad_link_video_source_t *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (krad_link_video_source_t *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int krad_transponder_rep_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct krad_transponder_rep_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct krad_transponder_rep_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"receiver_port\" : %u,",actual->receiver_port);
  res += snprintf(&json[res],max-res,"\"transmitter_port\" : %u",actual->transmitter_port);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_flac_encoder_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_flac_encoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_flac_encoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"bit_depth\" : %d",actual->bit_depth);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_vpx_decoder_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_vpx_decoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_vpx_decoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"yea\" : %d",actual->yea);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_theora_decoder_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_theora_decoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_theora_decoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"yea\" : %d",actual->yea);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_daala_decoder_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_daala_decoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_daala_decoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"lol\" : %d",actual->lol);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_vorbis_decoder_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_vorbis_decoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_vorbis_decoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"yea\" : %d",actual->yea);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_opus_decoder_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_opus_decoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_opus_decoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"yea\" : %d",actual->yea);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_flac_decoder_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_flac_decoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_flac_decoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"bit_depth\" : %d",actual->bit_depth);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_vpx_encoder_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_vpx_encoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_vpx_encoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"deadline\" : %ju,",actual->deadline);
  res += snprintf(&json[res],max-res,"\"bitrate\" : %u",actual->bitrate);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_theora_encoder_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_theora_encoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_theora_encoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"kf_distance\" : %d,",actual->kf_distance);
  res += snprintf(&json[res],max-res,"\"speed\" : %d,",actual->speed);
  res += snprintf(&json[res],max-res,"\"quality\" : %d",actual->quality);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_daala_encoder_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_daala_encoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_daala_encoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"lol\" : %d",actual->lol);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_vorbis_encoder_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_vorbis_encoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_vorbis_encoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"quality\" : %0.2f",actual->quality);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_opus_encoder_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_opus_encoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_opus_encoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"bandwidth\" : %d,",actual->bandwidth);
  res += snprintf(&json[res],max-res,"\"signal\" : %d,",actual->signal);
  res += snprintf(&json[res],max-res,"\"bitrate\" : %d,",actual->bitrate);
  res += snprintf(&json[res],max-res,"\"complexity\" : %d,",actual->complexity);
  res += snprintf(&json[res],max-res,"\"frame_size\" : %d",actual->frame_size);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_audio_codec_t_to_json(char *json, void *st, int32_t max) {
  int res;
  uber_St *uber_actual;

  kr_audio_codec_t *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
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

int kr_video_codec_t_to_json(char *json, void *st, int32_t max) {
  int res;
  uber_St *uber_actual;

  kr_video_codec_t *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
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

int kr_audio_encoder_St_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_audio_encoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_audio_encoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"sample_rate\" : %d,",actual->sample_rate);
  res += snprintf(&json[res],max-res,"\"channels\" : %d,",actual->channels);
  res += snprintf(&json[res],max-res,"\"bytes\" : %ju,",actual->bytes);
  res += snprintf(&json[res],max-res,"\"frames\" : %ju,",actual->frames);
  res += snprintf(&json[res],max-res,"\"codec\": ");
  uber.actual = &(actual->codec);
  uber.type = JSON_KR_AUDIO_CODEC_T;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_video_decoder_St_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_video_decoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_video_decoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"width\" : %d,",actual->width);
  res += snprintf(&json[res],max-res,"\"height\" : %d,",actual->height);
  res += snprintf(&json[res],max-res,"\"fps_numerator\" : %d,",actual->fps_numerator);
  res += snprintf(&json[res],max-res,"\"fps_denominator\" : %d,",actual->fps_denominator);
  res += snprintf(&json[res],max-res,"\"color_depth\" : %d,",actual->color_depth);
  res += snprintf(&json[res],max-res,"\"codec\": ");
  uber.actual = &(actual->codec);
  uber.type = JSON_KR_VIDEO_CODEC_T;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_audio_decoder_St_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_audio_decoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_audio_decoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"sample_rate\" : %d,",actual->sample_rate);
  res += snprintf(&json[res],max-res,"\"channels\" : %d,",actual->channels);
  res += snprintf(&json[res],max-res,"\"codec\": ");
  uber.actual = &(actual->codec);
  uber.type = JSON_KR_AUDIO_CODEC_T;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_video_encoder_St_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_video_encoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_video_encoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"width\" : %d,",actual->width);
  res += snprintf(&json[res],max-res,"\"height\" : %d,",actual->height);
  res += snprintf(&json[res],max-res,"\"fps_numerator\" : %d,",actual->fps_numerator);
  res += snprintf(&json[res],max-res,"\"fps_denominator\" : %d,",actual->fps_denominator);
  res += snprintf(&json[res],max-res,"\"color_depth\" : %d,",actual->color_depth);
  res += snprintf(&json[res],max-res,"\"codec\": ");
  uber.actual = &(actual->codec);
  uber.type = JSON_KR_VIDEO_CODEC_T;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"bytes\" : %ju,",actual->bytes);
  res += snprintf(&json[res],max-res,"\"frames\" : %ju",actual->frames);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_av_encoder_t_to_json(char *json, void *st, int32_t max) {
  int res;
  uber_St *uber_actual;

  kr_av_encoder_t *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
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

int kr_encoder_St_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_encoder_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_encoder_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"codec\": ");
  uber.actual = &(actual->codec);
  uber.type = JSON_KRAD_CODEC_T;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  index = krad_codec_t_to_index(actual->codec);
  uber_sub.type = index;
  uber_sub.actual = &(actual->av);
  uber.actual = &(uber_sub);
  uber.type = JSON_KR_AV_ENCODER_T;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_av_decoder_t_to_json(char *json, void *st, int32_t max) {
  int res;
  uber_St *uber_actual;

  kr_av_decoder_t *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
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

int kr_udp_muxer_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_udp_muxer_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_udp_muxer_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"ok\" : %d",actual->ok);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_ogg_muxer_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_ogg_muxer_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_ogg_muxer_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"ok\" : %d",actual->ok);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_mkv_muxer_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_mkv_muxer_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_mkv_muxer_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"ok\" : %d",actual->ok);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_transogg_muxer_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_transogg_muxer_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_transogg_muxer_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"ok\" : %d",actual->ok);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_udp_demuxer_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_udp_demuxer_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_udp_demuxer_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"ok\" : %d",actual->ok);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_ogg_demuxer_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_ogg_demuxer_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_ogg_demuxer_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"ok\" : %d",actual->ok);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_mkv_demuxer_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_mkv_demuxer_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_mkv_demuxer_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"ok\" : %d",actual->ok);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_transogg_demuxer_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_transogg_demuxer_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_transogg_demuxer_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"ok\" : %d",actual->ok);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_muxer_actual_t_to_json(char *json, void *st, int32_t max) {
  int res;
  uber_St *uber_actual;

  kr_muxer_actual_t *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
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

int kr_muxer_St_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_muxer_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_muxer_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"container\": ");
  uber.actual = &(actual->container);
  uber.type = JSON_KR_MUXER_ACTUAL_T;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_demuxer_actual_t_to_json(char *json, void *st, int32_t max) {
  int res;
  uber_St *uber_actual;

  kr_demuxer_actual_t *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
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

int kr_rawin_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_rawin_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_rawin_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"yea\" : %d",actual->yea);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_rawout_St_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_rawout_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_rawout_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"yea\" : %d",actual->yea);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_transponder_subunit_actual_t_to_json(char *json, void *st, int32_t max) {
  int res;
  uber_St *uber_actual;

  kr_transponder_subunit_actual_t *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
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

int krad_transponder_subunit_rep_St_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct krad_transponder_subunit_rep_St *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct krad_transponder_subunit_rep_St *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"type\": ");
  uber.actual = &(actual->type);
  uber.type = JSON_KR_TXPDR_SU_TYPE_T;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  index = kr_txpdr_su_type_t_to_index(actual->type);
  uber_sub.type = index;
  uber_sub.actual = &(actual->actual);
  uber.actual = &(uber_sub);
  uber.type = JSON_KR_TRANSPONDER_SUBUNIT_ACTUAL_T;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_transponder_path_io_type_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_transponder_path_io_type *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_transponder_path_io_type *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_transponder_info_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_transponder_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_transponder_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"active_paths\" : %u",actual->active_paths);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_transponder_path_io_path_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int res;
  uber_St *uber_actual;

  kr_transponder_path_io_path_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
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
      uber.type = JSON_KR_MIXER_PATH_INFO;
      res += info_pack_to_json(&json[res],&uber,max-res);
      break;
    }
    case 1: {
      uber.actual = &(actual->compositor_path_info);
      uber.type = JSON_KR_COMPOSITOR_PATH_INFO;
      res += info_pack_to_json(&json[res],&uber,max-res);
      break;
    }
    case 2: {
      uber.actual = &(actual->adapter_path_info);
      uber.type = JSON_KR_ADAPTER_PATH_INFO;
      res += info_pack_to_json(&json[res],&uber,max-res);
      break;
    }
  }


  return res;
}

int kr_transponder_path_io_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  uber_St uber_sub;
  int index;
  int res;
  struct kr_transponder_path_io_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_transponder_path_io_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"type\": ");
  uber.actual = &(actual->type);
  uber.type = JSON_KR_TRANSPONDER_PATH_IO_TYPE;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  index = kr_transponder_path_io_type_to_index(actual->type);
  uber_sub.type = index;
  uber_sub.actual = &(actual->info);
  uber.actual = &(uber_sub);
  uber.type = JSON_KR_TRANSPONDER_PATH_IO_PATH_INFO;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_transponder_path_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int res;
  struct kr_transponder_path_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_transponder_path_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"name\" : \"%s\",",actual->name);
  res += snprintf(&json[res],max-res,"\"input\": ");
  uber.actual = &(actual->input);
  uber.type = JSON_KR_TRANSPONDER_PATH_IO_INFO;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"output\": ");
  uber.actual = &(actual->output);
  uber.type = JSON_KR_TRANSPONDER_PATH_IO_INFO;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

