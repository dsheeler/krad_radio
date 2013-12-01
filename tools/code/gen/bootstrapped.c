#include "bootstrapped.h"
#include <string.h>

char *codegen_enum_to_string(cgen_enum val) {
  switch (val) {
    case CGEN_DEVICE_PARAMETERS:
      return "device_parameters";
    case CGEN_ENUM_IO_METHOD:
      return "enum_io_method";
    case CGEN_IO_METHOD:
      return "io_method";
    case CGEN_KR_ALSA:
      return "kr_alsa";
    case CGEN_KRAD_ALSA_SEQ_ST:
      return "krad_alsa_seq_St";
    case CGEN_KR_APP_CLIENT:
      return "kr_app_client";
    case CGEN_KRAD_APP_SHUTDOWN:
      return "krad_app_shutdown";
    case CGEN_KRAD_BROADCAST_MSG_ST:
      return "krad_broadcast_msg_St";
    case CGEN_KR_APP_BROADCASTER:
      return "kr_app_broadcaster";
    case CGEN_KR_APP_SERVER:
      return "kr_app_server";
    case CGEN_KR_APP_SERVER_CLIENT:
      return "kr_app_server_client";
    case CGEN_KR_EASER_UPDATE:
      return "kr_easer_update";
    case CGEN_KR_EASER:
      return "kr_easer";
    case CGEN_KR_EASING:
      return "kr_easing";
    case CGEN_KRAD_TICKER_ST:
      return "krad_ticker_St";
    case CGEN_KR_TIMER:
      return "kr_timer";
    case CGEN_KR_MTYPE_T:
      return "kr_mtype_t";
    case CGEN_KR_CODEME_ST:
      return "kr_codeme_St";
    case CGEN_KR_IMAGE_ST:
      return "kr_image_St";
    case CGEN_KR_AUDIO_ST:
      return "kr_audio_St";
    case CGEN_KR_MEDIUM_ST:
      return "kr_medium_St";
    case CGEN_KR_ENCODER_MACHINE_SECTOR_ST:
      return "kr_encoder_machine_sector_St";
    case CGEN_KR_COMPOSITOR_CONTROL_EASERS:
      return "kr_compositor_control_easers";
    case CGEN_KR_COMPOSITOR_INFO_CB_ARG:
      return "kr_compositor_info_cb_arg";
    case CGEN_KR_COMPOSITOR_SETUP:
      return "kr_compositor_setup";
    case CGEN_KR_COMPOSITOR:
      return "kr_compositor";
    case CGEN_KR_COMPOSITOR_SETTING:
      return "kr_compositor_setting";
    case CGEN_KR_COMPOSITOR_PATH_TYPE:
      return "kr_compositor_path_type";
    case CGEN_KR_COMPOSITOR_SUBUNIT_TYPE:
      return "kr_compositor_subunit_type";
    case CGEN_KR_VECTOR_TYPE:
      return "kr_vector_type";
    case CGEN_KR_COMPOSITOR_CONTROL:
      return "kr_compositor_control";
    case CGEN_KR_COMPOSITOR_CONTROLS:
      return "kr_compositor_controls";
    case CGEN_KR_SPRITE_INFO:
      return "kr_sprite_info";
    case CGEN_KR_TEXT_INFO:
      return "kr_text_info";
    case CGEN_KR_VECTOR_INFO:
      return "kr_vector_info";
    case CGEN_KR_COMPOSITOR_PATH_INFO:
      return "kr_compositor_path_info";
    case CGEN_KR_COMPOSITOR_INFO:
      return "kr_compositor_info";
    case CGEN_COMPOSITOR_PATH_STATE:
      return "compositor_path_state";
    case CGEN_KR_COMPOSITOR_PATH_INFO_CB_ARG:
      return "kr_compositor_path_info_cb_arg";
    case CGEN_KR_COMPOSITOR_PATH_FRAME_CB_ARG:
      return "kr_compositor_path_frame_cb_arg";
    case CGEN_KR_COMPOSITOR_PATH_SETTING:
      return "kr_compositor_path_setting";
    case CGEN_KR_COMPOSITOR_PATH_SETUP:
      return "kr_compositor_path_setup";
    case CGEN_KR_CROP:
      return "kr_crop";
    case CGEN_KR_CONVERT:
      return "kr_convert";
    case CGEN_KR_POS:
      return "kr_pos";
    case CGEN_KR_PERSPECTIVE_VIEW:
      return "kr_perspective_view";
    case CGEN_KR_PERSPECTIVE:
      return "kr_perspective";
    case CGEN_KRAD_CODEC_T:
      return "krad_codec_t";
    case CGEN_KRAD_CONTAINER_TYPE_T:
      return "krad_container_type_t";
    case CGEN_KRAD_CODEC_HEADER_ST:
      return "krad_codec_header_St";
    case CGEN_KRAD_CONTAINER_ST:
      return "krad_container_St";
    case CGEN_KRAD_DECKLINK_ST:
      return "krad_decklink_St";
    case CGEN_KRAD_DECKLINK_CAPTURE_ST:
      return "krad_decklink_capture_St";
    case CGEN_KR_EBML:
      return "kr_ebml";
    case CGEN_KR_FC2_ST:
      return "kr_fc2_St";
    case CGEN_KRAD_FLAC_ST:
      return "krad_flac_St";
    case CGEN_KRAD_FRAME_ST:
      return "krad_frame_St";
    case CGEN_KRAD_FRAMEPOOL_ST:
      return "krad_framepool_St";
    case CGEN_KR_FILE:
      return "kr_file";
    case CGEN_KRAD_IO_MODE_T:
      return "krad_io_mode_t";
    case CGEN_KR_IO2_ST:
      return "kr_io2_St";
    case CGEN_KR_STREAM:
      return "kr_stream";
    case CGEN_KR_JACK_EVENT:
      return "kr_jack_event";
    case CGEN_KR_JACK_PATH_EVENT_CB_ARG:
      return "kr_jack_path_event_cb_arg";
    case CGEN_KR_JACK_PATH_AUDIO_CB_ARG:
      return "kr_jack_path_audio_cb_arg";
    case CGEN_KR_JACK_EVENT_CB_ARG:
      return "kr_jack_event_cb_arg";
    case CGEN_KR_JACK_SETUP:
      return "kr_jack_setup";
    case CGEN_KR_JACK_PATH_SETUP:
      return "kr_jack_path_setup";
    case CGEN_KR_JACK_DIRECTION:
      return "kr_jack_direction";
    case CGEN_KR_JACK_STATE:
      return "kr_jack_state";
    case CGEN_KR_JACK_SETUP_INFO:
      return "kr_jack_setup_info";
    case CGEN_KR_JACK_INFO:
      return "kr_jack_info";
    case CGEN_KR_JACK_PATH_INFO:
      return "kr_jack_path_info";
    case CGEN_COREAUDIO_ST:
      return "coreaudio_St";
    case CGEN_KRAD_LIST_FORMAT_T:
      return "krad_list_format_t";
    case CGEN_KRAD_LIST_PARSE_STATE_T:
      return "krad_list_parse_state_t";
    case CGEN_KRAD_LIST_T:
      return "krad_list_t";
    case CGEN_KRAD_WEBM_DASH_VOD_T:
      return "krad_webm_dash_vod_t";
    case CGEN_KR_POOL_SETUP:
      return "kr_pool_setup";
    case CGEN_KR_MIXER_INFO_CB_ARG:
      return "kr_mixer_info_cb_arg";
    case CGEN_KR_MIXER_PATH_INFO_CB_ARG:
      return "kr_mixer_path_info_cb_arg";
    case CGEN_KR_MIXER_PATH_AUDIO_CB_ARG:
      return "kr_mixer_path_audio_cb_arg";
    case CGEN_KR_MIXER_PATH_SETUP:
      return "kr_mixer_path_setup";
    case CGEN_KR_MIXER_SETUP:
      return "kr_mixer_setup";
    case CGEN_KR_MIXER_CROSSFADER:
      return "kr_mixer_crossfader";
    case CGEN_KR_MIXER_PATH:
      return "kr_mixer_path";
    case CGEN_KR_MIXER_CHANNELS:
      return "kr_mixer_channels";
    case CGEN_KR_MIXER_CONTROL:
      return "kr_mixer_control";
    case CGEN_KR_MIXER_PATH_TYPE:
      return "kr_mixer_path_type";
    case CGEN_KR_MIXER_ADV_CTL:
      return "kr_mixer_adv_ctl";
    case CGEN_KR_MIXER_INFO:
      return "kr_mixer_info";
    case CGEN_KR_MIXER_PATH_INFO:
      return "kr_mixer_path_info";
    case CGEN_KR_MKV_TRACK_ST:
      return "kr_mkv_track_St";
    case CGEN_KR_MKV_ST:
      return "kr_mkv_St";
    case CGEN_KR_MUXER_TRANSPORT_TYPE:
      return "kr_muxer_transport_type";
    case CGEN_KR_MUXER_FILE_OUTPUT_PARAMS_ST:
      return "kr_muxer_file_output_params_St";
    case CGEN_KR_MUXER_STREAM_OUTPUT_PARAMS_ST:
      return "kr_muxer_stream_output_params_St";
    case CGEN_KR_MUXER_TRANSMISSION_OUTPUT_PARAMS_ST:
      return "kr_muxer_transmission_output_params_St";
    case CGEN_KR_MUXER_TRANSPORT_PARAMS:
      return "kr_muxer_transport_params";
    case CGEN_KR_MUXER_OUTPUT_PARAMS_ST:
      return "kr_muxer_output_params_St";
    case CGEN_KRAD_OGG_TRACK_ST:
      return "krad_ogg_track_St";
    case CGEN_KRAD_OGG_ST:
      return "krad_ogg_St";
    case CGEN_KR_OGG:
      return "kr_ogg";
    case CGEN_KR_OGG_IO:
      return "kr_ogg_io";
    case CGEN_KRAD_OPUS_ST:
      return "krad_opus_St";
    case CGEN_KRAD_OSC_ST:
      return "krad_osc_St";
    case CGEN_KR_DECODER_STATE_T:
      return "kr_decoder_state_t";
    case CGEN_KR_DECODER_CMD_T:
      return "kr_decoder_cmd_t";
    case CGEN_KR_DDIR_T:
      return "kr_ddir_t";
    case CGEN_KR_DEMUXER_STATE_T:
      return "kr_demuxer_state_t";
    case CGEN_KR_DEMUXER_CMD_T:
      return "kr_demuxer_cmd_t";
    case CGEN_KR_DEMUXER_PARAMS_ST:
      return "kr_demuxer_params_St";
    case CGEN_KR_MACHINE_PARAMS_ST:
      return "kr_machine_params_St";
    case CGEN_KR_MACHINE_ST:
      return "kr_machine_St";
    case CGEN_KR_MSG_ST:
      return "kr_msg_St";
    case CGEN_KR_MSGPAIR_ST:
      return "kr_msgpair_St";
    case CGEN_KR_MSGSYS_ST:
      return "kr_msgsys_St";
    case CGEN_KR_DIRECTION:
      return "kr_direction";
    case CGEN_KR_PLAYER_PLAYBACK_STATE:
      return "kr_player_playback_state";
    case CGEN_KR_PLAYER_CMD:
      return "kr_player_cmd";
    case CGEN_KR_VIDEO_PARAMS_ST:
      return "kr_video_params_St";
    case CGEN_KR_AUDIO_PARAMS_ST:
      return "kr_audio_params_St";
    case CGEN_KR_MEDIUM_PARAMS_T:
      return "kr_medium_params_t";
    case CGEN_KR_TRACK_INFO_ST:
      return "kr_track_info_St";
    case CGEN_KR_REMOTE_CONTROL:
      return "kr_remote_control";
    case CGEN_KR_LOG:
      return "kr_log";
    case CGEN_KR_RADIO:
      return "kr_radio";
    case CGEN_KR_UNIT_T:
      return "kr_unit_t";
    case CGEN_KR_MIXER_SUBUNIT_T:
      return "kr_mixer_subunit_t";
    case CGEN_KR_TRANSPONDER_SUBUNIT_TYPE_T:
      return "kr_transponder_subunit_type_t";
    case CGEN_KR_STATION_SUBUNIT_T:
      return "kr_station_subunit_t";
    case CGEN_KR_UNIT_CONTROL_DATA_T:
      return "kr_unit_control_data_t";
    case CGEN_KR_UNIT_CONTROL_VALUE_T:
      return "kr_unit_control_value_t";
    case CGEN_KR_TRANSPONDER_CONTROL_T:
      return "kr_transponder_control_t";
    case CGEN_KR_SUBUNIT_T:
      return "kr_subunit_t";
    case CGEN_KR_UNIT_CONTROL_NAME_T:
      return "kr_unit_control_name_t";
    case CGEN_KR_UNIT_PATH_ST:
      return "kr_unit_path_St";
    case CGEN_KR_UNIT_ID_T:
      return "kr_unit_id_t";
    case CGEN_KR_ADDRESS_ST:
      return "kr_address_St";
    case CGEN_KR_UNIT_CONTROL_ST:
      return "kr_unit_control_St";
    case CGEN_KR_REP_PTR_T:
      return "kr_rep_ptr_t";
    case CGEN_KR_REP_ACTUAL_T:
      return "kr_rep_actual_t";
    case CGEN_KR_CRATE_ST:
      return "kr_crate_St";
    case CGEN_KR_CLIENT_ST:
      return "kr_client_St";
    case CGEN_KR_SHM_ST:
      return "kr_shm_St";
    case CGEN_KR_LOG_LEVEL:
      return "kr_log_level";
    case CGEN_KR_LOG_MSG:
      return "kr_log_msg";
    case CGEN_KRAD_RADIO_REP_ST:
      return "krad_radio_rep_St";
    case CGEN_KR_REMOTE_ST:
      return "kr_remote_St";
    case CGEN_KR_TAG_ST:
      return "kr_tag_St";
    case CGEN_KR_RADIO_CLIENT:
      return "kr_radio_client";
    case CGEN_KRAD_SUBUNIT_PTR_T:
      return "krad_subunit_ptr_t";
    case CGEN_KRAD_RC_TYPE_T:
      return "krad_rc_type_t";
    case CGEN_KRAD_RC_ST:
      return "krad_rc_St";
    case CGEN_KRAD_RC_POLOLU_MAESTRO_ST:
      return "krad_rc_pololu_maestro_St";
    case CGEN_KRAD_RC_RX_ST:
      return "krad_rc_rx_St";
    case CGEN_KRAD_RC_SDL_JOY_ST:
      return "krad_rc_sdl_joy_St";
    case CGEN_KRAD_RC_TX_ST:
      return "krad_rc_tx_St";
    case CGEN_KRAD_RESAMPLE_RING_ST:
      return "krad_resample_ring_St";
    case CGEN_KR_ANALOG:
      return "kr_analog";
    case CGEN_KR_EQ_BAND:
      return "kr_eq_band";
    case CGEN_KR_EQ:
      return "kr_eq";
    case CGEN_KR_PASS:
      return "kr_pass";
    case CGEN_KR_SFX_INFO_CB_ARG:
      return "kr_sfx_info_cb_arg";
    case CGEN_KR_SFX_SETUP:
      return "kr_sfx_setup";
    case CGEN_KR_SFX_CONTROL:
      return "kr_sfx_control";
    case CGEN_KR_SFX_EFFECT_TYPE:
      return "kr_sfx_effect_type";
    case CGEN_KR_SFX_EFFECT_CONTROL:
      return "kr_sfx_effect_control";
    case CGEN_KR_EQ_BAND_INFO:
      return "kr_eq_band_info";
    case CGEN_KR_EQ_INFO:
      return "kr_eq_info";
    case CGEN_KR_LOWPASS_INFO:
      return "kr_lowpass_info";
    case CGEN_KR_HIGHPASS_INFO:
      return "kr_highpass_info";
    case CGEN_KR_ANALOG_INFO:
      return "kr_analog_info";
    case CGEN_TONE_T:
      return "tone_t";
    case CGEN_KRAD_TONE_T:
      return "krad_tone_t";
    case CGEN_KRAD_CONTROL_ST:
      return "krad_control_St";
    case CGEN_KRAD_SYSTEM_CPU_MONITOR_ST:
      return "krad_system_cpu_monitor_St";
    case CGEN_KRAD_SYSTEM_ST:
      return "krad_system_St";
    case CGEN_KRAD_TAG_ST:
      return "krad_tag_St";
    case CGEN_KRAD_TAGS_ST:
      return "krad_tags_St";
    case CGEN_KRAD_THEORA_ENCODER_ST:
      return "krad_theora_encoder_St";
    case CGEN_KRAD_THEORA_DECODER_ST:
      return "krad_theora_decoder_St";
    case CGEN_KR_ADAPTER_PATH_EVENT_CB_ARG:
      return "kr_adapter_path_event_cb_arg";
    case CGEN_KR_ADAPTER_PATH_AV_CB_ARG:
      return "kr_adapter_path_av_cb_arg";
    case CGEN_KR_ADAPTER_EVENT_CB_ARG:
      return "kr_adapter_event_cb_arg";
    case CGEN_KR_ADAPTER_PATH_SETUP:
      return "kr_adapter_path_setup";
    case CGEN_KR_ADAPTER_SETUP:
      return "kr_adapter_setup";
    case CGEN_KR_ADAPTER_PATH_DIRECTION:
      return "kr_adapter_path_direction";
    case CGEN_KR_ADAPTER_API:
      return "kr_adapter_api";
    case CGEN_KR_ADAPTER_INFO:
      return "kr_adapter_info";
    case CGEN_KR_ADAPTER_INFO_SUB:
      return "kr_adapter_info_sub";
    case CGEN_KR_ADAPTER_PATH_INFO:
      return "kr_adapter_path_info";
    case CGEN_KR_ADAPTER_PATH_INFO_SUB:
      return "kr_adapter_path_info_sub";
    case CGEN_KR_IMAGE:
      return "kr_image";
    case CGEN_KR_AUDIO:
      return "kr_audio";
    case CGEN_KR_SLICE_ST:
      return "kr_slice_St";
    case CGEN_KR_TRANSPONDER_EVENT_CB_ARG:
      return "kr_transponder_event_cb_arg";
    case CGEN_KR_TRANSPONDER_PATH_EVENT_CB_ARG:
      return "kr_transponder_path_event_cb_arg";
    case CGEN_KR_TRANSPONDER_SETUP:
      return "kr_transponder_setup";
    case CGEN_KR_TRANSPONDER_PATH_SETUP:
      return "kr_transponder_path_setup";
    case CGEN_KR_STAT_ST:
      return "kr_stat_St";
    case CGEN_KRAD_LINK_AV_MODE_T:
      return "krad_link_av_mode_t";
    case CGEN_KR_TXPDR_SU_TYPE_T:
      return "kr_txpdr_su_type_t";
    case CGEN_KRAD_LINK_TRANSPORT_MODE_T:
      return "krad_link_transport_mode_t";
    case CGEN_KRAD_LINK_VIDEO_SOURCE_T:
      return "krad_link_video_source_t";
    case CGEN_KRAD_TRANSPONDER_REP_ST:
      return "krad_transponder_rep_St";
    case CGEN_KR_FLAC_ENCODER_ST:
      return "kr_flac_encoder_St";
    case CGEN_KR_VPX_DECODER_ST:
      return "kr_vpx_decoder_St";
    case CGEN_KR_THEORA_DECODER_ST:
      return "kr_theora_decoder_St";
    case CGEN_KR_DAALA_DECODER_ST:
      return "kr_daala_decoder_St";
    case CGEN_KR_VORBIS_DECODER_ST:
      return "kr_vorbis_decoder_St";
    case CGEN_KR_OPUS_DECODER_ST:
      return "kr_opus_decoder_St";
    case CGEN_KR_FLAC_DECODER_ST:
      return "kr_flac_decoder_St";
    case CGEN_KR_VPX_ENCODER_ST:
      return "kr_vpx_encoder_St";
    case CGEN_KR_THEORA_ENCODER_ST:
      return "kr_theora_encoder_St";
    case CGEN_KR_DAALA_ENCODER_ST:
      return "kr_daala_encoder_St";
    case CGEN_KR_VORBIS_ENCODER_ST:
      return "kr_vorbis_encoder_St";
    case CGEN_KR_OPUS_ENCODER_ST:
      return "kr_opus_encoder_St";
    case CGEN_KR_AUDIO_CODEC_T:
      return "kr_audio_codec_t";
    case CGEN_KR_VIDEO_CODEC_T:
      return "kr_video_codec_t";
    case CGEN_KR_AUDIO_ENCODER_ST:
      return "kr_audio_encoder_St";
    case CGEN_KR_VIDEO_DECODER_ST:
      return "kr_video_decoder_St";
    case CGEN_KR_AUDIO_DECODER_ST:
      return "kr_audio_decoder_St";
    case CGEN_KR_VIDEO_ENCODER_ST:
      return "kr_video_encoder_St";
    case CGEN_KR_AV_ENCODER_T:
      return "kr_av_encoder_t";
    case CGEN_KR_ENCODER_ST:
      return "kr_encoder_St";
    case CGEN_KR_AV_DECODER_T:
      return "kr_av_decoder_t";
    case CGEN_KR_DECODER_ST:
      return "kr_decoder_St";
    case CGEN_KR_UDP_MUXER_ST:
      return "kr_udp_muxer_St";
    case CGEN_KR_OGG_MUXER_ST:
      return "kr_ogg_muxer_St";
    case CGEN_KR_MKV_MUXER_ST:
      return "kr_mkv_muxer_St";
    case CGEN_KR_TRANSOGG_MUXER_ST:
      return "kr_transogg_muxer_St";
    case CGEN_KR_UDP_DEMUXER_ST:
      return "kr_udp_demuxer_St";
    case CGEN_KR_OGG_DEMUXER_ST:
      return "kr_ogg_demuxer_St";
    case CGEN_KR_MKV_DEMUXER_ST:
      return "kr_mkv_demuxer_St";
    case CGEN_KR_TRANSOGG_DEMUXER_ST:
      return "kr_transogg_demuxer_St";
    case CGEN_KR_MUXER_ACTUAL_T:
      return "kr_muxer_actual_t";
    case CGEN_KR_MUXER_ST:
      return "kr_muxer_St";
    case CGEN_KR_DEMUXER_ACTUAL_T:
      return "kr_demuxer_actual_t";
    case CGEN_KR_DEMUXER_ST:
      return "kr_demuxer_St";
    case CGEN_KR_RAWIN_ST:
      return "kr_rawin_St";
    case CGEN_KR_RAWOUT_ST:
      return "kr_rawout_St";
    case CGEN_KR_TRANSPONDER_SUBUNIT_ACTUAL_T:
      return "kr_transponder_subunit_actual_t";
    case CGEN_KRAD_TRANSPONDER_SUBUNIT_REP_ST:
      return "krad_transponder_subunit_rep_St";
    case CGEN_KR_TRANSPONDER_PATH_IO_TYPE:
      return "kr_transponder_path_io_type";
    case CGEN_KR_TRANSPONDER_INFO:
      return "kr_transponder_info";
    case CGEN_KR_TRANSPONDER_PATH_IO_INFO:
      return "kr_transponder_path_io_info";
    case CGEN_KR_TRANSPONDER_PATH_IO_INFO_SUB:
      return "kr_transponder_path_io_info_sub";
    case CGEN_KR_TRANSPONDER_PATH_INFO:
      return "kr_transponder_path_info";
    case CGEN_KRAD_SLICE_TRACK_TYPE_T:
      return "krad_slice_track_type_t";
    case CGEN_KRAD_SUBSLICE_ST:
      return "krad_subslice_St";
    case CGEN_KRAD_SLICER_ST:
      return "krad_slicer_St";
    case CGEN_KRAD_REBUILDER_TRACK_ST:
      return "krad_rebuilder_track_St";
    case CGEN_KRAD_REBUILDER_ST:
      return "krad_rebuilder_St";
    case CGEN_KRAD_UDP_ST:
      return "krad_udp_St";
    case CGEN_KR_V4L2_SETUP:
      return "kr_v4l2_setup";
    case CGEN_KR_V4L2_STATE:
      return "kr_v4l2_state";
    case CGEN_KR_V4L2_MODE:
      return "kr_v4l2_mode";
    case CGEN_KR_V4L2_INFO:
      return "kr_v4l2_info";
    case CGEN_KR_V4L2_OPEN_INFO:
      return "kr_v4l2_open_info";
    case CGEN_KRAD_VORBIS_ST:
      return "krad_vorbis_St";
    case CGEN_KRAD_VPX_ENCODER_ST:
      return "krad_vpx_encoder_St";
    case CGEN_KRAD_VPX_DECODER_ST:
      return "krad_vpx_decoder_St";
    case CGEN_KR_WAYLAND_EVENT_TYPE:
      return "kr_wayland_event_type";
    case CGEN_KR_WAYLAND_POINTER_EVENT:
      return "kr_wayland_pointer_event";
    case CGEN_KR_WAYLAND_KEY_EVENT:
      return "kr_wayland_key_event";
    case CGEN_KR_WAYLAND_FRAME_EVENT:
      return "kr_wayland_frame_event";
    case CGEN_KR_WAYLAND_EVENT:
      return "kr_wayland_event";
    case CGEN_KR_WAYLAND_PATH_SETUP:
      return "kr_wayland_path_setup";
    case CGEN_KR_WAYLAND_SETUP:
      return "kr_wayland_setup";
    case CGEN_KR_WAYLAND_INFO:
      return "kr_wayland_info";
    case CGEN_KR_WAYLAND_PATH_INFO:
      return "kr_wayland_path_info";
    case CGEN_KRAD_INTERWEB_SHUTDOWN:
      return "krad_interweb_shutdown";
    case CGEN_KRAD_INTERWEB_SERVER_ST:
      return "krad_interweb_server_St";
    case CGEN_INTERWEB_CLIENT_TYPE:
      return "interweb_client_type";
    case CGEN_INTERWEB_VERB:
      return "interweb_verb";
    case CGEN_INTERWEBS_ST:
      return "interwebs_St";
    case CGEN_KRAD_INTERWEB_SERVER_CLIENT_ST:
      return "krad_interweb_server_client_St";
    case CGEN_KR_X11:
      return "kr_x11";
    case CGEN_KRAD_XMMS_PLAYBACK_CMD_T:
      return "krad_xmms_playback_cmd_t";
    case CGEN_KRAD_XMMS_ST:
      return "krad_xmms_St";
    case CGEN_KRAD_Y4M_ST:
      return "krad_y4m_St";
  }
  return "Unknown";
}

cgen_enum codegen_string_to_enum(char *string) {
  if (!strcmp(string,"device_parameters")) {
    return CGEN_DEVICE_PARAMETERS;
  }
  if (!strcmp(string,"enum_io_method")) {
    return CGEN_ENUM_IO_METHOD;
  }
  if (!strcmp(string,"io_method")) {
    return CGEN_IO_METHOD;
  }
  if (!strcmp(string,"kr_alsa")) {
    return CGEN_KR_ALSA;
  }
  if (!strcmp(string,"krad_alsa_seq_St")) {
    return CGEN_KRAD_ALSA_SEQ_ST;
  }
  if (!strcmp(string,"kr_app_client")) {
    return CGEN_KR_APP_CLIENT;
  }
  if (!strcmp(string,"krad_app_shutdown")) {
    return CGEN_KRAD_APP_SHUTDOWN;
  }
  if (!strcmp(string,"krad_broadcast_msg_St")) {
    return CGEN_KRAD_BROADCAST_MSG_ST;
  }
  if (!strcmp(string,"kr_app_broadcaster")) {
    return CGEN_KR_APP_BROADCASTER;
  }
  if (!strcmp(string,"kr_app_server")) {
    return CGEN_KR_APP_SERVER;
  }
  if (!strcmp(string,"kr_app_server_client")) {
    return CGEN_KR_APP_SERVER_CLIENT;
  }
  if (!strcmp(string,"kr_easer_update")) {
    return CGEN_KR_EASER_UPDATE;
  }
  if (!strcmp(string,"kr_easer")) {
    return CGEN_KR_EASER;
  }
  if (!strcmp(string,"kr_easing")) {
    return CGEN_KR_EASING;
  }
  if (!strcmp(string,"krad_ticker_St")) {
    return CGEN_KRAD_TICKER_ST;
  }
  if (!strcmp(string,"kr_timer")) {
    return CGEN_KR_TIMER;
  }
  if (!strcmp(string,"kr_mtype_t")) {
    return CGEN_KR_MTYPE_T;
  }
  if (!strcmp(string,"kr_codeme_St")) {
    return CGEN_KR_CODEME_ST;
  }
  if (!strcmp(string,"kr_image_St")) {
    return CGEN_KR_IMAGE_ST;
  }
  if (!strcmp(string,"kr_audio_St")) {
    return CGEN_KR_AUDIO_ST;
  }
  if (!strcmp(string,"kr_medium_St")) {
    return CGEN_KR_MEDIUM_ST;
  }
  if (!strcmp(string,"kr_encoder_machine_sector_St")) {
    return CGEN_KR_ENCODER_MACHINE_SECTOR_ST;
  }
  if (!strcmp(string,"kr_compositor_control_easers")) {
    return CGEN_KR_COMPOSITOR_CONTROL_EASERS;
  }
  if (!strcmp(string,"kr_compositor_info_cb_arg")) {
    return CGEN_KR_COMPOSITOR_INFO_CB_ARG;
  }
  if (!strcmp(string,"kr_compositor_setup")) {
    return CGEN_KR_COMPOSITOR_SETUP;
  }
  if (!strcmp(string,"kr_compositor")) {
    return CGEN_KR_COMPOSITOR;
  }
  if (!strcmp(string,"kr_compositor_setting")) {
    return CGEN_KR_COMPOSITOR_SETTING;
  }
  if (!strcmp(string,"kr_compositor_path_type")) {
    return CGEN_KR_COMPOSITOR_PATH_TYPE;
  }
  if (!strcmp(string,"kr_compositor_subunit_type")) {
    return CGEN_KR_COMPOSITOR_SUBUNIT_TYPE;
  }
  if (!strcmp(string,"kr_vector_type")) {
    return CGEN_KR_VECTOR_TYPE;
  }
  if (!strcmp(string,"kr_compositor_control")) {
    return CGEN_KR_COMPOSITOR_CONTROL;
  }
  if (!strcmp(string,"kr_compositor_controls")) {
    return CGEN_KR_COMPOSITOR_CONTROLS;
  }
  if (!strcmp(string,"kr_sprite_info")) {
    return CGEN_KR_SPRITE_INFO;
  }
  if (!strcmp(string,"kr_text_info")) {
    return CGEN_KR_TEXT_INFO;
  }
  if (!strcmp(string,"kr_vector_info")) {
    return CGEN_KR_VECTOR_INFO;
  }
  if (!strcmp(string,"kr_compositor_path_info")) {
    return CGEN_KR_COMPOSITOR_PATH_INFO;
  }
  if (!strcmp(string,"kr_compositor_info")) {
    return CGEN_KR_COMPOSITOR_INFO;
  }
  if (!strcmp(string,"compositor_path_state")) {
    return CGEN_COMPOSITOR_PATH_STATE;
  }
  if (!strcmp(string,"kr_compositor_path_info_cb_arg")) {
    return CGEN_KR_COMPOSITOR_PATH_INFO_CB_ARG;
  }
  if (!strcmp(string,"kr_compositor_path_frame_cb_arg")) {
    return CGEN_KR_COMPOSITOR_PATH_FRAME_CB_ARG;
  }
  if (!strcmp(string,"kr_compositor_path_setting")) {
    return CGEN_KR_COMPOSITOR_PATH_SETTING;
  }
  if (!strcmp(string,"kr_compositor_path_setup")) {
    return CGEN_KR_COMPOSITOR_PATH_SETUP;
  }
  if (!strcmp(string,"kr_crop")) {
    return CGEN_KR_CROP;
  }
  if (!strcmp(string,"kr_convert")) {
    return CGEN_KR_CONVERT;
  }
  if (!strcmp(string,"kr_pos")) {
    return CGEN_KR_POS;
  }
  if (!strcmp(string,"kr_perspective_view")) {
    return CGEN_KR_PERSPECTIVE_VIEW;
  }
  if (!strcmp(string,"kr_perspective")) {
    return CGEN_KR_PERSPECTIVE;
  }
  if (!strcmp(string,"krad_codec_t")) {
    return CGEN_KRAD_CODEC_T;
  }
  if (!strcmp(string,"krad_container_type_t")) {
    return CGEN_KRAD_CONTAINER_TYPE_T;
  }
  if (!strcmp(string,"krad_codec_header_St")) {
    return CGEN_KRAD_CODEC_HEADER_ST;
  }
  if (!strcmp(string,"krad_container_St")) {
    return CGEN_KRAD_CONTAINER_ST;
  }
  if (!strcmp(string,"krad_decklink_St")) {
    return CGEN_KRAD_DECKLINK_ST;
  }
  if (!strcmp(string,"krad_decklink_capture_St")) {
    return CGEN_KRAD_DECKLINK_CAPTURE_ST;
  }
  if (!strcmp(string,"kr_ebml")) {
    return CGEN_KR_EBML;
  }
  if (!strcmp(string,"kr_fc2_St")) {
    return CGEN_KR_FC2_ST;
  }
  if (!strcmp(string,"krad_flac_St")) {
    return CGEN_KRAD_FLAC_ST;
  }
  if (!strcmp(string,"krad_frame_St")) {
    return CGEN_KRAD_FRAME_ST;
  }
  if (!strcmp(string,"krad_framepool_St")) {
    return CGEN_KRAD_FRAMEPOOL_ST;
  }
  if (!strcmp(string,"kr_file")) {
    return CGEN_KR_FILE;
  }
  if (!strcmp(string,"krad_io_mode_t")) {
    return CGEN_KRAD_IO_MODE_T;
  }
  if (!strcmp(string,"kr_io2_St")) {
    return CGEN_KR_IO2_ST;
  }
  if (!strcmp(string,"kr_stream")) {
    return CGEN_KR_STREAM;
  }
  if (!strcmp(string,"kr_jack_event")) {
    return CGEN_KR_JACK_EVENT;
  }
  if (!strcmp(string,"kr_jack_path_event_cb_arg")) {
    return CGEN_KR_JACK_PATH_EVENT_CB_ARG;
  }
  if (!strcmp(string,"kr_jack_path_audio_cb_arg")) {
    return CGEN_KR_JACK_PATH_AUDIO_CB_ARG;
  }
  if (!strcmp(string,"kr_jack_event_cb_arg")) {
    return CGEN_KR_JACK_EVENT_CB_ARG;
  }
  if (!strcmp(string,"kr_jack_setup")) {
    return CGEN_KR_JACK_SETUP;
  }
  if (!strcmp(string,"kr_jack_path_setup")) {
    return CGEN_KR_JACK_PATH_SETUP;
  }
  if (!strcmp(string,"kr_jack_direction")) {
    return CGEN_KR_JACK_DIRECTION;
  }
  if (!strcmp(string,"kr_jack_state")) {
    return CGEN_KR_JACK_STATE;
  }
  if (!strcmp(string,"kr_jack_setup_info")) {
    return CGEN_KR_JACK_SETUP_INFO;
  }
  if (!strcmp(string,"kr_jack_info")) {
    return CGEN_KR_JACK_INFO;
  }
  if (!strcmp(string,"kr_jack_path_info")) {
    return CGEN_KR_JACK_PATH_INFO;
  }
  if (!strcmp(string,"coreaudio_St")) {
    return CGEN_COREAUDIO_ST;
  }
  if (!strcmp(string,"krad_list_format_t")) {
    return CGEN_KRAD_LIST_FORMAT_T;
  }
  if (!strcmp(string,"krad_list_parse_state_t")) {
    return CGEN_KRAD_LIST_PARSE_STATE_T;
  }
  if (!strcmp(string,"krad_list_t")) {
    return CGEN_KRAD_LIST_T;
  }
  if (!strcmp(string,"krad_webm_dash_vod_t")) {
    return CGEN_KRAD_WEBM_DASH_VOD_T;
  }
  if (!strcmp(string,"kr_pool_setup")) {
    return CGEN_KR_POOL_SETUP;
  }
  if (!strcmp(string,"kr_mixer_info_cb_arg")) {
    return CGEN_KR_MIXER_INFO_CB_ARG;
  }
  if (!strcmp(string,"kr_mixer_path_info_cb_arg")) {
    return CGEN_KR_MIXER_PATH_INFO_CB_ARG;
  }
  if (!strcmp(string,"kr_mixer_path_audio_cb_arg")) {
    return CGEN_KR_MIXER_PATH_AUDIO_CB_ARG;
  }
  if (!strcmp(string,"kr_mixer_path_setup")) {
    return CGEN_KR_MIXER_PATH_SETUP;
  }
  if (!strcmp(string,"kr_mixer_setup")) {
    return CGEN_KR_MIXER_SETUP;
  }
  if (!strcmp(string,"kr_mixer_crossfader")) {
    return CGEN_KR_MIXER_CROSSFADER;
  }
  if (!strcmp(string,"kr_mixer_path")) {
    return CGEN_KR_MIXER_PATH;
  }
  if (!strcmp(string,"kr_mixer_channels")) {
    return CGEN_KR_MIXER_CHANNELS;
  }
  if (!strcmp(string,"kr_mixer_control")) {
    return CGEN_KR_MIXER_CONTROL;
  }
  if (!strcmp(string,"kr_mixer_path_type")) {
    return CGEN_KR_MIXER_PATH_TYPE;
  }
  if (!strcmp(string,"kr_mixer_adv_ctl")) {
    return CGEN_KR_MIXER_ADV_CTL;
  }
  if (!strcmp(string,"kr_mixer_info")) {
    return CGEN_KR_MIXER_INFO;
  }
  if (!strcmp(string,"kr_mixer_path_info")) {
    return CGEN_KR_MIXER_PATH_INFO;
  }
  if (!strcmp(string,"kr_mkv_track_St")) {
    return CGEN_KR_MKV_TRACK_ST;
  }
  if (!strcmp(string,"kr_mkv_St")) {
    return CGEN_KR_MKV_ST;
  }
  if (!strcmp(string,"kr_muxer_transport_type")) {
    return CGEN_KR_MUXER_TRANSPORT_TYPE;
  }
  if (!strcmp(string,"kr_muxer_file_output_params_St")) {
    return CGEN_KR_MUXER_FILE_OUTPUT_PARAMS_ST;
  }
  if (!strcmp(string,"kr_muxer_stream_output_params_St")) {
    return CGEN_KR_MUXER_STREAM_OUTPUT_PARAMS_ST;
  }
  if (!strcmp(string,"kr_muxer_transmission_output_params_St")) {
    return CGEN_KR_MUXER_TRANSMISSION_OUTPUT_PARAMS_ST;
  }
  if (!strcmp(string,"kr_muxer_transport_params")) {
    return CGEN_KR_MUXER_TRANSPORT_PARAMS;
  }
  if (!strcmp(string,"kr_muxer_output_params_St")) {
    return CGEN_KR_MUXER_OUTPUT_PARAMS_ST;
  }
  if (!strcmp(string,"krad_ogg_track_St")) {
    return CGEN_KRAD_OGG_TRACK_ST;
  }
  if (!strcmp(string,"krad_ogg_St")) {
    return CGEN_KRAD_OGG_ST;
  }
  if (!strcmp(string,"kr_ogg")) {
    return CGEN_KR_OGG;
  }
  if (!strcmp(string,"kr_ogg_io")) {
    return CGEN_KR_OGG_IO;
  }
  if (!strcmp(string,"krad_opus_St")) {
    return CGEN_KRAD_OPUS_ST;
  }
  if (!strcmp(string,"krad_osc_St")) {
    return CGEN_KRAD_OSC_ST;
  }
  if (!strcmp(string,"kr_decoder_state_t")) {
    return CGEN_KR_DECODER_STATE_T;
  }
  if (!strcmp(string,"kr_decoder_cmd_t")) {
    return CGEN_KR_DECODER_CMD_T;
  }
  if (!strcmp(string,"kr_ddir_t")) {
    return CGEN_KR_DDIR_T;
  }
  if (!strcmp(string,"kr_demuxer_state_t")) {
    return CGEN_KR_DEMUXER_STATE_T;
  }
  if (!strcmp(string,"kr_demuxer_cmd_t")) {
    return CGEN_KR_DEMUXER_CMD_T;
  }
  if (!strcmp(string,"kr_demuxer_params_St")) {
    return CGEN_KR_DEMUXER_PARAMS_ST;
  }
  if (!strcmp(string,"kr_machine_params_St")) {
    return CGEN_KR_MACHINE_PARAMS_ST;
  }
  if (!strcmp(string,"kr_machine_St")) {
    return CGEN_KR_MACHINE_ST;
  }
  if (!strcmp(string,"kr_msg_St")) {
    return CGEN_KR_MSG_ST;
  }
  if (!strcmp(string,"kr_msgpair_St")) {
    return CGEN_KR_MSGPAIR_ST;
  }
  if (!strcmp(string,"kr_msgsys_St")) {
    return CGEN_KR_MSGSYS_ST;
  }
  if (!strcmp(string,"kr_direction")) {
    return CGEN_KR_DIRECTION;
  }
  if (!strcmp(string,"kr_player_playback_state")) {
    return CGEN_KR_PLAYER_PLAYBACK_STATE;
  }
  if (!strcmp(string,"kr_player_cmd")) {
    return CGEN_KR_PLAYER_CMD;
  }
  if (!strcmp(string,"kr_video_params_St")) {
    return CGEN_KR_VIDEO_PARAMS_ST;
  }
  if (!strcmp(string,"kr_audio_params_St")) {
    return CGEN_KR_AUDIO_PARAMS_ST;
  }
  if (!strcmp(string,"kr_medium_params_t")) {
    return CGEN_KR_MEDIUM_PARAMS_T;
  }
  if (!strcmp(string,"kr_track_info_St")) {
    return CGEN_KR_TRACK_INFO_ST;
  }
  if (!strcmp(string,"kr_remote_control")) {
    return CGEN_KR_REMOTE_CONTROL;
  }
  if (!strcmp(string,"kr_log")) {
    return CGEN_KR_LOG;
  }
  if (!strcmp(string,"kr_radio")) {
    return CGEN_KR_RADIO;
  }
  if (!strcmp(string,"kr_unit_t")) {
    return CGEN_KR_UNIT_T;
  }
  if (!strcmp(string,"kr_mixer_subunit_t")) {
    return CGEN_KR_MIXER_SUBUNIT_T;
  }
  if (!strcmp(string,"kr_transponder_subunit_type_t")) {
    return CGEN_KR_TRANSPONDER_SUBUNIT_TYPE_T;
  }
  if (!strcmp(string,"kr_station_subunit_t")) {
    return CGEN_KR_STATION_SUBUNIT_T;
  }
  if (!strcmp(string,"kr_unit_control_data_t")) {
    return CGEN_KR_UNIT_CONTROL_DATA_T;
  }
  if (!strcmp(string,"kr_unit_control_value_t")) {
    return CGEN_KR_UNIT_CONTROL_VALUE_T;
  }
  if (!strcmp(string,"kr_transponder_control_t")) {
    return CGEN_KR_TRANSPONDER_CONTROL_T;
  }
  if (!strcmp(string,"kr_subunit_t")) {
    return CGEN_KR_SUBUNIT_T;
  }
  if (!strcmp(string,"kr_unit_control_name_t")) {
    return CGEN_KR_UNIT_CONTROL_NAME_T;
  }
  if (!strcmp(string,"kr_unit_path_St")) {
    return CGEN_KR_UNIT_PATH_ST;
  }
  if (!strcmp(string,"kr_unit_id_t")) {
    return CGEN_KR_UNIT_ID_T;
  }
  if (!strcmp(string,"kr_address_St")) {
    return CGEN_KR_ADDRESS_ST;
  }
  if (!strcmp(string,"kr_unit_control_St")) {
    return CGEN_KR_UNIT_CONTROL_ST;
  }
  if (!strcmp(string,"kr_rep_ptr_t")) {
    return CGEN_KR_REP_PTR_T;
  }
  if (!strcmp(string,"kr_rep_actual_t")) {
    return CGEN_KR_REP_ACTUAL_T;
  }
  if (!strcmp(string,"kr_crate_St")) {
    return CGEN_KR_CRATE_ST;
  }
  if (!strcmp(string,"kr_client_St")) {
    return CGEN_KR_CLIENT_ST;
  }
  if (!strcmp(string,"kr_shm_St")) {
    return CGEN_KR_SHM_ST;
  }
  if (!strcmp(string,"kr_log_level")) {
    return CGEN_KR_LOG_LEVEL;
  }
  if (!strcmp(string,"kr_log_msg")) {
    return CGEN_KR_LOG_MSG;
  }
  if (!strcmp(string,"krad_radio_rep_St")) {
    return CGEN_KRAD_RADIO_REP_ST;
  }
  if (!strcmp(string,"kr_remote_St")) {
    return CGEN_KR_REMOTE_ST;
  }
  if (!strcmp(string,"kr_tag_St")) {
    return CGEN_KR_TAG_ST;
  }
  if (!strcmp(string,"kr_radio_client")) {
    return CGEN_KR_RADIO_CLIENT;
  }
  if (!strcmp(string,"krad_subunit_ptr_t")) {
    return CGEN_KRAD_SUBUNIT_PTR_T;
  }
  if (!strcmp(string,"krad_rc_type_t")) {
    return CGEN_KRAD_RC_TYPE_T;
  }
  if (!strcmp(string,"krad_rc_St")) {
    return CGEN_KRAD_RC_ST;
  }
  if (!strcmp(string,"krad_rc_pololu_maestro_St")) {
    return CGEN_KRAD_RC_POLOLU_MAESTRO_ST;
  }
  if (!strcmp(string,"krad_rc_rx_St")) {
    return CGEN_KRAD_RC_RX_ST;
  }
  if (!strcmp(string,"krad_rc_sdl_joy_St")) {
    return CGEN_KRAD_RC_SDL_JOY_ST;
  }
  if (!strcmp(string,"krad_rc_tx_St")) {
    return CGEN_KRAD_RC_TX_ST;
  }
  if (!strcmp(string,"krad_resample_ring_St")) {
    return CGEN_KRAD_RESAMPLE_RING_ST;
  }
  if (!strcmp(string,"kr_analog")) {
    return CGEN_KR_ANALOG;
  }
  if (!strcmp(string,"kr_eq_band")) {
    return CGEN_KR_EQ_BAND;
  }
  if (!strcmp(string,"kr_eq")) {
    return CGEN_KR_EQ;
  }
  if (!strcmp(string,"kr_pass")) {
    return CGEN_KR_PASS;
  }
  if (!strcmp(string,"kr_sfx_info_cb_arg")) {
    return CGEN_KR_SFX_INFO_CB_ARG;
  }
  if (!strcmp(string,"kr_sfx_setup")) {
    return CGEN_KR_SFX_SETUP;
  }
  if (!strcmp(string,"kr_sfx_control")) {
    return CGEN_KR_SFX_CONTROL;
  }
  if (!strcmp(string,"kr_sfx_effect_type")) {
    return CGEN_KR_SFX_EFFECT_TYPE;
  }
  if (!strcmp(string,"kr_sfx_effect_control")) {
    return CGEN_KR_SFX_EFFECT_CONTROL;
  }
  if (!strcmp(string,"kr_eq_band_info")) {
    return CGEN_KR_EQ_BAND_INFO;
  }
  if (!strcmp(string,"kr_eq_info")) {
    return CGEN_KR_EQ_INFO;
  }
  if (!strcmp(string,"kr_lowpass_info")) {
    return CGEN_KR_LOWPASS_INFO;
  }
  if (!strcmp(string,"kr_highpass_info")) {
    return CGEN_KR_HIGHPASS_INFO;
  }
  if (!strcmp(string,"kr_analog_info")) {
    return CGEN_KR_ANALOG_INFO;
  }
  if (!strcmp(string,"tone_t")) {
    return CGEN_TONE_T;
  }
  if (!strcmp(string,"krad_tone_t")) {
    return CGEN_KRAD_TONE_T;
  }
  if (!strcmp(string,"krad_control_St")) {
    return CGEN_KRAD_CONTROL_ST;
  }
  if (!strcmp(string,"krad_system_cpu_monitor_St")) {
    return CGEN_KRAD_SYSTEM_CPU_MONITOR_ST;
  }
  if (!strcmp(string,"krad_system_St")) {
    return CGEN_KRAD_SYSTEM_ST;
  }
  if (!strcmp(string,"krad_tag_St")) {
    return CGEN_KRAD_TAG_ST;
  }
  if (!strcmp(string,"krad_tags_St")) {
    return CGEN_KRAD_TAGS_ST;
  }
  if (!strcmp(string,"krad_theora_encoder_St")) {
    return CGEN_KRAD_THEORA_ENCODER_ST;
  }
  if (!strcmp(string,"krad_theora_decoder_St")) {
    return CGEN_KRAD_THEORA_DECODER_ST;
  }
  if (!strcmp(string,"kr_adapter_path_event_cb_arg")) {
    return CGEN_KR_ADAPTER_PATH_EVENT_CB_ARG;
  }
  if (!strcmp(string,"kr_adapter_path_av_cb_arg")) {
    return CGEN_KR_ADAPTER_PATH_AV_CB_ARG;
  }
  if (!strcmp(string,"kr_adapter_event_cb_arg")) {
    return CGEN_KR_ADAPTER_EVENT_CB_ARG;
  }
  if (!strcmp(string,"kr_adapter_path_setup")) {
    return CGEN_KR_ADAPTER_PATH_SETUP;
  }
  if (!strcmp(string,"kr_adapter_setup")) {
    return CGEN_KR_ADAPTER_SETUP;
  }
  if (!strcmp(string,"kr_adapter_path_direction")) {
    return CGEN_KR_ADAPTER_PATH_DIRECTION;
  }
  if (!strcmp(string,"kr_adapter_api")) {
    return CGEN_KR_ADAPTER_API;
  }
  if (!strcmp(string,"kr_adapter_info")) {
    return CGEN_KR_ADAPTER_INFO;
  }
  if (!strcmp(string,"kr_adapter_info_sub")) {
    return CGEN_KR_ADAPTER_INFO_SUB;
  }
  if (!strcmp(string,"kr_adapter_path_info")) {
    return CGEN_KR_ADAPTER_PATH_INFO;
  }
  if (!strcmp(string,"kr_adapter_path_info_sub")) {
    return CGEN_KR_ADAPTER_PATH_INFO_SUB;
  }
  if (!strcmp(string,"kr_image")) {
    return CGEN_KR_IMAGE;
  }
  if (!strcmp(string,"kr_audio")) {
    return CGEN_KR_AUDIO;
  }
  if (!strcmp(string,"kr_slice_St")) {
    return CGEN_KR_SLICE_ST;
  }
  if (!strcmp(string,"kr_transponder_event_cb_arg")) {
    return CGEN_KR_TRANSPONDER_EVENT_CB_ARG;
  }
  if (!strcmp(string,"kr_transponder_path_event_cb_arg")) {
    return CGEN_KR_TRANSPONDER_PATH_EVENT_CB_ARG;
  }
  if (!strcmp(string,"kr_transponder_setup")) {
    return CGEN_KR_TRANSPONDER_SETUP;
  }
  if (!strcmp(string,"kr_transponder_path_setup")) {
    return CGEN_KR_TRANSPONDER_PATH_SETUP;
  }
  if (!strcmp(string,"kr_stat_St")) {
    return CGEN_KR_STAT_ST;
  }
  if (!strcmp(string,"krad_link_av_mode_t")) {
    return CGEN_KRAD_LINK_AV_MODE_T;
  }
  if (!strcmp(string,"kr_txpdr_su_type_t")) {
    return CGEN_KR_TXPDR_SU_TYPE_T;
  }
  if (!strcmp(string,"krad_link_transport_mode_t")) {
    return CGEN_KRAD_LINK_TRANSPORT_MODE_T;
  }
  if (!strcmp(string,"krad_link_video_source_t")) {
    return CGEN_KRAD_LINK_VIDEO_SOURCE_T;
  }
  if (!strcmp(string,"krad_transponder_rep_St")) {
    return CGEN_KRAD_TRANSPONDER_REP_ST;
  }
  if (!strcmp(string,"kr_flac_encoder_St")) {
    return CGEN_KR_FLAC_ENCODER_ST;
  }
  if (!strcmp(string,"kr_vpx_decoder_St")) {
    return CGEN_KR_VPX_DECODER_ST;
  }
  if (!strcmp(string,"kr_theora_decoder_St")) {
    return CGEN_KR_THEORA_DECODER_ST;
  }
  if (!strcmp(string,"kr_daala_decoder_St")) {
    return CGEN_KR_DAALA_DECODER_ST;
  }
  if (!strcmp(string,"kr_vorbis_decoder_St")) {
    return CGEN_KR_VORBIS_DECODER_ST;
  }
  if (!strcmp(string,"kr_opus_decoder_St")) {
    return CGEN_KR_OPUS_DECODER_ST;
  }
  if (!strcmp(string,"kr_flac_decoder_St")) {
    return CGEN_KR_FLAC_DECODER_ST;
  }
  if (!strcmp(string,"kr_vpx_encoder_St")) {
    return CGEN_KR_VPX_ENCODER_ST;
  }
  if (!strcmp(string,"kr_theora_encoder_St")) {
    return CGEN_KR_THEORA_ENCODER_ST;
  }
  if (!strcmp(string,"kr_daala_encoder_St")) {
    return CGEN_KR_DAALA_ENCODER_ST;
  }
  if (!strcmp(string,"kr_vorbis_encoder_St")) {
    return CGEN_KR_VORBIS_ENCODER_ST;
  }
  if (!strcmp(string,"kr_opus_encoder_St")) {
    return CGEN_KR_OPUS_ENCODER_ST;
  }
  if (!strcmp(string,"kr_audio_codec_t")) {
    return CGEN_KR_AUDIO_CODEC_T;
  }
  if (!strcmp(string,"kr_video_codec_t")) {
    return CGEN_KR_VIDEO_CODEC_T;
  }
  if (!strcmp(string,"kr_audio_encoder_St")) {
    return CGEN_KR_AUDIO_ENCODER_ST;
  }
  if (!strcmp(string,"kr_video_decoder_St")) {
    return CGEN_KR_VIDEO_DECODER_ST;
  }
  if (!strcmp(string,"kr_audio_decoder_St")) {
    return CGEN_KR_AUDIO_DECODER_ST;
  }
  if (!strcmp(string,"kr_video_encoder_St")) {
    return CGEN_KR_VIDEO_ENCODER_ST;
  }
  if (!strcmp(string,"kr_av_encoder_t")) {
    return CGEN_KR_AV_ENCODER_T;
  }
  if (!strcmp(string,"kr_encoder_St")) {
    return CGEN_KR_ENCODER_ST;
  }
  if (!strcmp(string,"kr_av_decoder_t")) {
    return CGEN_KR_AV_DECODER_T;
  }
  if (!strcmp(string,"kr_decoder_St")) {
    return CGEN_KR_DECODER_ST;
  }
  if (!strcmp(string,"kr_udp_muxer_St")) {
    return CGEN_KR_UDP_MUXER_ST;
  }
  if (!strcmp(string,"kr_ogg_muxer_St")) {
    return CGEN_KR_OGG_MUXER_ST;
  }
  if (!strcmp(string,"kr_mkv_muxer_St")) {
    return CGEN_KR_MKV_MUXER_ST;
  }
  if (!strcmp(string,"kr_transogg_muxer_St")) {
    return CGEN_KR_TRANSOGG_MUXER_ST;
  }
  if (!strcmp(string,"kr_udp_demuxer_St")) {
    return CGEN_KR_UDP_DEMUXER_ST;
  }
  if (!strcmp(string,"kr_ogg_demuxer_St")) {
    return CGEN_KR_OGG_DEMUXER_ST;
  }
  if (!strcmp(string,"kr_mkv_demuxer_St")) {
    return CGEN_KR_MKV_DEMUXER_ST;
  }
  if (!strcmp(string,"kr_transogg_demuxer_St")) {
    return CGEN_KR_TRANSOGG_DEMUXER_ST;
  }
  if (!strcmp(string,"kr_muxer_actual_t")) {
    return CGEN_KR_MUXER_ACTUAL_T;
  }
  if (!strcmp(string,"kr_muxer_St")) {
    return CGEN_KR_MUXER_ST;
  }
  if (!strcmp(string,"kr_demuxer_actual_t")) {
    return CGEN_KR_DEMUXER_ACTUAL_T;
  }
  if (!strcmp(string,"kr_demuxer_St")) {
    return CGEN_KR_DEMUXER_ST;
  }
  if (!strcmp(string,"kr_rawin_St")) {
    return CGEN_KR_RAWIN_ST;
  }
  if (!strcmp(string,"kr_rawout_St")) {
    return CGEN_KR_RAWOUT_ST;
  }
  if (!strcmp(string,"kr_transponder_subunit_actual_t")) {
    return CGEN_KR_TRANSPONDER_SUBUNIT_ACTUAL_T;
  }
  if (!strcmp(string,"krad_transponder_subunit_rep_St")) {
    return CGEN_KRAD_TRANSPONDER_SUBUNIT_REP_ST;
  }
  if (!strcmp(string,"kr_transponder_path_io_type")) {
    return CGEN_KR_TRANSPONDER_PATH_IO_TYPE;
  }
  if (!strcmp(string,"kr_transponder_info")) {
    return CGEN_KR_TRANSPONDER_INFO;
  }
  if (!strcmp(string,"kr_transponder_path_io_info")) {
    return CGEN_KR_TRANSPONDER_PATH_IO_INFO;
  }
  if (!strcmp(string,"kr_transponder_path_io_info_sub")) {
    return CGEN_KR_TRANSPONDER_PATH_IO_INFO_SUB;
  }
  if (!strcmp(string,"kr_transponder_path_info")) {
    return CGEN_KR_TRANSPONDER_PATH_INFO;
  }
  if (!strcmp(string,"krad_slice_track_type_t")) {
    return CGEN_KRAD_SLICE_TRACK_TYPE_T;
  }
  if (!strcmp(string,"krad_subslice_St")) {
    return CGEN_KRAD_SUBSLICE_ST;
  }
  if (!strcmp(string,"krad_slicer_St")) {
    return CGEN_KRAD_SLICER_ST;
  }
  if (!strcmp(string,"krad_rebuilder_track_St")) {
    return CGEN_KRAD_REBUILDER_TRACK_ST;
  }
  if (!strcmp(string,"krad_rebuilder_St")) {
    return CGEN_KRAD_REBUILDER_ST;
  }
  if (!strcmp(string,"krad_udp_St")) {
    return CGEN_KRAD_UDP_ST;
  }
  if (!strcmp(string,"kr_v4l2_setup")) {
    return CGEN_KR_V4L2_SETUP;
  }
  if (!strcmp(string,"kr_v4l2_state")) {
    return CGEN_KR_V4L2_STATE;
  }
  if (!strcmp(string,"kr_v4l2_mode")) {
    return CGEN_KR_V4L2_MODE;
  }
  if (!strcmp(string,"kr_v4l2_info")) {
    return CGEN_KR_V4L2_INFO;
  }
  if (!strcmp(string,"kr_v4l2_open_info")) {
    return CGEN_KR_V4L2_OPEN_INFO;
  }
  if (!strcmp(string,"krad_vorbis_St")) {
    return CGEN_KRAD_VORBIS_ST;
  }
  if (!strcmp(string,"krad_vpx_encoder_St")) {
    return CGEN_KRAD_VPX_ENCODER_ST;
  }
  if (!strcmp(string,"krad_vpx_decoder_St")) {
    return CGEN_KRAD_VPX_DECODER_ST;
  }
  if (!strcmp(string,"kr_wayland_event_type")) {
    return CGEN_KR_WAYLAND_EVENT_TYPE;
  }
  if (!strcmp(string,"kr_wayland_pointer_event")) {
    return CGEN_KR_WAYLAND_POINTER_EVENT;
  }
  if (!strcmp(string,"kr_wayland_key_event")) {
    return CGEN_KR_WAYLAND_KEY_EVENT;
  }
  if (!strcmp(string,"kr_wayland_frame_event")) {
    return CGEN_KR_WAYLAND_FRAME_EVENT;
  }
  if (!strcmp(string,"kr_wayland_event")) {
    return CGEN_KR_WAYLAND_EVENT;
  }
  if (!strcmp(string,"kr_wayland_path_setup")) {
    return CGEN_KR_WAYLAND_PATH_SETUP;
  }
  if (!strcmp(string,"kr_wayland_setup")) {
    return CGEN_KR_WAYLAND_SETUP;
  }
  if (!strcmp(string,"kr_wayland_info")) {
    return CGEN_KR_WAYLAND_INFO;
  }
  if (!strcmp(string,"kr_wayland_path_info")) {
    return CGEN_KR_WAYLAND_PATH_INFO;
  }
  if (!strcmp(string,"krad_interweb_shutdown")) {
    return CGEN_KRAD_INTERWEB_SHUTDOWN;
  }
  if (!strcmp(string,"krad_interweb_server_St")) {
    return CGEN_KRAD_INTERWEB_SERVER_ST;
  }
  if (!strcmp(string,"interweb_client_type")) {
    return CGEN_INTERWEB_CLIENT_TYPE;
  }
  if (!strcmp(string,"interweb_verb")) {
    return CGEN_INTERWEB_VERB;
  }
  if (!strcmp(string,"interwebs_St")) {
    return CGEN_INTERWEBS_ST;
  }
  if (!strcmp(string,"krad_interweb_server_client_St")) {
    return CGEN_KRAD_INTERWEB_SERVER_CLIENT_ST;
  }
  if (!strcmp(string,"kr_x11")) {
    return CGEN_KR_X11;
  }
  if (!strcmp(string,"krad_xmms_playback_cmd_t")) {
    return CGEN_KRAD_XMMS_PLAYBACK_CMD_T;
  }
  if (!strcmp(string,"krad_xmms_St")) {
    return CGEN_KRAD_XMMS_ST;
  }
  if (!strcmp(string,"krad_y4m_St")) {
    return CGEN_KRAD_Y4M_ST;
  }
  return 0;
}

