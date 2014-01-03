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
    case KR_ADP_X11:
      return 4;
    case KR_ADP_ALSA:
      return 5;
    case KR_ADP_KRAD:
      return 6;
    case KR_ADP_ENCODER:
      return 7;
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
    case KR_ADP_X11:
      return "kr_adp_x11";
    case KR_ADP_ALSA:
      return "kr_adp_alsa";
    case KR_ADP_KRAD:
      return "kr_adp_krad";
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
  if (!strcmp(string,"kr_adp_x11")) {
    return KR_ADP_X11;
  }
  if (!strcmp(string,"kr_adp_alsa")) {
    return KR_ADP_ALSA;
  }
  if (!strcmp(string,"kr_adp_krad")) {
    return KR_ADP_KRAD;
  }
  if (!strcmp(string,"kr_adp_encoder")) {
    return KR_ADP_ENCODER;
  }

  return -1;
}

int kr_adapter_api_info_init(void *st, int idx) {
  kr_adapter_api_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_info *)st;
  memset(st, 0, sizeof(kr_adapter_api_info));
  switch (idx) {
    case 0: {
      kr_jack_info_init(&actual->jack);
      break;
    }
    case 1: {
      kr_wayland_info_init(&actual->wayland);
      break;
    }
    case 2: {
      kr_v4l2_info_init(&actual->v4l2);
      break;
    }
    case 3: {
      kr_decklink_info_init(&actual->decklink);
      break;
    }
    case 4: {
      kr_x11_info_init(&actual->x11);
      break;
    }
  }


  return -1;
}

int kr_adapter_api_info_valid(void *st, int idx) {
  kr_adapter_api_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_info *)st;
  switch (idx) {
    case 0: {
      kr_jack_info_valid(&actual->jack);
      break;
    }
    case 1: {
      kr_wayland_info_valid(&actual->wayland);
      break;
    }
    case 2: {
      kr_v4l2_info_valid(&actual->v4l2);
      break;
    }
    case 3: {
      kr_decklink_info_valid(&actual->decklink);
      break;
    }
    case 4: {
      kr_x11_info_valid(&actual->x11);
      break;
    }
  }


  return -1;
}

int kr_adapter_api_info_random(void *st, int idx) {
  kr_adapter_api_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_info *)st;
  memset(st, 0, sizeof(kr_adapter_api_info));
  switch (idx) {
    case 0: {
      kr_jack_info_random(&actual->jack);
      break;
    }
    case 1: {
      kr_wayland_info_random(&actual->wayland);
      break;
    }
    case 2: {
      kr_v4l2_info_random(&actual->v4l2);
      break;
    }
    case 3: {
      kr_decklink_info_random(&actual->decklink);
      break;
    }
    case 4: {
      kr_x11_info_random(&actual->x11);
      break;
    }
  }


  return -1;
}

int kr_adapter_api_path_info_init(void *st, int idx) {
  kr_adapter_api_path_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_path_info *)st;
  memset(st, 0, sizeof(kr_adapter_api_path_info));
  switch (idx) {
    case 0: {
      kr_jack_path_info_init(&actual->jack);
      break;
    }
    case 1: {
      kr_wayland_path_info_init(&actual->wayland);
      break;
    }
    case 2: {
      kr_v4l2_open_info_init(&actual->v4l2);
      break;
    }
    case 3: {
      kr_decklink_path_info_init(&actual->decklink);
      break;
    }
    case 4: {
      kr_x11_path_info_init(&actual->x11);
      break;
    }
    case 5: {
      kr_alsa_path_info_init(&actual->alsa);
      break;
    }
  }


  return -1;
}

int kr_adapter_api_path_info_valid(void *st, int idx) {
  kr_adapter_api_path_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_path_info *)st;
  switch (idx) {
    case 0: {
      kr_jack_path_info_valid(&actual->jack);
      break;
    }
    case 1: {
      kr_wayland_path_info_valid(&actual->wayland);
      break;
    }
    case 2: {
      kr_v4l2_open_info_valid(&actual->v4l2);
      break;
    }
    case 3: {
      kr_decklink_path_info_valid(&actual->decklink);
      break;
    }
    case 4: {
      kr_x11_path_info_valid(&actual->x11);
      break;
    }
    case 5: {
      kr_alsa_path_info_valid(&actual->alsa);
      break;
    }
  }


  return -1;
}

int kr_adapter_api_path_info_random(void *st, int idx) {
  kr_adapter_api_path_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_adapter_api_path_info *)st;
  memset(st, 0, sizeof(kr_adapter_api_path_info));
  switch (idx) {
    case 0: {
      kr_jack_path_info_random(&actual->jack);
      break;
    }
    case 1: {
      kr_wayland_path_info_random(&actual->wayland);
      break;
    }
    case 2: {
      kr_v4l2_open_info_random(&actual->v4l2);
      break;
    }
    case 3: {
      kr_decklink_path_info_random(&actual->decklink);
      break;
    }
    case 4: {
      kr_x11_path_info_random(&actual->x11);
      break;
    }
    case 5: {
      kr_alsa_path_info_random(&actual->alsa);
      break;
    }
  }


  return -1;
}

int kr_adapter_info_init(void *st) {
  struct kr_adapter_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_adapter_info *)st;
  memset(st, 0, sizeof(struct kr_adapter_info));
  kr_adapter_api_info_init(&actual->api_info,kr_adapter_api_to_index(actual->api));

  return 0;
}

int kr_adapter_info_valid(void *st) {
  struct kr_adapter_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_adapter_info *)st;
  kr_adapter_api_info_valid(&actual->api_info,kr_adapter_api_to_index(actual->api));

  return 0;
}

int kr_adapter_info_random(void *st) {
  struct kr_adapter_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_adapter_info *)st;
  memset(st, 0, sizeof(struct kr_adapter_info));
  kr_adapter_api_info_random(&actual->api_info,kr_adapter_api_to_index(actual->api));

  return 0;
}

int kr_adapter_path_info_init(void *st) {
  struct kr_adapter_path_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_adapter_path_info *)st;
  memset(st, 0, sizeof(struct kr_adapter_path_info));
  for (i = 0; i < 64; i++) {
  }
  kr_adapter_api_path_info_init(&actual->info,kr_adapter_api_to_index(actual->api));

  return 0;
}

int kr_adapter_path_info_valid(void *st) {
  struct kr_adapter_path_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_adapter_path_info *)st;
  for (i = 0; i < 64; i++) {
    if (!actual->name[i]) {
      break;
    }
    if (i == 63 && actual->name[i]) {
      return -2;
    }
  }
  kr_adapter_api_path_info_valid(&actual->info,kr_adapter_api_to_index(actual->api));

  return 0;
}

int kr_adapter_path_info_random(void *st) {
  struct kr_adapter_path_info *actual;

  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_adapter_path_info *)st;
  memset(st, 0, sizeof(struct kr_adapter_path_info));
  for (i = 0; i < 64; i++) {
    scale = (double)25 / RAND_MAX;
    actual->name[i] = 97 + floor(rand() * scale);
    if (i == 63) {
      actual->name[63] = '\0';
    }
  }
  kr_adapter_api_path_info_random(&actual->info,kr_adapter_api_to_index(actual->api));

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

int kr_stat_St_init(void *st) {
  struct kr_stat_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_stat_St *)st;
  memset(st, 0, sizeof(struct kr_stat_St));

  return 0;
}

int kr_stat_St_valid(void *st) {
  struct kr_stat_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_stat_St *)st;

  return 0;
}

int kr_stat_St_random(void *st) {
  struct kr_stat_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_stat_St *)st;
  memset(st, 0, sizeof(struct kr_stat_St));

  return 0;
}

int krad_transponder_rep_St_init(void *st) {
  struct krad_transponder_rep_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct krad_transponder_rep_St *)st;
  memset(st, 0, sizeof(struct krad_transponder_rep_St));

  return 0;
}

int krad_transponder_rep_St_valid(void *st) {
  struct krad_transponder_rep_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct krad_transponder_rep_St *)st;

  return 0;
}

int krad_transponder_rep_St_random(void *st) {
  struct krad_transponder_rep_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct krad_transponder_rep_St *)st;
  memset(st, 0, sizeof(struct krad_transponder_rep_St));

  return 0;
}

int kr_flac_encoder_St_init(void *st) {
  struct kr_flac_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_flac_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_flac_encoder_St));

  return 0;
}

int kr_flac_encoder_St_valid(void *st) {
  struct kr_flac_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_flac_encoder_St *)st;

  return 0;
}

int kr_flac_encoder_St_random(void *st) {
  struct kr_flac_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_flac_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_flac_encoder_St));

  return 0;
}

int kr_vpx_decoder_St_init(void *st) {
  struct kr_vpx_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_vpx_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_vpx_decoder_St));

  return 0;
}

int kr_vpx_decoder_St_valid(void *st) {
  struct kr_vpx_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_vpx_decoder_St *)st;

  return 0;
}

int kr_vpx_decoder_St_random(void *st) {
  struct kr_vpx_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_vpx_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_vpx_decoder_St));

  return 0;
}

int kr_theora_decoder_St_init(void *st) {
  struct kr_theora_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_theora_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_theora_decoder_St));

  return 0;
}

int kr_theora_decoder_St_valid(void *st) {
  struct kr_theora_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_theora_decoder_St *)st;

  return 0;
}

int kr_theora_decoder_St_random(void *st) {
  struct kr_theora_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_theora_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_theora_decoder_St));

  return 0;
}

int kr_daala_decoder_St_init(void *st) {
  struct kr_daala_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_daala_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_daala_decoder_St));

  return 0;
}

int kr_daala_decoder_St_valid(void *st) {
  struct kr_daala_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_daala_decoder_St *)st;

  return 0;
}

int kr_daala_decoder_St_random(void *st) {
  struct kr_daala_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_daala_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_daala_decoder_St));

  return 0;
}

int kr_vorbis_decoder_St_init(void *st) {
  struct kr_vorbis_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_vorbis_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_vorbis_decoder_St));

  return 0;
}

int kr_vorbis_decoder_St_valid(void *st) {
  struct kr_vorbis_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_vorbis_decoder_St *)st;

  return 0;
}

int kr_vorbis_decoder_St_random(void *st) {
  struct kr_vorbis_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_vorbis_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_vorbis_decoder_St));

  return 0;
}

int kr_opus_decoder_St_init(void *st) {
  struct kr_opus_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_opus_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_opus_decoder_St));

  return 0;
}

int kr_opus_decoder_St_valid(void *st) {
  struct kr_opus_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_opus_decoder_St *)st;

  return 0;
}

int kr_opus_decoder_St_random(void *st) {
  struct kr_opus_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_opus_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_opus_decoder_St));

  return 0;
}

int kr_flac_decoder_St_init(void *st) {
  struct kr_flac_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_flac_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_flac_decoder_St));

  return 0;
}

int kr_flac_decoder_St_valid(void *st) {
  struct kr_flac_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_flac_decoder_St *)st;

  return 0;
}

int kr_flac_decoder_St_random(void *st) {
  struct kr_flac_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_flac_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_flac_decoder_St));

  return 0;
}

int kr_vpx_encoder_St_init(void *st) {
  struct kr_vpx_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_vpx_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_vpx_encoder_St));

  return 0;
}

int kr_vpx_encoder_St_valid(void *st) {
  struct kr_vpx_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_vpx_encoder_St *)st;

  return 0;
}

int kr_vpx_encoder_St_random(void *st) {
  struct kr_vpx_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_vpx_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_vpx_encoder_St));

  return 0;
}

int kr_theora_encoder_St_init(void *st) {
  struct kr_theora_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_theora_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_theora_encoder_St));

  return 0;
}

int kr_theora_encoder_St_valid(void *st) {
  struct kr_theora_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_theora_encoder_St *)st;

  return 0;
}

int kr_theora_encoder_St_random(void *st) {
  struct kr_theora_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_theora_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_theora_encoder_St));

  return 0;
}

int kr_daala_encoder_St_init(void *st) {
  struct kr_daala_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_daala_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_daala_encoder_St));

  return 0;
}

int kr_daala_encoder_St_valid(void *st) {
  struct kr_daala_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_daala_encoder_St *)st;

  return 0;
}

int kr_daala_encoder_St_random(void *st) {
  struct kr_daala_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_daala_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_daala_encoder_St));

  return 0;
}

int kr_vorbis_encoder_St_init(void *st) {
  struct kr_vorbis_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_vorbis_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_vorbis_encoder_St));

  return 0;
}

int kr_vorbis_encoder_St_valid(void *st) {
  struct kr_vorbis_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_vorbis_encoder_St *)st;

  return 0;
}

int kr_vorbis_encoder_St_random(void *st) {
  struct kr_vorbis_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_vorbis_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_vorbis_encoder_St));

  return 0;
}

int kr_opus_encoder_St_init(void *st) {
  struct kr_opus_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_opus_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_opus_encoder_St));

  return 0;
}

int kr_opus_encoder_St_valid(void *st) {
  struct kr_opus_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_opus_encoder_St *)st;

  return 0;
}

int kr_opus_encoder_St_random(void *st) {
  struct kr_opus_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_opus_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_opus_encoder_St));

  return 0;
}

int kr_audio_codec_t_init(void *st, int idx) {
  kr_audio_codec_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_audio_codec_t *)st;
  memset(st, 0, sizeof(kr_audio_codec_t));
  switch (idx) {
  }


  return -1;
}

int kr_audio_codec_t_valid(void *st, int idx) {
  kr_audio_codec_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_audio_codec_t *)st;
  switch (idx) {
  }


  return -1;
}

int kr_audio_codec_t_random(void *st, int idx) {
  kr_audio_codec_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_audio_codec_t *)st;
  memset(st, 0, sizeof(kr_audio_codec_t));
  switch (idx) {
  }


  return -1;
}

int kr_video_codec_t_init(void *st, int idx) {
  kr_video_codec_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_video_codec_t *)st;
  memset(st, 0, sizeof(kr_video_codec_t));
  switch (idx) {
  }


  return -1;
}

int kr_video_codec_t_valid(void *st, int idx) {
  kr_video_codec_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_video_codec_t *)st;
  switch (idx) {
  }


  return -1;
}

int kr_video_codec_t_random(void *st, int idx) {
  kr_video_codec_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_video_codec_t *)st;
  memset(st, 0, sizeof(kr_video_codec_t));
  switch (idx) {
  }


  return -1;
}

int kr_audio_encoder_St_init(void *st) {
  struct kr_audio_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_audio_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_audio_encoder_St));

  return 0;
}

int kr_audio_encoder_St_valid(void *st) {
  struct kr_audio_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_audio_encoder_St *)st;

  return 0;
}

int kr_audio_encoder_St_random(void *st) {
  struct kr_audio_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_audio_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_audio_encoder_St));

  return 0;
}

int kr_video_decoder_St_init(void *st) {
  struct kr_video_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_video_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_video_decoder_St));

  return 0;
}

int kr_video_decoder_St_valid(void *st) {
  struct kr_video_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_video_decoder_St *)st;

  return 0;
}

int kr_video_decoder_St_random(void *st) {
  struct kr_video_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_video_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_video_decoder_St));

  return 0;
}

int kr_audio_decoder_St_init(void *st) {
  struct kr_audio_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_audio_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_audio_decoder_St));

  return 0;
}

int kr_audio_decoder_St_valid(void *st) {
  struct kr_audio_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_audio_decoder_St *)st;

  return 0;
}

int kr_audio_decoder_St_random(void *st) {
  struct kr_audio_decoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_audio_decoder_St *)st;
  memset(st, 0, sizeof(struct kr_audio_decoder_St));

  return 0;
}

int kr_video_encoder_St_init(void *st) {
  struct kr_video_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_video_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_video_encoder_St));

  return 0;
}

int kr_video_encoder_St_valid(void *st) {
  struct kr_video_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_video_encoder_St *)st;

  return 0;
}

int kr_video_encoder_St_random(void *st) {
  struct kr_video_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_video_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_video_encoder_St));

  return 0;
}

int kr_av_encoder_t_init(void *st, int idx) {
  kr_av_encoder_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_av_encoder_t *)st;
  memset(st, 0, sizeof(kr_av_encoder_t));
  switch (idx) {
  }


  return -1;
}

int kr_av_encoder_t_valid(void *st, int idx) {
  kr_av_encoder_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_av_encoder_t *)st;
  switch (idx) {
  }


  return -1;
}

int kr_av_encoder_t_random(void *st, int idx) {
  kr_av_encoder_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_av_encoder_t *)st;
  memset(st, 0, sizeof(kr_av_encoder_t));
  switch (idx) {
  }


  return -1;
}

int kr_encoder_St_init(void *st) {
  struct kr_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_encoder_St));
  kr_av_encoder_t_init(&actual->av,krad_codec_t_to_index(actual->codec));

  return 0;
}

int kr_encoder_St_valid(void *st) {
  struct kr_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_encoder_St *)st;
  kr_av_encoder_t_valid(&actual->av,krad_codec_t_to_index(actual->codec));

  return 0;
}

int kr_encoder_St_random(void *st) {
  struct kr_encoder_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_encoder_St *)st;
  memset(st, 0, sizeof(struct kr_encoder_St));
  kr_av_encoder_t_random(&actual->av,krad_codec_t_to_index(actual->codec));

  return 0;
}

int kr_av_decoder_t_init(void *st, int idx) {
  kr_av_decoder_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_av_decoder_t *)st;
  memset(st, 0, sizeof(kr_av_decoder_t));
  switch (idx) {
  }


  return -1;
}

int kr_av_decoder_t_valid(void *st, int idx) {
  kr_av_decoder_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_av_decoder_t *)st;
  switch (idx) {
  }


  return -1;
}

int kr_av_decoder_t_random(void *st, int idx) {
  kr_av_decoder_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_av_decoder_t *)st;
  memset(st, 0, sizeof(kr_av_decoder_t));
  switch (idx) {
  }


  return -1;
}

int kr_udp_muxer_St_init(void *st) {
  struct kr_udp_muxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_udp_muxer_St *)st;
  memset(st, 0, sizeof(struct kr_udp_muxer_St));

  return 0;
}

int kr_udp_muxer_St_valid(void *st) {
  struct kr_udp_muxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_udp_muxer_St *)st;

  return 0;
}

int kr_udp_muxer_St_random(void *st) {
  struct kr_udp_muxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_udp_muxer_St *)st;
  memset(st, 0, sizeof(struct kr_udp_muxer_St));

  return 0;
}

int kr_ogg_muxer_St_init(void *st) {
  struct kr_ogg_muxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_ogg_muxer_St *)st;
  memset(st, 0, sizeof(struct kr_ogg_muxer_St));

  return 0;
}

int kr_ogg_muxer_St_valid(void *st) {
  struct kr_ogg_muxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_ogg_muxer_St *)st;

  return 0;
}

int kr_ogg_muxer_St_random(void *st) {
  struct kr_ogg_muxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_ogg_muxer_St *)st;
  memset(st, 0, sizeof(struct kr_ogg_muxer_St));

  return 0;
}

int kr_mkv_muxer_St_init(void *st) {
  struct kr_mkv_muxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_mkv_muxer_St *)st;
  memset(st, 0, sizeof(struct kr_mkv_muxer_St));

  return 0;
}

int kr_mkv_muxer_St_valid(void *st) {
  struct kr_mkv_muxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_mkv_muxer_St *)st;

  return 0;
}

int kr_mkv_muxer_St_random(void *st) {
  struct kr_mkv_muxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_mkv_muxer_St *)st;
  memset(st, 0, sizeof(struct kr_mkv_muxer_St));

  return 0;
}

int kr_transogg_muxer_St_init(void *st) {
  struct kr_transogg_muxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transogg_muxer_St *)st;
  memset(st, 0, sizeof(struct kr_transogg_muxer_St));

  return 0;
}

int kr_transogg_muxer_St_valid(void *st) {
  struct kr_transogg_muxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transogg_muxer_St *)st;

  return 0;
}

int kr_transogg_muxer_St_random(void *st) {
  struct kr_transogg_muxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transogg_muxer_St *)st;
  memset(st, 0, sizeof(struct kr_transogg_muxer_St));

  return 0;
}

int kr_udp_demuxer_St_init(void *st) {
  struct kr_udp_demuxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_udp_demuxer_St *)st;
  memset(st, 0, sizeof(struct kr_udp_demuxer_St));

  return 0;
}

int kr_udp_demuxer_St_valid(void *st) {
  struct kr_udp_demuxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_udp_demuxer_St *)st;

  return 0;
}

int kr_udp_demuxer_St_random(void *st) {
  struct kr_udp_demuxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_udp_demuxer_St *)st;
  memset(st, 0, sizeof(struct kr_udp_demuxer_St));

  return 0;
}

int kr_ogg_demuxer_St_init(void *st) {
  struct kr_ogg_demuxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_ogg_demuxer_St *)st;
  memset(st, 0, sizeof(struct kr_ogg_demuxer_St));

  return 0;
}

int kr_ogg_demuxer_St_valid(void *st) {
  struct kr_ogg_demuxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_ogg_demuxer_St *)st;

  return 0;
}

int kr_ogg_demuxer_St_random(void *st) {
  struct kr_ogg_demuxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_ogg_demuxer_St *)st;
  memset(st, 0, sizeof(struct kr_ogg_demuxer_St));

  return 0;
}

int kr_mkv_demuxer_St_init(void *st) {
  struct kr_mkv_demuxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_mkv_demuxer_St *)st;
  memset(st, 0, sizeof(struct kr_mkv_demuxer_St));

  return 0;
}

int kr_mkv_demuxer_St_valid(void *st) {
  struct kr_mkv_demuxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_mkv_demuxer_St *)st;

  return 0;
}

int kr_mkv_demuxer_St_random(void *st) {
  struct kr_mkv_demuxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_mkv_demuxer_St *)st;
  memset(st, 0, sizeof(struct kr_mkv_demuxer_St));

  return 0;
}

int kr_transogg_demuxer_St_init(void *st) {
  struct kr_transogg_demuxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transogg_demuxer_St *)st;
  memset(st, 0, sizeof(struct kr_transogg_demuxer_St));

  return 0;
}

int kr_transogg_demuxer_St_valid(void *st) {
  struct kr_transogg_demuxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transogg_demuxer_St *)st;

  return 0;
}

int kr_transogg_demuxer_St_random(void *st) {
  struct kr_transogg_demuxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transogg_demuxer_St *)st;
  memset(st, 0, sizeof(struct kr_transogg_demuxer_St));

  return 0;
}

int kr_muxer_actual_t_init(void *st, int idx) {
  kr_muxer_actual_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_muxer_actual_t *)st;
  memset(st, 0, sizeof(kr_muxer_actual_t));
  switch (idx) {
  }


  return -1;
}

int kr_muxer_actual_t_valid(void *st, int idx) {
  kr_muxer_actual_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_muxer_actual_t *)st;
  switch (idx) {
  }


  return -1;
}

int kr_muxer_actual_t_random(void *st, int idx) {
  kr_muxer_actual_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_muxer_actual_t *)st;
  memset(st, 0, sizeof(kr_muxer_actual_t));
  switch (idx) {
  }


  return -1;
}

int kr_muxer_St_init(void *st) {
  struct kr_muxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_muxer_St *)st;
  memset(st, 0, sizeof(struct kr_muxer_St));

  return 0;
}

int kr_muxer_St_valid(void *st) {
  struct kr_muxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_muxer_St *)st;

  return 0;
}

int kr_muxer_St_random(void *st) {
  struct kr_muxer_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_muxer_St *)st;
  memset(st, 0, sizeof(struct kr_muxer_St));

  return 0;
}

int kr_demuxer_actual_t_init(void *st, int idx) {
  kr_demuxer_actual_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_demuxer_actual_t *)st;
  memset(st, 0, sizeof(kr_demuxer_actual_t));
  switch (idx) {
  }


  return -1;
}

int kr_demuxer_actual_t_valid(void *st, int idx) {
  kr_demuxer_actual_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_demuxer_actual_t *)st;
  switch (idx) {
  }


  return -1;
}

int kr_demuxer_actual_t_random(void *st, int idx) {
  kr_demuxer_actual_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_demuxer_actual_t *)st;
  memset(st, 0, sizeof(kr_demuxer_actual_t));
  switch (idx) {
  }


  return -1;
}

int kr_rawin_St_init(void *st) {
  struct kr_rawin_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_rawin_St *)st;
  memset(st, 0, sizeof(struct kr_rawin_St));

  return 0;
}

int kr_rawin_St_valid(void *st) {
  struct kr_rawin_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_rawin_St *)st;

  return 0;
}

int kr_rawin_St_random(void *st) {
  struct kr_rawin_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_rawin_St *)st;
  memset(st, 0, sizeof(struct kr_rawin_St));

  return 0;
}

int kr_rawout_St_init(void *st) {
  struct kr_rawout_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_rawout_St *)st;
  memset(st, 0, sizeof(struct kr_rawout_St));

  return 0;
}

int kr_rawout_St_valid(void *st) {
  struct kr_rawout_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_rawout_St *)st;

  return 0;
}

int kr_rawout_St_random(void *st) {
  struct kr_rawout_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_rawout_St *)st;
  memset(st, 0, sizeof(struct kr_rawout_St));

  return 0;
}

int kr_transponder_subunit_actual_t_init(void *st, int idx) {
  kr_transponder_subunit_actual_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_transponder_subunit_actual_t *)st;
  memset(st, 0, sizeof(kr_transponder_subunit_actual_t));
  switch (idx) {
  }


  return -1;
}

int kr_transponder_subunit_actual_t_valid(void *st, int idx) {
  kr_transponder_subunit_actual_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_transponder_subunit_actual_t *)st;
  switch (idx) {
  }


  return -1;
}

int kr_transponder_subunit_actual_t_random(void *st, int idx) {
  kr_transponder_subunit_actual_t *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_transponder_subunit_actual_t *)st;
  memset(st, 0, sizeof(kr_transponder_subunit_actual_t));
  switch (idx) {
  }


  return -1;
}

int krad_transponder_subunit_rep_St_init(void *st) {
  struct krad_transponder_subunit_rep_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct krad_transponder_subunit_rep_St *)st;
  memset(st, 0, sizeof(struct krad_transponder_subunit_rep_St));

  return 0;
}

int krad_transponder_subunit_rep_St_valid(void *st) {
  struct krad_transponder_subunit_rep_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct krad_transponder_subunit_rep_St *)st;

  return 0;
}

int krad_transponder_subunit_rep_St_random(void *st) {
  struct krad_transponder_subunit_rep_St *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct krad_transponder_subunit_rep_St *)st;
  memset(st, 0, sizeof(struct krad_transponder_subunit_rep_St));

  return 0;
}

int kr_transponder_info_init(void *st) {
  struct kr_transponder_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_info *)st;
  memset(st, 0, sizeof(struct kr_transponder_info));

  return 0;
}

int kr_transponder_info_valid(void *st) {
  struct kr_transponder_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_info *)st;

  return 0;
}

int kr_transponder_info_random(void *st) {
  struct kr_transponder_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_info *)st;
  memset(st, 0, sizeof(struct kr_transponder_info));

  return 0;
}

int kr_transponder_path_io_path_info_init(void *st, int idx) {
  kr_transponder_path_io_path_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_transponder_path_io_path_info *)st;
  memset(st, 0, sizeof(kr_transponder_path_io_path_info));
  switch (idx) {
    case 0: {
      kr_mixer_path_info_init(&actual->mixer_path_info);
      break;
    }
    case 1: {
      kr_compositor_path_info_init(&actual->compositor_path_info);
      break;
    }
    case 2: {
      kr_adapter_path_info_init(&actual->adapter_path_info);
      break;
    }
  }


  return -1;
}

int kr_transponder_path_io_path_info_valid(void *st, int idx) {
  kr_transponder_path_io_path_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_transponder_path_io_path_info *)st;
  switch (idx) {
    case 0: {
      kr_mixer_path_info_valid(&actual->mixer_path_info);
      break;
    }
    case 1: {
      kr_compositor_path_info_valid(&actual->compositor_path_info);
      break;
    }
    case 2: {
      kr_adapter_path_info_valid(&actual->adapter_path_info);
      break;
    }
  }


  return -1;
}

int kr_transponder_path_io_path_info_random(void *st, int idx) {
  kr_transponder_path_io_path_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (kr_transponder_path_io_path_info *)st;
  memset(st, 0, sizeof(kr_transponder_path_io_path_info));
  switch (idx) {
    case 0: {
      kr_mixer_path_info_random(&actual->mixer_path_info);
      break;
    }
    case 1: {
      kr_compositor_path_info_random(&actual->compositor_path_info);
      break;
    }
    case 2: {
      kr_adapter_path_info_random(&actual->adapter_path_info);
      break;
    }
  }


  return -1;
}

int kr_transponder_path_io_info_init(void *st) {
  struct kr_transponder_path_io_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_path_io_info *)st;
  memset(st, 0, sizeof(struct kr_transponder_path_io_info));
  kr_transponder_path_io_path_info_init(&actual->info,kr_transponder_path_io_type_to_index(actual->type));

  return 0;
}

int kr_transponder_path_io_info_valid(void *st) {
  struct kr_transponder_path_io_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_path_io_info *)st;
  kr_transponder_path_io_path_info_valid(&actual->info,kr_transponder_path_io_type_to_index(actual->type));

  return 0;
}

int kr_transponder_path_io_info_random(void *st) {
  struct kr_transponder_path_io_info *actual;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_path_io_info *)st;
  memset(st, 0, sizeof(struct kr_transponder_path_io_info));
  kr_transponder_path_io_path_info_random(&actual->info,kr_transponder_path_io_type_to_index(actual->type));

  return 0;
}

int kr_transponder_path_info_init(void *st) {
  struct kr_transponder_path_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_path_info *)st;
  memset(st, 0, sizeof(struct kr_transponder_path_info));
  for (i = 0; i < 128; i++) {
  }
  kr_transponder_path_io_info_init(&actual->input);
  kr_transponder_path_io_info_init(&actual->output);

  return 0;
}

int kr_transponder_path_info_valid(void *st) {
  struct kr_transponder_path_info *actual;

  int i;

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_path_info *)st;
  for (i = 0; i < 128; i++) {
    if (!actual->name[i]) {
      break;
    }
    if (i == 127 && actual->name[i]) {
      return -2;
    }
  }
  kr_transponder_path_io_info_valid(&actual->input);
  kr_transponder_path_io_info_valid(&actual->output);

  return 0;
}

int kr_transponder_path_info_random(void *st) {
  struct kr_transponder_path_info *actual;

  int i;
  struct timeval tv;
  double scale;

  gettimeofday(&tv, NULL);
  srand(tv.tv_sec + tv.tv_usec * 1000000ul);

  if (st == NULL) {
    return -1;
  }

  actual = (struct kr_transponder_path_info *)st;
  memset(st, 0, sizeof(struct kr_transponder_path_info));
  for (i = 0; i < 128; i++) {
    scale = (double)25 / RAND_MAX;
    actual->name[i] = 97 + floor(rand() * scale);
    if (i == 127) {
      actual->name[127] = '\0';
    }
  }
  kr_transponder_path_io_info_random(&actual->input);
  kr_transponder_path_io_info_random(&actual->output);

  return 0;
}

