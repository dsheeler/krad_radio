#include "krad_muxponder.h"

typedef struct kr_muxponder_track_St kr_muxponder_track_t;
typedef struct kr_muxponder_output_St kr_muxponder_output_t;

struct kr_muxponder_track_St {
  uint64_t total_frames;
  kr_track_info_t info;
};

typedef union {
  krad_transmission_t *transmission;
  krad_stream_t *stream;
  kr_file_t *file;
} kr_muxponder_output_actual;

struct kr_muxponder_output_St {
  uint32_t active;
  int wrote_hdr;
  kr_muxponder_output_actual transport;
  kr_muxer_output_params_t params;
};

struct kr_muxponder_St {
  kr_muxponder_track_t *tracks;
  kr_muxponder_output_t *outputs;
  kr_transmitter_t *transmitter;
  kr_mkv_t *mkv;
  int track_count;
  int current_track;
  int got_hdr;
  uint8_t *mkv_hdr;
  size_t mkv_hdr_size;
};

int kr_muxponder_destroy_output (kr_muxponder_t *muxponder, int num) {

  kr_muxponder_output_t *output;
  
  if (num >= KR_MUXPONDER_MAX_OUTPUTS) {
    return -2;
  }

  output = &muxponder->outputs[num];

  if (output->active == 0) {
    return -1;
  }

  switch (output->params.transport) {
    case LOCAL_FILE:
      kr_file_close (&output->transport.file);
      break;
    case STREAM:
      kr_stream_destroy (&output->transport.stream);
      break;
    case TRANSMISSION:
      kr_transmission_destroy (output->transport.transmission);
      break;
  }

  output->wrote_hdr = 0;
  output->active = 0;
  
  return 0;  
}

int kr_muxponder_destroy (kr_muxponder_t **muxponder) {
  int o;
  if ((muxponder != NULL) && (*muxponder != NULL)) {
    for (o = 0; o < KR_MUXPONDER_MAX_OUTPUTS; o++) {
      if ((*muxponder)->outputs[o].active == 1) {
        kr_muxponder_destroy_output ((*muxponder), o);
      }    
    }
    if ((*muxponder)->got_hdr == 1) {
      free ((*muxponder)->mkv_hdr);
    }
    kr_mkv_destroy (&(*muxponder)->mkv);
    free ((*muxponder)->tracks);
    free ((*muxponder)->outputs);
    free (*muxponder);
    *muxponder = NULL;
    return 0;
  }
  return -1;
}

int muxponder_data_cb (uint8_t *data, size_t size, uint32_t sync, void *ptr) {

  int o;
  kr_muxponder_t *muxponder;
  kr_muxponder_output_t *output;

  muxponder = (kr_muxponder_t *)ptr;

  //printf ("yay I got %zu bytes! sync is %d\n", size, sync);

  if ((muxponder->got_hdr == 0) && ((sync != 1) || (size < 1))) {
    return -2;
  }

  if (muxponder->got_hdr == 0) {
    muxponder->got_hdr = 1;
    muxponder->mkv_hdr_size = size;
    muxponder->mkv_hdr = malloc (muxponder->mkv_hdr_size);
    memcpy (muxponder->mkv_hdr, data, muxponder->mkv_hdr_size);
    size = 0;       
  }

  for (o = 0; o < KR_MUXPONDER_MAX_OUTPUTS; o++) {

    output = &muxponder->outputs[o];
    if (output->active != 1) {
      continue;
    }

    switch (output->params.transport) {
      case LOCAL_FILE:
        if (output->wrote_hdr == 0) {
          if (sync == 1) {
            write (output->transport.file->fd, muxponder->mkv_hdr, muxponder->mkv_hdr_size);
            output->wrote_hdr = 1;
          } else {
            continue;
          }
        }
        if (size > 0) {
          write (output->transport.file->fd, data, size);
        }
        break;
      case STREAM:
        if (output->wrote_hdr == 0) {
          if (sync == 1) {
            kr_stream_write (output->transport.stream, muxponder->mkv_hdr, muxponder->mkv_hdr_size);
            output->wrote_hdr = 1;
          } else {
            continue;
          }
        }
        if (size > 0) {
          //printf ("sending %zu to %d\n", size, output->transport.stream->sd);
          kr_stream_write (output->transport.stream, data, size);
        }
        break;
      case TRANSMISSION:
        if (output->wrote_hdr == 0) {
          if (sync == 1) {
            krad_transmitter_transmission_set_header (output->transport.transmission,
                                                      muxponder->mkv_hdr,
                                                      muxponder->mkv_hdr_size);
            output->wrote_hdr = 1;
          } else {
            continue;
          }
        }
        krad_transmitter_transmission_add_data_opt (output->transport.transmission,
                                                    data,
                                                    size,
                                                    sync);
        break;
    }
  }

  return 0;
}

kr_muxponder_t *kr_muxponder_create (krad_transmitter_t *transmitter) {
  kr_muxponder_t *muxponder;
  muxponder = calloc (1, sizeof(kr_muxponder_t));
  muxponder->tracks = calloc (KR_MUXPONDER_MAX_TRACKS,
                              sizeof(kr_muxponder_track_t));  
  muxponder->outputs = calloc (KR_MUXPONDER_MAX_OUTPUTS,
                              sizeof(kr_muxponder_output_t));

  muxponder->transmitter = transmitter;
  muxponder->current_track = 1;
  muxponder->mkv = kr_mkv_create_io_callback (muxponder_data_cb, muxponder);

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

  if (muxponder->got_hdr == 1) {
    return -2;
  }

  t = kr_muxponder_new_tracknumber (muxponder);
  if (t == -1) {
    return t;
  }
  
  memcpy (&muxponder->tracks[t].info, track_info, sizeof (kr_track_info_t));

  kr_mkv_add_video_track (muxponder->mkv, VP8,
                          30,
                          1,
                          muxponder->tracks[t].info.params.v.width,
                          muxponder->tracks[t].info.params.v.height);
  
  return t;
}

int kr_muxponder_create_output (kr_muxponder_t *muxponder,
                                kr_muxer_output_params_t *output_params) {

  int o;

  if (output_params->container != MKV) {
    failfast ("only mkv supported for this momento");
  }

  for (o = 0; o < KR_MUXPONDER_MAX_OUTPUTS; o++) {
    if (muxponder->outputs[o].active == 0) {
      break;
    }
  }
  
  if (o == KR_MUXPONDER_MAX_OUTPUTS) {
    return -1;
  }
  
  muxponder->outputs[o].active = 1;
  
  memcpy (&muxponder->outputs[o].params, output_params, sizeof (kr_muxer_output_params_t));

  switch (muxponder->outputs[o].params.transport) {
    case LOCAL_FILE:
      muxponder->outputs[o].transport.file = kr_file_create (muxponder->outputs[o].params.transport_params.file_output_params.filename);
      break;
    case STREAM:
      muxponder->outputs[o].transport.stream = kr_stream_create (muxponder->outputs[o].params.transport_params.stream_output_params.host,
                                                                 muxponder->outputs[o].params.transport_params.stream_output_params.port,
                                                                 muxponder->outputs[o].params.transport_params.stream_output_params.mount,
                                                                 muxponder->outputs[o].params.transport_params.stream_output_params.password);
      if (muxponder->outputs[o].transport.stream == NULL) {
        printf ("fail to create stream\n");
      }

      break;
    case TRANSMISSION:
      muxponder->outputs[o].transport.transmission = kr_transmission_create (muxponder->transmitter,
                                                                             muxponder->outputs[o].params.transport_params.transmission_output_params.mount,
                                                                             muxponder->outputs[o].params.transport_params.transmission_output_params.content_type);
      break;
  }

  return o;
}

int kr_muxponder_add_data (kr_muxponder_t *muxponder,
                           int track, uint64_t timecode,
                           uint8_t *data, size_t size, int sync) {

  kr_mkv_add_video (muxponder->mkv, track, data, size, sync);
  
  return 0;
}
