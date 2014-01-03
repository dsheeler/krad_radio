#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_adapter_common.h"
#include <stdio.h>
#include <stdint.h>
#include "gen.h"
#include "krad_transponder_common.h"
int kr_adapter_path_direction_to_json(char *json, void *st, int32_t max);
int kr_adapter_api_to_json(char *json, void *st, int32_t max);
int kr_adapter_api_info_to_json(char *json, void *st, int32_t max);
int kr_adapter_api_path_info_to_json(char *json, void *st, int32_t max);
int kr_adapter_info_to_json(char *json, void *st, int32_t max);
int kr_adapter_path_info_to_json(char *json, void *st, int32_t max);
int kr_stat_St_to_json(char *json, void *st, int32_t max);
int krad_link_av_mode_t_to_json(char *json, void *st, int32_t max);
int kr_txpdr_su_type_t_to_json(char *json, void *st, int32_t max);
int krad_link_transport_mode_t_to_json(char *json, void *st, int32_t max);
int krad_link_video_source_t_to_json(char *json, void *st, int32_t max);
int krad_transponder_rep_St_to_json(char *json, void *st, int32_t max);
int kr_flac_encoder_St_to_json(char *json, void *st, int32_t max);
int kr_vpx_decoder_St_to_json(char *json, void *st, int32_t max);
int kr_theora_decoder_St_to_json(char *json, void *st, int32_t max);
int kr_daala_decoder_St_to_json(char *json, void *st, int32_t max);
int kr_vorbis_decoder_St_to_json(char *json, void *st, int32_t max);
int kr_opus_decoder_St_to_json(char *json, void *st, int32_t max);
int kr_flac_decoder_St_to_json(char *json, void *st, int32_t max);
int kr_vpx_encoder_St_to_json(char *json, void *st, int32_t max);
int kr_theora_encoder_St_to_json(char *json, void *st, int32_t max);
int kr_daala_encoder_St_to_json(char *json, void *st, int32_t max);
int kr_vorbis_encoder_St_to_json(char *json, void *st, int32_t max);
int kr_opus_encoder_St_to_json(char *json, void *st, int32_t max);
int kr_audio_codec_t_to_json(char *json, void *st, int32_t max);
int kr_video_codec_t_to_json(char *json, void *st, int32_t max);
int kr_audio_encoder_St_to_json(char *json, void *st, int32_t max);
int kr_video_decoder_St_to_json(char *json, void *st, int32_t max);
int kr_audio_decoder_St_to_json(char *json, void *st, int32_t max);
int kr_video_encoder_St_to_json(char *json, void *st, int32_t max);
int kr_av_encoder_t_to_json(char *json, void *st, int32_t max);
int kr_encoder_St_to_json(char *json, void *st, int32_t max);
int kr_av_decoder_t_to_json(char *json, void *st, int32_t max);
int kr_udp_muxer_St_to_json(char *json, void *st, int32_t max);
int kr_ogg_muxer_St_to_json(char *json, void *st, int32_t max);
int kr_mkv_muxer_St_to_json(char *json, void *st, int32_t max);
int kr_transogg_muxer_St_to_json(char *json, void *st, int32_t max);
int kr_udp_demuxer_St_to_json(char *json, void *st, int32_t max);
int kr_ogg_demuxer_St_to_json(char *json, void *st, int32_t max);
int kr_mkv_demuxer_St_to_json(char *json, void *st, int32_t max);
int kr_transogg_demuxer_St_to_json(char *json, void *st, int32_t max);
int kr_muxer_actual_t_to_json(char *json, void *st, int32_t max);
int kr_muxer_St_to_json(char *json, void *st, int32_t max);
int kr_demuxer_actual_t_to_json(char *json, void *st, int32_t max);
int kr_rawin_St_to_json(char *json, void *st, int32_t max);
int kr_rawout_St_to_json(char *json, void *st, int32_t max);
int kr_transponder_subunit_actual_t_to_json(char *json, void *st, int32_t max);
int krad_transponder_subunit_rep_St_to_json(char *json, void *st, int32_t max);
int kr_transponder_path_io_type_to_json(char *json, void *st, int32_t max);
int kr_transponder_info_to_json(char *json, void *st, int32_t max);
int kr_transponder_path_io_path_info_to_json(char *json, void *st, int32_t max);
int kr_transponder_path_io_info_to_json(char *json, void *st, int32_t max);
int kr_transponder_path_info_to_json(char *json, void *st, int32_t max);
