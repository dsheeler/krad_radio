#ifndef KRAD_MUXPONDER_H
#define KRAD_MUXPONDER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include "krad_player_common.h"
#include "krad_transmitter.h"
#include "krad_mkv.h"

#define KR_MUXPONDER_MAX_TRACKS 10
#define KR_MUXPONDER_MAX_OUTPUTS 10

typedef struct kr_muxponder_St kr_muxponder_t;
typedef struct kr_muxer_output_params_St kr_muxer_output_params_t;

typedef struct kr_muxer_file_output_params_St kr_muxer_file_output_params_t;
typedef struct kr_muxer_stream_output_params_St kr_muxer_stream_output_params_t;
typedef struct kr_muxer_transmission_output_params_St kr_muxer_transmission_output_params_t;

typedef enum {
	LOCAL_FILE,
	STREAM,
	TRANSMISSION
} kr_muxer_transport_type;

struct kr_muxer_file_output_params_St {
  char *filename;
  //int map;
};

struct kr_muxer_stream_output_params_St {
  char *host;
  uint32_t port;
  char *mount;
  char *password;
};

struct kr_muxer_transmission_output_params_St {
  char *mount;
  char *content_type;
};

typedef union {
  kr_muxer_file_output_params_t file_output_params;
  kr_muxer_stream_output_params_t stream_output_params;
  kr_muxer_transmission_output_params_t transmission_output_params;
} kr_muxer_transport_params;

struct kr_muxer_output_params_St {
  krad_container_type_t container;
  kr_muxer_transport_type transport;
  kr_muxer_transport_params transport_params;
};

kr_muxponder_t *kr_muxponder_create (krad_transmitter_t *transmitter);
int kr_muxponder_destroy (kr_muxponder_t **muxponder);

int kr_muxponder_create_track (kr_muxponder_t *muxponder,
                               kr_track_info_t *track);

int kr_muxponder_add_data (kr_muxponder_t *muxponder,
                           int track, uint64_t timecode,
                           uint8_t *data, size_t size, int sync);

int kr_muxponder_create_output (kr_muxponder_t *muxponder, kr_muxer_output_params_t *params);
int kr_muxponder_destroy_output (kr_muxponder_t *muxponder, int num);

#endif
