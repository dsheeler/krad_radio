#ifndef KRAD_TRANSPONDER_COMMON_H
#define KRAD_TRANSPONDER_COMMON_H

#include <string.h>
#include <opus_defines.h>
#include "krad_ebml2.h"
#include "krad_codec_header.h"
#include "krad_jack_common.h"

#define KR_AIN 0
#define KR_AOUT 1
#define KR_VIN 0
#define KR_VOUT 1

typedef struct krad_transponder_rep_St krad_transponder_rep_t;
typedef struct krad_transponder_rep_St kr_transponder_t;

typedef struct krad_transponder_subunit_rep_St krad_transponder_subunit_rep_t;
typedef struct krad_transponder_subunit_rep_St kr_transponder_subunit_t;

typedef struct kr_rawin_St kr_rawin_t;
typedef struct kr_rawout_St kr_rawout_t;
typedef struct kr_encoder_St kr_encoder_t;
//typedef struct kr_decoder_St kr_decoder_t;
typedef struct kr_muxer_St kr_muxer_t;
//typedef struct kr_demuxer_St kr_demuxer_t;

typedef struct kr_audio_encoder_St kr_audio_encoder_t;
typedef struct kr_video_encoder_St kr_video_encoder_t;
typedef struct kr_audio_decoder_St kr_audio_decoder_t;
typedef struct kr_video_decoder_St kr_video_decoder_t;

typedef struct kr_vpx_encoder_St kr_vpx_encoder_t;
typedef struct kr_theora_encoder_St kr_theora_encoder_t;
typedef struct kr_daala_encoder_St kr_daala_encoder_t;
typedef struct kr_vorbis_encoder_St kr_vorbis_encoder_t;
typedef struct kr_opus_encoder_St kr_opus_encoder_t;
typedef struct kr_flac_encoder_St kr_flac_encoder_t;

typedef struct kr_vpx_decoder_St kr_vpx_decoder_t;
typedef struct kr_theora_decoder_St kr_theora_decoder_t;
typedef struct kr_daala_decoder_St kr_daala_decoder_t;
typedef struct kr_vorbis_decoder_St kr_vorbis_decoder_t;
typedef struct kr_opus_decoder_St kr_opus_decoder_t;
typedef struct kr_flac_decoder_St kr_flac_decoder_t;

typedef struct kr_udp_muxer_St kr_udp_muxer_t;
typedef struct kr_ogg_muxer_St kr_ogg_muxer_t;
typedef struct kr_mkv_muxer_St kr_mkv_muxer_t;
typedef struct kr_transogg_muxer_St kr_transogg_muxer_t;

typedef struct kr_udp_demuxer_St kr_udp_demuxer_t;
typedef struct kr_ogg_demuxer_St kr_ogg_demuxer_t;
typedef struct kr_mkv_demuxer_St kr_mkv_demuxer_t;
typedef struct kr_transogg_demuxer_St kr_transogg_demuxer_t;

typedef struct kr_stat_St kr_stat_t;

struct kr_stat_St {
  uint64_t time_last;
  uint64_t time_total;
  uint64_t started_on;
};

typedef enum {
	AUDIO_ONLY = 150,
	VIDEO_ONLY,
	AUDIO_AND_VIDEO,
} krad_link_av_mode_t;

typedef enum {
  DEMUX,
  DECODE,
  MUX,
  ENCODE,
	RAWIN,
	RAWOUT,
	FAILURE,
} kr_txpdr_su_type_t;

typedef enum {
	TCP = 250,
	UDP,
	FILESYSTEM,
	FAIL,
} krad_link_transport_mode_t;

typedef enum {
	V4L2 = 500,
	DECKLINK,
	X11,
	NOVIDEO,
#ifdef KRAD_USE_FLYCAP
  FLYCAP
#endif
} krad_link_video_source_t;

typedef enum {
  KR_ADP_V4L2,
  KR_ADP_DECKLINK,
  KR_ADP_ALSA,
  KR_ADP_X11,
  KR_ADP_WAYLAND,
  KR_ADP_ENCODER,
  KR_ADP_FLYCAP,
  KR_ADP_KRAPI,
  KR_ADP_JACK
} kr_adapter_api;

typedef enum {
  KR_XPDR_INPUT = 1,
  KR_XPDR_OUTPUT
} kr_xpdr_dir;

struct krad_transponder_rep_St {
  uint16_t receiver_port;
  uint16_t transmitter_port;
};

struct kr_flac_encoder_St {
  int bit_depth;
};

struct kr_vpx_decoder_St {
  int yea;
};

struct kr_theora_decoder_St {
  int yea;
};

struct kr_daala_decoder_St {
  int lol;
};

struct kr_vorbis_decoder_St {
  int yea;
};

struct kr_opus_decoder_St {
  int yea;
};

struct kr_flac_decoder_St {
  int bit_depth;
};

struct kr_vpx_encoder_St {
  uint64_t deadline;
  uint32_t bitrate;
};

struct kr_theora_encoder_St {
  int kf_distance;
  int speed;
  int quality;
};

struct kr_daala_encoder_St {
  int lol;
};

struct kr_vorbis_encoder_St {
	float quality;
};

struct kr_opus_encoder_St {
	int bandwidth;
	int signal;
	int bitrate;
	int complexity;
	int frame_size;
};

typedef union {
	kr_vorbis_encoder_t vorbis;
	kr_opus_encoder_t opus;
	kr_flac_encoder_t flac;
} kr_audio_codec_t;

typedef union {
//	kr_vhs_encoder_t kvhs;
	kr_vpx_encoder_t vpx;
	kr_theora_encoder_t theora;
	kr_daala_encoder_t daala;
} kr_video_codec_t;

struct kr_audio_encoder_St {
  int sample_rate;
  int channels;
  uint64_t bytes;
  uint64_t frames;
  kr_audio_codec_t codec;
};

struct kr_video_decoder_St {
  int width;
  int height;
  int fps_numerator;
  int fps_denominator;
  int color_depth;
  kr_video_codec_t codec;
};

struct kr_audio_decoder_St {
  int sample_rate;
  int channels;
  kr_audio_codec_t codec;
};

struct kr_video_encoder_St {
  int width;
  int height;
  int fps_numerator;
  int fps_denominator;
  int color_depth;
  kr_video_codec_t codec;

  uint64_t bytes;
  uint64_t frames;

};

typedef union {
	kr_audio_encoder_t audio;
	kr_video_encoder_t video;
} kr_av_encoder_t;

struct kr_encoder_St {
	krad_codec_t codec;
	kr_av_encoder_t av;
};

typedef union {
	kr_audio_decoder_t audio;
	kr_video_decoder_t video;
} kr_av_decoder_t;
/*
struct kr_decoder_St {
	krad_codec_t codec;
	kr_av_decoder_t decoder;
};
*/
struct kr_udp_muxer_St {
  int ok;
};

struct kr_ogg_muxer_St {
  int ok;
};

struct kr_mkv_muxer_St {
  int ok;
};

struct kr_transogg_muxer_St {
  int ok;
};

struct kr_udp_demuxer_St {
  int ok;
};

struct kr_ogg_demuxer_St {
  int ok;
};

struct kr_mkv_demuxer_St {
  int ok;
};

struct kr_transogg_demuxer_St {
  int ok;
};

typedef union {
	kr_ogg_muxer_t ogg;
	kr_mkv_muxer_t mkv;
	kr_transogg_muxer_t togg;
	kr_udp_muxer_t udp;
} kr_muxer_actual_t;

struct kr_muxer_St {
	kr_container_type_t type;
	kr_muxer_actual_t container;
};

typedef union {
	kr_udp_demuxer_t udp;
	kr_ogg_demuxer_t ogg;
	kr_mkv_demuxer_t mkv;
	kr_transogg_demuxer_t togg;
} kr_demuxer_actual_t;

/*
struct kr_demuxer_St {
	kr_container_type_t type;
	kr_demuxer_actual_t container;
};
*/

struct kr_rawin_St {
  int yea;
};

struct kr_rawout_St {
  int yea;
};

typedef union {
  kr_encoder_t encoder;
//  kr_decoder_t decoder;
  kr_muxer_t muxer;
//  kr_demuxer_t demuxer;
  kr_rawout_t rawout;
  kr_rawin_t rawin;
} kr_transponder_subunit_actual_t;

struct krad_transponder_subunit_rep_St {
  kr_txpdr_su_type_t type;
  kr_stat_t stats;
  kr_transponder_subunit_actual_t actual;
};

typedef struct kr_transponder_path_setup kr_transponder_path_setup;
typedef struct kr_transponder_path_setup kr_xpdr_path_setup;
typedef struct kr_transponder_path_info kr_transponder_path_info;
typedef struct kr_transponder_path_info kr_xdpr_path_info;

typedef struct kr_transponder_info kr_transponder_info;
typedef struct kr_transponder_info kr_xpdr_info;

struct kr_transponder_info {
  uint32_t active_paths;
};

typedef enum {
  KR_XPDR_MIXER,
  KR_XPDR_COMPOSITOR,
  KR_XPDER_ADAPTER
} kr_transponder_path_io_type;

struct kr_adapter_info {
  kr_adapter_api api;
  union {
    kr_jack_info jack;
    /*
    kr_alsa_info alsa;
    kr_v4l2_info v4l2;
    kr_decklink_info decklink;
    */
  } info;
};

struct kr_adapter_path_info {
  kr_adapter_api api;
  union {
    kr_jack_path_info jack;
    /*
    kr_alsa_path_info alsa;
    kr_v4l2_path_info v4l2;
    kr_decklink_path_info decklink;
    */
  } info;
};

struct kr_transponder_path_io_info {
  kr_transponder_path_io_type type;
  union {
    kr_mixer_path_info mixer_path_info;
    kr_compositor_path_info compositor_path_info;
    kr_adapter_path_info adapter_path_info;
  } info;
};

struct kr_transponder_path_info {
  char name[128];
  kr_transponder_path_io_info input;
  kr_transponder_path_io_info output;
};

struct kr_transponder_path_setup {
  char name[128];
  kr_transponder_path_io_info input;
  kr_transponder_path_io_info output;
};

char *krad_opus_signal_to_nice_string (int signal);
char *krad_opus_bandwidth_to_nice_string (int bandwidth);
int krad_opus_string_to_bandwidth (char *string);
int krad_opus_string_to_signal (char *string);
char *krad_opus_bandwidth_to_string (int bandwidth);
char *krad_opus_signal_to_string (int signal);
char *kr_color_depth_to_string (int depth);
char *kr_container_type_to_string (kr_container_type_t type);
char *krad_link_transport_mode_to_string (krad_link_transport_mode_t transport_mode);
char *krad_link_video_source_to_string (krad_link_video_source_t video_source);
krad_link_transport_mode_t krad_link_string_to_transport_mode (char *string);
krad_link_video_source_t krad_link_string_to_video_source (char *string);
char *kr_txpdr_subunit_type_to_string (kr_txpdr_su_type_t type);
kr_txpdr_su_type_t kr_txpdr_string_to_subunit_type (char *string);
char *krad_link_av_mode_to_string (krad_link_av_mode_t av_mode);
krad_link_av_mode_t krad_link_string_to_av_mode (char *string);
char *krad_codec_to_string (krad_codec_t codec);
krad_codec_t krad_string_to_codec (char *string);
krad_codec_t krad_string_to_audio_codec (char *string);
krad_codec_t krad_string_to_video_codec (char *string);
krad_codec_t krad_string_to_codec_full (char *string, krad_link_av_mode_t av_mode);

int krad_codec_is_video(krad_codec_t codec);
int krad_codec_is_audio(krad_codec_t codec);

#endif
