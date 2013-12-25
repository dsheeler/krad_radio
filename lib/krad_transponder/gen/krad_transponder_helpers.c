#include "krad_transponder_helpers.h"

int kr_adapter_path_direction_to_index(int val) {
  switch (val) {
    case KR_ADP_PATH_INPUT:
      return 0;
    case KR_ADP_PATH_OUTPUT:
      return 1;
  }
  return -1;
}

char *kr_strfr_kr_adapter_path_direction(int val) {
  switch (val) {
    case KR_ADP_PATH_INPUT:
      return "kr_adp_path_input";
    case KR_ADP_PATH_OUTPUT:
      return "kr_adp_path_output";
  }
  return NULL;
}

int kr_strto_kr_adapter_path_direction(char *string) {
  if (!strcmp(string,"kr_adp_path_input")) {
    return KR_ADP_PATH_INPUT;
  }
  if (!strcmp(string,"kr_adp_path_output")) {
    return KR_ADP_PATH_OUTPUT;
  }

  return -1;
}

int kr_adapter_api_to_index(int val) {
  switch (val) {
    case KR_ADP_JACK:
      return 0;
    case KR_ADP_WAYLAND:
      return 1;
    case KR_ADP_V4L2:
      return 2;
    case KR_ADP_DECKLINK:
      return 3;
    case KR_ADP_KRAPI:
      return 4;
    case KR_ADP_ALSA:
      return 5;
    case KR_ADP_ENCODER:
      return 6;
  }
  return -1;
}

char *kr_strfr_kr_adapter_api(int val) {
  switch (val) {
    case KR_ADP_JACK:
      return "kr_adp_jack";
    case KR_ADP_WAYLAND:
      return "kr_adp_wayland";
    case KR_ADP_V4L2:
      return "kr_adp_v4l2";
    case KR_ADP_DECKLINK:
      return "kr_adp_decklink";
    case KR_ADP_KRAPI:
      return "kr_adp_krapi";
    case KR_ADP_ALSA:
      return "kr_adp_alsa";
    case KR_ADP_ENCODER:
      return "kr_adp_encoder";
  }
  return NULL;
}

int kr_strto_kr_adapter_api(char *string) {
  if (!strcmp(string,"kr_adp_jack")) {
    return KR_ADP_JACK;
  }
  if (!strcmp(string,"kr_adp_wayland")) {
    return KR_ADP_WAYLAND;
  }
  if (!strcmp(string,"kr_adp_v4l2")) {
    return KR_ADP_V4L2;
  }
  if (!strcmp(string,"kr_adp_decklink")) {
    return KR_ADP_DECKLINK;
  }
  if (!strcmp(string,"kr_adp_krapi")) {
    return KR_ADP_KRAPI;
  }
  if (!strcmp(string,"kr_adp_alsa")) {
    return KR_ADP_ALSA;
  }
  if (!strcmp(string,"kr_adp_encoder")) {
    return KR_ADP_ENCODER;
  }

  return -1;
}

int kr_adapter_api_info_init(kr_adapter_api_info *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
    case 0: {
      kr_jack_info_init(&st->jack);
      break;
    }
    case 1: {
      kr_wayland_info_init(&st->wayland);
      break;
    }
    case 2: {
      kr_v4l2_info_init(&st->v4l2);
      break;
    }
  }


  return -1;
}

int kr_adapter_api_info_valid(kr_adapter_api_info *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
    case 0: {
      kr_jack_info_valid(&st->jack);
      break;
    }
    case 1: {
      kr_wayland_info_valid(&st->wayland);
      break;
    }
    case 2: {
      kr_v4l2_info_valid(&st->v4l2);
      break;
    }
  }


  return -1;
}

int kr_adapter_api_info_random(kr_adapter_api_info *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
    case 0: {
      kr_jack_info_random(&st->jack);
      break;
    }
    case 1: {
      kr_wayland_info_random(&st->wayland);
      break;
    }
    case 2: {
      kr_v4l2_info_random(&st->v4l2);
      break;
    }
  }


  return -1;
}

int kr_adapter_api_path_info_init(kr_adapter_api_path_info *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
    case 0: {
      kr_jack_path_info_init(&st->jack);
      break;
    }
    case 1: {
      kr_wayland_path_info_init(&st->wayland);
      break;
    }
    case 2: {
      kr_v4l2_open_info_init(&st->v4l2);
      break;
    }
  }


  return -1;
}

int kr_adapter_api_path_info_valid(kr_adapter_api_path_info *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
    case 0: {
      kr_jack_path_info_valid(&st->jack);
      break;
    }
    case 1: {
      kr_wayland_path_info_valid(&st->wayland);
      break;
    }
    case 2: {
      kr_v4l2_open_info_valid(&st->v4l2);
      break;
    }
  }


  return -1;
}

int kr_adapter_api_path_info_random(kr_adapter_api_path_info *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
    case 0: {
      kr_jack_path_info_random(&st->jack);
      break;
    }
    case 1: {
      kr_wayland_path_info_random(&st->wayland);
      break;
    }
    case 2: {
      kr_v4l2_open_info_random(&st->v4l2);
      break;
    }
  }


  return -1;
}

int kr_adapter_info_init(struct kr_adapter_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_adapter_info));
  kr_adapter_api_info_init(&st->api_info,kr_adapter_api_to_index(st->api));

  return 0;
}

int kr_adapter_info_valid(struct kr_adapter_info *st) {
  if (st == NULL) {
    return -1;
  }

  kr_adapter_api_info_valid(&st->api_info,kr_adapter_api_to_index(st->api));

  return 0;
}

int kr_adapter_info_random(struct kr_adapter_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_adapter_info));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  kr_adapter_api_info_random(&st->api_info,kr_adapter_api_to_index(st->api));

  return 0;
}

int kr_adapter_path_info_init(struct kr_adapter_path_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_adapter_path_info));
  kr_adapter_api_path_info_init(&st->info,kr_adapter_api_to_index(st->api));

  return 0;
}

int kr_adapter_path_info_valid(struct kr_adapter_path_info *st) {
  if (st == NULL) {
    return -1;
  }

  kr_adapter_api_path_info_valid(&st->info,kr_adapter_api_to_index(st->api));

  return 0;
}

int kr_adapter_path_info_random(struct kr_adapter_path_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_adapter_path_info));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  kr_adapter_api_path_info_random(&st->info,kr_adapter_api_to_index(st->api));

  return 0;
}

int kr_txpdr_su_type_t_to_index(int val) {
  switch (val) {
    case DEMUX:
      return 0;
    case DECODE:
      return 1;
    case MUX:
      return 2;
    case ENCODE:
      return 3;
    case RAWIN:
      return 4;
    case RAWOUT:
      return 5;
    case FAILURE:
      return 6;
  }
  return -1;
}

char *kr_strfr_kr_txpdr_su_type_t(int val) {
  switch (val) {
    case DEMUX:
      return "demux";
    case DECODE:
      return "decode";
    case MUX:
      return "mux";
    case ENCODE:
      return "encode";
    case RAWIN:
      return "rawin";
    case RAWOUT:
      return "rawout";
    case FAILURE:
      return "failure";
  }
  return NULL;
}

int kr_strto_kr_txpdr_su_type_t(char *string) {
  if (!strcmp(string,"demux")) {
    return DEMUX;
  }
  if (!strcmp(string,"decode")) {
    return DECODE;
  }
  if (!strcmp(string,"mux")) {
    return MUX;
  }
  if (!strcmp(string,"encode")) {
    return ENCODE;
  }
  if (!strcmp(string,"rawin")) {
    return RAWIN;
  }
  if (!strcmp(string,"rawout")) {
    return RAWOUT;
  }
  if (!strcmp(string,"failure")) {
    return FAILURE;
  }

  return -1;
}

int kr_transponder_path_io_type_to_index(int val) {
  switch (val) {
    case KR_XPDR_MIXER:
      return 0;
    case KR_XPDR_COMPOSITOR:
      return 1;
    case KR_XPDR_ADAPTER:
      return 2;
  }
  return -1;
}

char *kr_strfr_kr_transponder_path_io_type(int val) {
  switch (val) {
    case KR_XPDR_MIXER:
      return "kr_xpdr_mixer";
    case KR_XPDR_COMPOSITOR:
      return "kr_xpdr_compositor";
    case KR_XPDR_ADAPTER:
      return "kr_xpdr_adapter";
  }
  return NULL;
}

int kr_strto_kr_transponder_path_io_type(char *string) {
  if (!strcmp(string,"kr_xpdr_mixer")) {
    return KR_XPDR_MIXER;
  }
  if (!strcmp(string,"kr_xpdr_compositor")) {
    return KR_XPDR_COMPOSITOR;
  }
  if (!strcmp(string,"kr_xpdr_adapter")) {
    return KR_XPDR_ADAPTER;
  }

  return -1;
}

int kr_stat_St_init(struct kr_stat_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_stat_St));

  return 0;
}

int kr_stat_St_valid(struct kr_stat_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_stat_St_random(struct kr_stat_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_stat_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int krad_transponder_rep_St_init(struct krad_transponder_rep_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct krad_transponder_rep_St));

  return 0;
}

int krad_transponder_rep_St_valid(struct krad_transponder_rep_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int krad_transponder_rep_St_random(struct krad_transponder_rep_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct krad_transponder_rep_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_flac_encoder_St_init(struct kr_flac_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_flac_encoder_St));

  return 0;
}

int kr_flac_encoder_St_valid(struct kr_flac_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_flac_encoder_St_random(struct kr_flac_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_flac_encoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_vpx_decoder_St_init(struct kr_vpx_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_vpx_decoder_St));

  return 0;
}

int kr_vpx_decoder_St_valid(struct kr_vpx_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_vpx_decoder_St_random(struct kr_vpx_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_vpx_decoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_theora_decoder_St_init(struct kr_theora_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_theora_decoder_St));

  return 0;
}

int kr_theora_decoder_St_valid(struct kr_theora_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_theora_decoder_St_random(struct kr_theora_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_theora_decoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_daala_decoder_St_init(struct kr_daala_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_daala_decoder_St));

  return 0;
}

int kr_daala_decoder_St_valid(struct kr_daala_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_daala_decoder_St_random(struct kr_daala_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_daala_decoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_vorbis_decoder_St_init(struct kr_vorbis_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_vorbis_decoder_St));

  return 0;
}

int kr_vorbis_decoder_St_valid(struct kr_vorbis_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_vorbis_decoder_St_random(struct kr_vorbis_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_vorbis_decoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_opus_decoder_St_init(struct kr_opus_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_opus_decoder_St));

  return 0;
}

int kr_opus_decoder_St_valid(struct kr_opus_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_opus_decoder_St_random(struct kr_opus_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_opus_decoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_flac_decoder_St_init(struct kr_flac_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_flac_decoder_St));

  return 0;
}

int kr_flac_decoder_St_valid(struct kr_flac_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_flac_decoder_St_random(struct kr_flac_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_flac_decoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_vpx_encoder_St_init(struct kr_vpx_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_vpx_encoder_St));

  return 0;
}

int kr_vpx_encoder_St_valid(struct kr_vpx_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_vpx_encoder_St_random(struct kr_vpx_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_vpx_encoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_theora_encoder_St_init(struct kr_theora_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_theora_encoder_St));

  return 0;
}

int kr_theora_encoder_St_valid(struct kr_theora_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_theora_encoder_St_random(struct kr_theora_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_theora_encoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_daala_encoder_St_init(struct kr_daala_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_daala_encoder_St));

  return 0;
}

int kr_daala_encoder_St_valid(struct kr_daala_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_daala_encoder_St_random(struct kr_daala_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_daala_encoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_vorbis_encoder_St_init(struct kr_vorbis_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_vorbis_encoder_St));

  return 0;
}

int kr_vorbis_encoder_St_valid(struct kr_vorbis_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_vorbis_encoder_St_random(struct kr_vorbis_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_vorbis_encoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_opus_encoder_St_init(struct kr_opus_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_opus_encoder_St));

  return 0;
}

int kr_opus_encoder_St_valid(struct kr_opus_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_opus_encoder_St_random(struct kr_opus_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_opus_encoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_audio_codec_t_init(kr_audio_codec_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_audio_codec_t_valid(kr_audio_codec_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_audio_codec_t_random(kr_audio_codec_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_video_codec_t_init(kr_video_codec_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_video_codec_t_valid(kr_video_codec_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_video_codec_t_random(kr_video_codec_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_audio_encoder_St_init(struct kr_audio_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_audio_encoder_St));

  return 0;
}

int kr_audio_encoder_St_valid(struct kr_audio_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_audio_encoder_St_random(struct kr_audio_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_audio_encoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_video_decoder_St_init(struct kr_video_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_video_decoder_St));

  return 0;
}

int kr_video_decoder_St_valid(struct kr_video_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_video_decoder_St_random(struct kr_video_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_video_decoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_audio_decoder_St_init(struct kr_audio_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_audio_decoder_St));

  return 0;
}

int kr_audio_decoder_St_valid(struct kr_audio_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_audio_decoder_St_random(struct kr_audio_decoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_audio_decoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_video_encoder_St_init(struct kr_video_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_video_encoder_St));

  return 0;
}

int kr_video_encoder_St_valid(struct kr_video_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_video_encoder_St_random(struct kr_video_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_video_encoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_av_encoder_t_init(kr_av_encoder_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_av_encoder_t_valid(kr_av_encoder_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_av_encoder_t_random(kr_av_encoder_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_encoder_St_init(struct kr_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_encoder_St));
  kr_av_encoder_t_init(&st->av,krad_codec_t_to_index(st->codec));

  return 0;
}

int kr_encoder_St_valid(struct kr_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  kr_av_encoder_t_valid(&st->av,krad_codec_t_to_index(st->codec));

  return 0;
}

int kr_encoder_St_random(struct kr_encoder_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_encoder_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  kr_av_encoder_t_random(&st->av,krad_codec_t_to_index(st->codec));

  return 0;
}

int kr_av_decoder_t_init(kr_av_decoder_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_av_decoder_t_valid(kr_av_decoder_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_av_decoder_t_random(kr_av_decoder_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_udp_muxer_St_init(struct kr_udp_muxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_udp_muxer_St));

  return 0;
}

int kr_udp_muxer_St_valid(struct kr_udp_muxer_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_udp_muxer_St_random(struct kr_udp_muxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_udp_muxer_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_ogg_muxer_St_init(struct kr_ogg_muxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_ogg_muxer_St));

  return 0;
}

int kr_ogg_muxer_St_valid(struct kr_ogg_muxer_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_ogg_muxer_St_random(struct kr_ogg_muxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_ogg_muxer_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_mkv_muxer_St_init(struct kr_mkv_muxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_mkv_muxer_St));

  return 0;
}

int kr_mkv_muxer_St_valid(struct kr_mkv_muxer_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_mkv_muxer_St_random(struct kr_mkv_muxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_mkv_muxer_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_transogg_muxer_St_init(struct kr_transogg_muxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_transogg_muxer_St));

  return 0;
}

int kr_transogg_muxer_St_valid(struct kr_transogg_muxer_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_transogg_muxer_St_random(struct kr_transogg_muxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_transogg_muxer_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_udp_demuxer_St_init(struct kr_udp_demuxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_udp_demuxer_St));

  return 0;
}

int kr_udp_demuxer_St_valid(struct kr_udp_demuxer_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_udp_demuxer_St_random(struct kr_udp_demuxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_udp_demuxer_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_ogg_demuxer_St_init(struct kr_ogg_demuxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_ogg_demuxer_St));

  return 0;
}

int kr_ogg_demuxer_St_valid(struct kr_ogg_demuxer_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_ogg_demuxer_St_random(struct kr_ogg_demuxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_ogg_demuxer_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_mkv_demuxer_St_init(struct kr_mkv_demuxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_mkv_demuxer_St));

  return 0;
}

int kr_mkv_demuxer_St_valid(struct kr_mkv_demuxer_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_mkv_demuxer_St_random(struct kr_mkv_demuxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_mkv_demuxer_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_transogg_demuxer_St_init(struct kr_transogg_demuxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_transogg_demuxer_St));

  return 0;
}

int kr_transogg_demuxer_St_valid(struct kr_transogg_demuxer_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_transogg_demuxer_St_random(struct kr_transogg_demuxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_transogg_demuxer_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_muxer_actual_t_init(kr_muxer_actual_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_muxer_actual_t_valid(kr_muxer_actual_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_muxer_actual_t_random(kr_muxer_actual_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_muxer_St_init(struct kr_muxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_muxer_St));

  return 0;
}

int kr_muxer_St_valid(struct kr_muxer_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_muxer_St_random(struct kr_muxer_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_muxer_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_demuxer_actual_t_init(kr_demuxer_actual_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_demuxer_actual_t_valid(kr_demuxer_actual_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_demuxer_actual_t_random(kr_demuxer_actual_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_rawin_St_init(struct kr_rawin_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_rawin_St));

  return 0;
}

int kr_rawin_St_valid(struct kr_rawin_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_rawin_St_random(struct kr_rawin_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_rawin_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_rawout_St_init(struct kr_rawout_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_rawout_St));

  return 0;
}

int kr_rawout_St_valid(struct kr_rawout_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_rawout_St_random(struct kr_rawout_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_rawout_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_transponder_subunit_actual_t_init(kr_transponder_subunit_actual_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_transponder_subunit_actual_t_valid(kr_transponder_subunit_actual_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int kr_transponder_subunit_actual_t_random(kr_transponder_subunit_actual_t *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
  }


  return -1;
}

int krad_transponder_subunit_rep_St_init(struct krad_transponder_subunit_rep_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct krad_transponder_subunit_rep_St));

  return 0;
}

int krad_transponder_subunit_rep_St_valid(struct krad_transponder_subunit_rep_St *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int krad_transponder_subunit_rep_St_random(struct krad_transponder_subunit_rep_St *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct krad_transponder_subunit_rep_St));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_transponder_info_init(struct kr_transponder_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_transponder_info));

  return 0;
}

int kr_transponder_info_valid(struct kr_transponder_info *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_transponder_info_random(struct kr_transponder_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_transponder_info));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);


  return 0;
}

int kr_transponder_path_io_path_info_init(kr_transponder_path_io_path_info *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
    case 0: {
      kr_mixer_path_info_init(&st->mixer_path_info);
      break;
    }
    case 1: {
      kr_compositor_path_info_init(&st->compositor_path_info);
      break;
    }
    case 2: {
      kr_adapter_path_info_init(&st->adapter_path_info);
      break;
    }
  }


  return -1;
}

int kr_transponder_path_io_path_info_valid(kr_transponder_path_io_path_info *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
    case 0: {
      kr_mixer_path_info_valid(&st->mixer_path_info);
      break;
    }
    case 1: {
      kr_compositor_path_info_valid(&st->compositor_path_info);
      break;
    }
    case 2: {
      kr_adapter_path_info_valid(&st->adapter_path_info);
      break;
    }
  }


  return -1;
}

int kr_transponder_path_io_path_info_random(kr_transponder_path_io_path_info *st, int idx) {
  if (st == NULL) {
    return -1;
  }

  switch (idx) {
    case 0: {
      kr_mixer_path_info_random(&st->mixer_path_info);
      break;
    }
    case 1: {
      kr_compositor_path_info_random(&st->compositor_path_info);
      break;
    }
    case 2: {
      kr_adapter_path_info_random(&st->adapter_path_info);
      break;
    }
  }


  return -1;
}

int kr_transponder_path_io_info_init(struct kr_transponder_path_io_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_transponder_path_io_info));
  kr_transponder_path_io_path_info_init(&st->info,kr_transponder_path_io_type_to_index(st->type));

  return 0;
}

int kr_transponder_path_io_info_valid(struct kr_transponder_path_io_info *st) {
  if (st == NULL) {
    return -1;
  }

  kr_transponder_path_io_path_info_valid(&st->info,kr_transponder_path_io_type_to_index(st->type));

  return 0;
}

int kr_transponder_path_io_info_random(struct kr_transponder_path_io_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_transponder_path_io_info));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  kr_transponder_path_io_path_info_random(&st->info,kr_transponder_path_io_type_to_index(st->type));

  return 0;
}

int kr_transponder_path_info_init(struct kr_transponder_path_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_transponder_path_info));
  kr_transponder_path_io_info_init(&st->input);
  kr_transponder_path_io_info_init(&st->output);

  return 0;
}

int kr_transponder_path_info_valid(struct kr_transponder_path_info *st) {
  if (st == NULL) {
    return -1;
  }

  kr_transponder_path_io_info_valid(&st->input);
  kr_transponder_path_io_info_valid(&st->output);

  return 0;
}

int kr_transponder_path_info_random(struct kr_transponder_path_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(struct kr_transponder_path_info));
  struct timeval tv;
  double scale;

  if (st == NULL) {
    return -1;
  }

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  kr_transponder_path_io_info_random(&st->input);
  kr_transponder_path_io_info_random(&st->output);

  return 0;
}

