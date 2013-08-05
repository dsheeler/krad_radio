#ifndef KRAD_TRANSPONDER_H
#define KRAD_TRANSPONDER_H

typedef struct krad_link_St krad_link_t;
typedef struct kr_transponder krad_transponder_t;
typedef struct kr_transponder kr_transponder;
typedef struct kr_transponder_setup kr_transponder_setup;

typedef struct kr_transponder_info_cb_arg kr_transponder_info_cb_arg;

typedef void (kr_transponder_info_cb)(kr_transponder_info_cb_arg *);

#include "krad_radio.h"
#include "krad_slice.h"
#include "krad_transponder_interface.h"

#include "krad_player_common.h"

#ifdef KRAD_USE_FLYCAP
#include "krad_fc2.h"
#endif

#define DEFAULT_VPX_BITRATE 1000
#define DEFAULT_THEORA_QUALITY 31
#define DEFAULT_CAPTURE_BUFFER_FRAMES 50
#define DEFAULT_DECODING_BUFFER_FRAMES 50
#define DEFAULT_VORBIS_QUALITY 0.4
#define KRAD_DEFAULT_FLAC_BIT_DEPTH 16
#define DEFAULT_COMPOSITOR_WIDTH 960
#define DEFAULT_COMPOSITOR_HEIGHT 540
#define DEFAULT_COMPOSITOR_FPS 30
#define DEFAULT_COMPOSITOR_FPS_NUMERATOR DEFAULT_COMPOSITOR_FPS * 1000
#define DEFAULT_COMPOSITOR_FPS_DENOMINATOR 1 * 1000
#define DEFAULT_FPS_NUMERATOR DEFAULT_FPS * 1000
#define DEFAULT_FPS_DENOMINATOR 1 * 1000
#define KRAD_LINK_DEFAULT_TCP_PORT 80
#define KRAD_LINK_DEFAULT_UDP_PORT 42666
#define KRAD_LINK_DEFAULT_VIDEO_CODEC VP8
#define KRAD_LINK_DEFAULT_AUDIO_CODEC VORBIS
#define KRAD_TRANSPONDER_MAX_SUBUNITS 32

struct kr_transponder_setup {
  kr_mixer *mixer;
  kr_compositor *compositor;
  void *user;
  kr_transponder_info_cb *cb;
};

struct kr_transponder {
  kr_address_t address;
	krad_link_t *krad_link[KRAD_TRANSPONDER_MAX_SUBUNITS];
	kr_radio *krad_radio;
	krad_transmitter_t *krad_transmitter;
	kr_xpdr_t *xpdr;
};

struct krad_link_St {

	kr_radio *krad_radio;
	krad_transponder_t *krad_transponder;

	int link_num;
	char sysname[64];
	krad_tags_t *krad_tags;

	krad_link_av_mode_t av_mode;

	krad_framepool_t *krad_framepool;
	kr_mixer_path *krad_mixer_portgroup;
	krad_compositor_port_t *krad_compositor_port;

	krad_compositor_port_t *krad_compositor_port2;

	krad_decklink_t *krad_decklink;
	krad_x11_t *krad_x11;
	krad_vpx_encoder_t *krad_vpx_encoder;
	krad_vpx_decoder_t *krad_vpx_decoder;
	krad_theora_encoder_t *krad_theora_encoder;
	krad_theora_decoder_t *krad_theora_decoder;

#ifdef KRAD_USE_FLYCAP
  kr_fc2_t *fc;
#endif

	krad_ticker_t *krad_ticker;

	krad_vhs_t *krad_vhs;
	krad_y4m_t *krad_y4m;

	krad_vorbis_t *krad_vorbis;
	krad_flac_t *krad_flac;
	krad_opus_t *krad_opus;
//	krad_ogg_t *krad_ogg;
//	krad_ebml_t *krad_ebml;
	krad_container_t *krad_container;
	krad_v4l2_t *krad_v4l2;

	kr_txpdr_su_type_t type;
	krad_link_video_source_t video_source;
	krad_codec_t audio_codec;
	krad_codec_t video_codec;

	krad_codec_t last_audio_codec;
	krad_codec_t last_video_codec;

	krad_codec_t codec;

	krad_link_transport_mode_t transport_mode;

	int krad_compositor_port_fd;

	int color_depth;

	char audio_input[64];
	char device[64];
	char output[512];
	char input[512];
	char host[256];
	int port;
	char mount[256];
	char content_type[64];
	char password[64];

	int sd;

	int vp8_bitrate;
	int theora_quality;
	int flac_bit_depth;
	int opus_bitrate;
	float vorbis_quality;

	int capture_width;
	int capture_height;
	int capture_fps;
	int composite_width;
	int composite_height;
	int encoding_width;
	int encoding_height;

	int encoding_fps_numerator;
	int encoding_fps_denominator;

	int fps_numerator;
	int fps_denominator;

	int mjpeg_mode;
	int video_passthru;

	int capture_buffer_frames;
	int decoding_buffer_frames;

	krad_ringbuffer_t *audio_capture_ringbuffer[KR_MXR_MAX_CHANNELS];
	krad_ringbuffer_t *audio_input_ringbuffer[KR_MXR_MAX_CHANNELS];
	krad_ringbuffer_t *audio_output_ringbuffer[KR_MXR_MAX_CHANNELS];
	float *samples[KR_MXR_MAX_CHANNELS];

	int audio_encoder_ready;
	int audio_frames_captured;

	int video_track;
	int audio_track;

	int channels;

	kr_xpdr_subunit_t *track_sources[10];

	kr_xpdr_subunit_t *subunit;

	kr_mixer_path *mixer_portgroup;
	int au_framecnt;
	float *au_samples[KR_MXR_MAX_CHANNELS];
	float *au_interleaved_samples;
  int socketpair[2];

	unsigned char *au_header[3];
	int au_header_len[3];
	unsigned char *au_buffer;
	unsigned char *vu_header[3];
	int vu_header_len[3];
	float *au_audio;

	krad_resample_ring_t *krad_resample_ring[KR_MXR_MAX_CHANNELS];

  int graph_id;

  unsigned char *demux_buffer;
	unsigned char *demux_header_buffer;
	int demux_video_packets;
	int demux_audio_packets;
	int demux_current_track;

	krad_codec_t demux_track_codecs[10];

  krad_transmission_t *muxer_krad_transmission;
  unsigned char *muxer_packet;
  uint64_t muxer_video_frames_muxed;
  uint64_t muxer_audio_frames_muxed;
  float muxer_audio_frames_per_video_frame;
  int muxer_seen_passthu_keyframe;
  int muxer_initial_passthu_frames_skipped;

#ifdef KRAD_USE_WAYLAND
  kr_wayland *wayland;
  kr_wayland_window *window;
#endif
};

void demuxer_unit_create(void *arg);
int demuxer_unit_process(void *arg);
void demuxer_unit_destroy(void *arg);

krad_link_t *krad_transponder_get_link_from_sysname(krad_transponder_t *tpdr, char *sysname);
krad_tags_t *krad_transponder_get_tags_for_link (krad_transponder_t *tpdr, char *sysname);
krad_tags_t *krad_link_get_tags(krad_link_t *krad_link);

void krad_link_destroy(krad_link_t *krad_link);
krad_link_t *krad_link_prepare(int linknum);
void krad_link_start(krad_link_t *krad_link);

kr_transponder *kr_transponder_create(kr_transponder_setup *setup);
int kr_transponder_destroy(kr_transponder *transponder);
void krad_link_audio_samples_callback(int frames, void *userdata, float **samples);

#endif
