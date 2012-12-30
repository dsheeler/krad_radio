#ifndef KRAD_TRANSPONDER_H
#define KRAD_TRANSPONDER_H

typedef struct krad_link_St krad_link_t;
typedef struct krad_transponder_St krad_transponder_t;
typedef struct krad_transponder_listen_client_St krad_transponder_listen_client_t;

#include "krad_radio.h"
#include "krad_transponder_interface.h"

#define DEFAULT_VPX_BITRATE 1200
#define DEFAULT_THEORA_QUALITY 31
#define DEFAULT_CAPTURE_BUFFER_FRAMES 50
#define DEFAULT_DECODING_BUFFER_FRAMES 50
#define DEFAULT_VORBIS_QUALITY 0.5
#define KRAD_DEFAULT_FLAC_BIT_DEPTH 16
#define DEFAULT_COMPOSITOR_WIDTH 1280
#define DEFAULT_COMPOSITOR_HEIGHT 720
#define DEFAULT_COMPOSITOR_FPS 30
#define DEFAULT_COMPOSITOR_FPS_NUMERATOR DEFAULT_COMPOSITOR_FPS * 1000
#define DEFAULT_COMPOSITOR_FPS_DENOMINATOR 1 * 1000
#define DEFAULT_FPS_NUMERATOR DEFAULT_FPS * 1000
#define DEFAULT_FPS_DENOMINATOR 1 * 1000
#define KRAD_LINK_DEFAULT_TCP_PORT 80
#define KRAD_LINK_DEFAULT_UDP_PORT 42666
#define KRAD_LINK_DEFAULT_VIDEO_CODEC VP8
#define KRAD_LINK_DEFAULT_AUDIO_CODEC VORBIS
#define KRAD_TRANSPONDER_MAX_LINKS 42
#define HELP -1337

struct krad_transponder_St {
	krad_link_t *krad_link[KRAD_TRANSPONDER_MAX_LINKS];
	krad_radio_t *krad_radio;

	pthread_mutex_t change_lock;

	/* transponder listener */	
	unsigned char *buffer;

	int port;
	int sd;
	struct sockaddr_in local_address;
	int listening;
	int stop_listening;
	pthread_t listening_thread;
	
	/* transponder transmitter */	
	krad_transmitter_t *krad_transmitter;	
	
	
	krad_Xtransponder_t *krad_Xtransponder;
	
};

struct krad_transponder_listen_client_St {

	krad_transponder_t *krad_transponder;
	pthread_t client_thread;

	char in_buffer[1024];
	char out_buffer[1024];
	char mount[256];
	char content_type[256];
	int got_mount;
	int got_content_type;

	int in_buffer_pos;
	int out_buffer_pos;
	
	int sd;
	int ret;
	int wrote;

};

struct krad_link_St {

	krad_radio_t *krad_radio;

	krad_transponder_t *krad_transponder;

	int link_num;

	char sysname[64];
	krad_tags_t *krad_tags;

	krad_link_av_mode_t av_mode;

	krad_framepool_t *krad_framepool;

	krad_mixer_portgroup_t *krad_mixer_portgroup;
	krad_compositor_port_t *krad_compositor_port;

	krad_decklink_t *krad_decklink;
	krad_x11_t *krad_x11;
	krad_vpx_encoder_t *krad_vpx_encoder;
	krad_vpx_decoder_t *krad_vpx_decoder;
	krad_theora_encoder_t *krad_theora_encoder;
	krad_theora_decoder_t *krad_theora_decoder;
	
	krad_ticker_t *krad_ticker;
	
	krad_vhs_t *krad_vhs;
	krad_y4m_t *krad_y4m;	

	krad_vorbis_t *krad_vorbis;
	krad_flac_t *krad_flac;
	krad_opus_t *krad_opus;
//	krad_ogg_t *krad_ogg;
	krad_ebml_t *krad_ebml;
	krad_container_t *krad_container;
	krad_v4l2_t *krad_v4l2;
	
	krad_link_operation_mode_t operation_mode;
	krad_link_video_source_t video_source;
	krad_codec_t audio_codec;
	krad_codec_t video_codec;
	
	krad_codec_t last_audio_codec;
	krad_codec_t last_video_codec;
	
	krad_link_transport_mode_t transport_mode;
	
	int krad_compositor_port_fd;

	int color_depth;
	
	char audio_input[128];	
	
	char device[512];
	char alsa_capture_device[512];
	char alsa_playback_device[512];
	char jack_ports[512];
	
	char output[512];
	char input[512];
	char host[512];
	int port;
	char mount[512];
	char content_type[512];	
	char password[512];
	
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
	
	int playing;
	
	int encoding;
	int capturing;
	
	krad_ringbuffer_t *audio_capture_ringbuffer[KRAD_MIXER_MAX_CHANNELS];	
	krad_ringbuffer_t *audio_input_ringbuffer[KRAD_MIXER_MAX_CHANNELS];
	krad_ringbuffer_t *audio_output_ringbuffer[KRAD_MIXER_MAX_CHANNELS];
	float *samples[KRAD_MIXER_MAX_CHANNELS];

	int audio_encoder_ready;
	int audio_frames_captured;

	krad_ringbuffer_t *decoded_audio_ringbuffer;
	krad_ringbuffer_t *encoded_audio_ringbuffer;
	krad_ringbuffer_t *encoded_video_ringbuffer;
	
	int video_track;
	int audio_track;

	int channels;

	pthread_t main_thread;
	pthread_t video_capture_thread;
	pthread_t video_encoding_thread;
	pthread_t audio_encoding_thread;
	pthread_t video_decoding_thread;
	pthread_t audio_decoding_thread;
	pthread_t stream_output_thread;
	pthread_t stream_input_thread;
	pthread_t udp_output_thread;
	pthread_t udp_input_thread;

	int destroy;
	int verbose;
	
	krad_rebuilder_t *krad_rebuilder;
	krad_slicer_t *krad_slicer;

	krad_mixer_portgroup_t *mixer_portgroup;
	int au_framecnt;
	float *au_samples[KRAD_MIXER_MAX_CHANNELS];
	float *au_interleaved_samples;
	unsigned char *au_buffer;
  int socketpair[2];

	unsigned char *au_header[3];
	int au_header_len[3];
	
	unsigned char *vu_header[3];
	int vu_header_len[3];
	float *au_audio;

	krad_resample_ring_t *krad_resample_ring[KRAD_MIXER_MAX_CHANNELS];


	unsigned char *vu_buffer;

  int aud_graph_id;
  int vud_graph_id;

  int au_graph_id;
  int vu_graph_id;
  int cap_graph_id;
  int demux_graph_id;
  int mux_graph_id;  
};

int krad_link_wait_codec_init (krad_link_t *krad_link);

void krad_transponder_link_to_ebml ( krad_ipc_server_client_t *client, krad_link_t *krad_link);

void krad_transponder_stop_listening (krad_transponder_t *krad_linker);
int krad_transponder_listen (krad_transponder_t *krad_transponder, int port);

krad_link_t *krad_transponder_get_link_from_sysname (krad_transponder_t *krad_transponder, char *sysname);
krad_tags_t *krad_transponder_get_tags_for_link (krad_transponder_t *krad_transponder, char *sysname);
krad_tags_t *krad_link_get_tags (krad_link_t *krad_link);

krad_transponder_t *krad_transponder_create ();
void krad_transponder_destroy (krad_transponder_t *krad_transponder);
int krad_transponder_handler ( krad_transponder_t *krad_transponder, krad_ipc_server_t *krad_ipc );
void krad_link_shutdown();
void krad_link_audio_samples_callback (int frames, void *userdata, float **samples);
void krad_link_destroy (krad_link_t *krad_link);
krad_link_t *krad_link_create (int linknum);
void krad_link_run (krad_link_t *krad_link);

#endif
