#include "krad_muxponder.h"

typedef struct kr_muxponder_track_St kr_muxponder_track_t;
typedef struct kr_muxponder_output_St kr_muxponder_output_t;

struct kr_muxponder_track_St {
  uint64_t total_frames;
  kr_track_info_t info;
};

struct kr_muxponder_output_St {
  uint64_t position_or_something;
  kr_muxer_output_params_t params;
};

struct kr_muxponder_St {
  kr_muxponder_track_t *tracks;
  kr_muxponder_output_t *outputs;
  kr_transmitter_t *transmitter;  
  int track_count;
  int current_track;
};

int kr_muxponder_destroy (kr_muxponder_t **muxponder) {
  if ((muxponder != NULL) && (*muxponder != NULL)) {
    free ((*muxponder)->tracks);
    free ((*muxponder)->outputs);
    free (*muxponder);
    *muxponder = NULL;
    return 0;
  }
  return -1;
}

kr_muxponder_t *kr_muxponder_create (krad_transmitter_t *transmitter) {
  kr_muxponder_t *muxponder;
  muxponder = calloc (1, sizeof(kr_muxponder_t));
  muxponder->tracks = calloc (KR_MUXPONDER_MAX_TRACKS,
                              sizeof(kr_muxponder_track_t));  
  muxponder->outputs = calloc (KR_MUXPONDER_MAX_OUTPUTS,
                              sizeof(kr_muxponder_output_t));

  muxponder->transmitter = transmitter;

  return muxponder;
}

static int kr_muxponder_new_tracknumber (kr_muxponder_t *muxponder) {

  int tracknumber;

  if (muxponder->current_track == KR_MUXPONDER_MAX_TRACKS) {
    return -1;
  }

  tracknumber = muxponder->current_track;

  muxponder->track_count++;
  muxponder->current_track++;

  return tracknumber;
}

int kr_muxponder_create_track (kr_muxponder_t *muxponder,
                               kr_track_info_t *track_info) {

  int t;

  t = kr_muxponder_new_tracknumber (muxponder);
  if (t == -1) {
    return t;
  }
  
  memcpy (&muxponder->tracks[t].info, track_info, sizeof (kr_track_info_t));
  
  return t;
}

int kr_muxponder_create_output (kr_muxponder_t *muxponder,
                                kr_muxer_output_params_t *params) {

  return -1;
}

int kr_muxponder_add_data (kr_muxponder_t *muxponder,
                           int track, uint64_t timecode,
                           uint8_t *data, size_t size, int sync) {
  return -1;
}

