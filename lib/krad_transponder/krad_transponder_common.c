#include "krad_transponder_common.h"

char *krad_opus_bandwidth_to_string(int bandwidth) {

	switch (bandwidth) {
		case OPUS_AUTO:
			return "OPUS_AUTO";
		case OPUS_BANDWIDTH_NARROWBAND:
			return "OPUS_BANDWIDTH_NARROWBAND";
		case OPUS_BANDWIDTH_MEDIUMBAND:
			return "OPUS_BANDWIDTH_MEDIUMBAND";
		case OPUS_BANDWIDTH_WIDEBAND:
			return "OPUS_BANDWIDTH_WIDEBAND";
		case OPUS_BANDWIDTH_SUPERWIDEBAND:
			return "OPUS_BANDWIDTH_SUPERWIDEBAND";
		case OPUS_BANDWIDTH_FULLBAND:
			return "OPUS_BANDWIDTH_FULLBAND";
		default:
			return "OPUS_AUTO";
	}
}

char *krad_opus_bandwidth_to_nice_string(int bandwidth) {

	switch (bandwidth) {
		case OPUS_AUTO:
			return "Auto";
		case OPUS_BANDWIDTH_NARROWBAND:
			return "Narrowband";
		case OPUS_BANDWIDTH_MEDIUMBAND:
			return "Mediumband";
		case OPUS_BANDWIDTH_WIDEBAND:
			return "Wideband";
		case OPUS_BANDWIDTH_SUPERWIDEBAND:
			return "Super Wideband";
		case OPUS_BANDWIDTH_FULLBAND:
			return "Fullband";
		default:
			return "Unknown";
	}
}

char *krad_opus_signal_to_string(int signal) {

	switch (signal) {
		case OPUS_AUTO:
			return "OPUS_AUTO";
		case OPUS_SIGNAL_VOICE:
			return "OPUS_SIGNAL_VOICE";
		case OPUS_SIGNAL_MUSIC:
			return "OPUS_SIGNAL_MUSIC";
		default:
			return "OPUS_AUTO";
	}
}

char *krad_opus_signal_to_nice_string(int signal) {
	switch (signal) {
		case OPUS_AUTO:
			return "Auto";
		case OPUS_SIGNAL_VOICE:
			return "Voice";
		case OPUS_SIGNAL_MUSIC:
			return "Music";
		default:
			return "Unknown";
	}
}

int krad_opus_string_to_signal(char *string) {

	if ((strlen(string) == 17) && (strncmp(string, "OPUS_SIGNAL_VOICE", 17) == 0)) {
		return OPUS_SIGNAL_VOICE;
	}

	if ((strlen(string) == 17) && (strncmp(string, "OPUS_SIGNAL_MUSIC", 17) == 0)) {
		return OPUS_SIGNAL_MUSIC;
	}

	return OPUS_AUTO;
}

int krad_opus_string_to_bandwidth (char *string) {

	if ((strlen(string) == 25) && (strncmp(string, "OPUS_BANDWIDTH_NARROWBAND", 25) == 0)) {
		return OPUS_BANDWIDTH_NARROWBAND;
	}

	if ((strlen(string) == 25) && (strncmp(string, "OPUS_BANDWIDTH_MEDIUMBAND", 25) == 0)) {
		return OPUS_BANDWIDTH_MEDIUMBAND;
	}

	if ((strlen(string) == 23) && (strncmp(string, "OPUS_BANDWIDTH_WIDEBAND", 23) == 0)) {
		return OPUS_BANDWIDTH_WIDEBAND;
	}

	if ((strlen(string) == 28) && (strncmp(string, "OPUS_BANDWIDTH_SUPERWIDEBAND", 28) == 0)) {
		return OPUS_BANDWIDTH_SUPERWIDEBAND;
	}

	if ((strlen(string) == 23) && (strncmp(string, "OPUS_BANDWIDTH_FULLBAND", 23) == 0)) {
		return OPUS_BANDWIDTH_FULLBAND;
	}

	return OPUS_AUTO;
}

char *krad_codec_to_string (krad_codec_t codec) {

	switch (codec) {
		case VORBIS:
			return "Vorbis";
		case FLAC:
			return "FLAC";
		case OPUS:
			return "Opus";
		case VP8:
			return "VP8";
		case THEORA:
			return "Theora";
		case Y4M:
			return "Y4M";
		case MJPEG:
			return "Mjpeg";
		case PNG:
			return "PNG";
		case KVHS:
			return "KVHS";
		case DAALA:
			return "Daala";
		case H264:
			return "H264";
		default:
			return "No Codec";
	}
}

char *kr_color_depth_to_string (int depth) {

	switch (depth) {
		case 420:
			return "4:2:0";
		case 422:
			return "4:2:2";
		case 444:
			return "4:4:4";
		default:
			return "???";
	}
}

int krad_codec_is_audio (krad_codec_t codec) {

	switch (codec) {
		case VORBIS:
	    return 1;
		case FLAC:
	    return 1;
		case OPUS:
	    return 1;
    default:
	    return 0;
	}

	return 0;
}

int krad_codec_is_video (krad_codec_t codec) {

	switch (codec) {
		case VP8:
	    return 1;
		case THEORA:
	    return 1;
		case Y4M:
	    return 1;
		case MJPEG:
	    return 1;
		case KVHS:
	    return 1;
		case DAALA:
	    return 1;
		case H264:
	    return 1;
    default:
	    return 0;
	}

	return 0;
}

krad_codec_t krad_string_to_codec (char *string) {
	return krad_string_to_codec_full (string, AUDIO_AND_VIDEO);
}

krad_codec_t krad_string_to_audio_codec (char *string) {
	return  krad_string_to_codec_full (string, AUDIO_ONLY);
}

krad_codec_t krad_string_to_video_codec (char *string) {
	return krad_string_to_codec_full (string, VIDEO_ONLY);
}

krad_codec_t krad_string_to_codec_full (char *string, krad_link_av_mode_t av_mode) {

	if ((av_mode == AUDIO_ONLY) || (av_mode == AUDIO_AND_VIDEO)) {

		if ((strstr(string, "Vorbis") != NULL) || (strstr(string, "vorbis") != NULL)) {
			return VORBIS;
		}

		if ((strstr(string, "FLAC") != NULL) || (strstr(string, "flac") != NULL)) {
			return FLAC;
		}

		if ((strstr(string, "Opus") != NULL) || (strstr(string, "opus") != NULL)) {
			return OPUS;
		}

	}

	if ((av_mode == VIDEO_ONLY) || (av_mode == AUDIO_AND_VIDEO)) {

		if ((strstr(string, "VP8") != NULL) || (strstr(string, "vp8") != NULL)) {
			return VP8;
		}

		if ((strstr(string, "Theora") != NULL) || (strstr(string, "theora") != NULL)) {
			return THEORA;
		}

		if ((strstr(string, "Y4M") != NULL) || (strstr(string, "y4m") != NULL)) {
			return Y4M;
		}

		if ((strstr(string, "KVHS") != NULL) || (strstr(string, "vhs") != NULL)) {
			return KVHS;
		}

		if ((strstr(string, "MJPEG") != NULL) || (strstr(string, "Mjpeg") != NULL) || (strstr(string, "mjpeg") != NULL)) {
			return MJPEG;
		}

		if (strstr(string, "PNG") != NULL) {
			return PNG;
		}

		if (strstr(string, "Daala") != NULL) {
			return DAALA;
		}

		if ((strstr(string, "H264") != NULL) || (strstr(string, "h264") != NULL) || (strstr(string, "x264") != NULL)) {
			return H264;
		}
	}

	return NOCODEC;

}

krad_link_av_mode_t krad_link_string_to_av_mode (char *string) {

	if ((strcmp(string, "av") == 0) || (strcmp(string, "audiovideo") == 0) || (strcmp(string, "audio and video") == 0)
	 	|| (strcmp(string, "audio_and_video") == 0)) {
		return AUDIO_AND_VIDEO;
	}

	if ((strcmp(string, "video") == 0) || (strcmp(string, "videoonly") == 0) || (strcmp(string, "video only") == 0)
	 	|| (strcmp(string, "video_only") == 0)) {
		return VIDEO_ONLY;
	}

	return AUDIO_ONLY;

}

char *krad_link_av_mode_to_string (krad_link_av_mode_t av_mode) {

	switch (av_mode) {
		case AUDIO_ONLY:
			return "audio only";
		case VIDEO_ONLY:
			return "video only";
		case AUDIO_AND_VIDEO:
			return "audio and video";
		default:
			return "Unknown";
	}
}

char *kr_txpdr_subunit_type_to_string (kr_txpdr_su_type_t type) {

	switch (type) {
		case RAWIN:
			return "RAW In";
		case RAWOUT:
			return "RAW Out";
		case DEMUX:
			return "Demuxer";
		case MUX:
			return "Muxer";
		case ENCODE:
			return "Encoder";
		case FAILURE:
			return "Failure";
		default:
			return "Unknown";
	}
}

kr_txpdr_su_type_t kr_txpdr_string_to_subunit_type (char *string) {

	if (((strlen(string) == 6) && (strncmp(string, "RAW In", 6) == 0)) ||
	     ((strlen(string) == 5) && (strncmp(string, "rawin", 5) == 0))) {
		return RAWIN;
	}

	if (((strlen(string) == 5) && (strncmp(string, "demux", 5) == 0)) ||
      ((strlen(string) == 7) && (strncmp(string, "demuxer", 7) == 0))) {
		return DEMUX;
	}

	if (((strlen(string) == 7) && (strncmp(string, "RAW Out", 7) == 0)) ||
	     ((strlen(string) == 6) && (strncmp(string, "rawout", 6) == 0))) {
		return RAWOUT;
	}

	if (((strlen(string) == 3) && (strncmp(string, "mux", 3) == 0)) ||
      ((strlen(string) == 5) && (strncmp(string, "muxer", 5) == 0))) {
		return MUX;
	}

	if (((strlen(string) == 3) && (strncmp(string, "enc", 3) == 0)) ||
      ((strlen(string) == 6) && (strncmp(string, "encode", 6) == 0)) ||
      ((strlen(string) == 7) && (strncmp(string, "encoder", 7) == 0))) {
		return ENCODE;
	}

	if (((strlen(string) == 3) && (strncmp(string, "dec", 3) == 0)) ||
      ((strlen(string) == 6) && (strncmp(string, "decode", 6) == 0)) ||
      ((strlen(string) == 7) && (strncmp(string, "decoder", 7) == 0))) {
		return DECODE;
	}

	return FAILURE;
}

krad_link_video_source_t krad_link_string_to_video_source (char *string) {

	if ((strcmp(string, "x11") == 0) || (strcmp(string, "X11") == 0)) {
		return X11;
	}

	if (strcmp(string, "decklink") == 0) {
		return DECKLINK;
	}
#ifdef KRAD_USE_FLYCAP
	if ((strcmp(string, "fc") == 0) || (strcmp(string, "fc2") == 0) || (strcmp(string, "flycap") == 0)) {
		return FLYCAP;
	}
#endif
	if ((strcmp(string, "V4L2") == 0) || (strcmp(string, "v4l2") == 0) || (strcmp(string, "v4l") == 0)) {
		return V4L2;
	}

	return NOVIDEO;

}

char *krad_link_video_source_to_string (krad_link_video_source_t video_source) {

	switch (video_source) {
		case V4L2:
			return "V4L2";
		case DECKLINK:
			return "decklink";
		case X11:
			return "X11";
#ifdef KRAD_USE_FLYCAP
		case FLYCAP:
			return "FLYCAP";
#endif
		case NOVIDEO:
			return "novideo";
		default:
			return "Unknown";
	}
}

char *kr_container_type_to_string (kr_container_type_t type) {
	switch (type) {
		case MKV:
			return "MKV";
		case KUDP:
			return "KUDP";
		case OGG:
			return "Ogg";
		case TOGG:
			return "transOgg";
		case NATIVEFLAC:
			return "Native FLAC";
		case Y4MFILE:
			return "Y4M";
		default:
			return "Unknown";
	}
}

krad_link_transport_mode_t krad_link_string_to_transport_mode (char *string) {

	if ((strcmp(string, "UDP") == 0) || (strcmp(string, "udp") == 0)) {
		return UDP;
	}

	if ((strcmp(string, "TCP") == 0) || (strcmp(string, "tcp") == 0)) {
		return TCP;
	}

	if ((strcmp(string, "filesystem") == 0) || (strcmp(string, "FILESYSTEM") == 0)) {
		return FILESYSTEM;
	}

	return FAIL;

}

char *krad_link_transport_mode_to_string(krad_link_transport_mode_t transport_mode) {
	switch (transport_mode) {
		case TCP:
			return "TCP";
		case UDP:
			return "UDP";
		case FILESYSTEM:
			return "filesystem";
		case FAIL:
			return "failure";
		default:
			return "Unknown";
	}
}
