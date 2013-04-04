#include "krad_radio.h"
#include "krad_slice.h"

#ifndef KRAD_TRANSPONDER_GRAPH
#define KRAD_TRANSPONDER_GRAPH

#define KRAD_TRANSPONDER_SUBUNITS 99
#define KRAD_TRANSPONDER_PORT_CONNECTIONS 200

typedef struct krad_xpdr_St krad_xpdr_t;
typedef struct krad_xpdr_St kr_xpdr_t;
typedef struct kr_xpdr_subunit_St kr_xpdr_subunit_t;
typedef struct kr_xpdr_subunit_St xpdr_subunit_t;
typedef struct kr_xpdr_input_St kr_xpdr_input_t;
typedef struct kr_xpdr_output_St kr_xpdr_output_t;
typedef struct kr_xpdr_control_msg_St kr_xpdr_control_msg_t;
typedef struct kr_xpdr_watch_St kr_xpdr_watch_t;

typedef enum {
  CONNECTPORTS = 77999,
  DISCONNECTPORTS,
  UPDATE,
  DESTROY,
} kr_xpdr_control_msg_type_t;

typedef enum {
  DEMUXER = 1999,
  MUXER,
  DECODER,
  ENCODER,
  RAW,
} xpdr_subunit_type_t;

struct kr_xpdr_watch_St {
  int idle_callback_interval;
  int (*readable_callback)(void *);
  void (*destroy_callback)(void *);
  krad_codec_header_t *(*encoder_header_callback)(void *);  
  void *callback_pointer;
  int fd;
};

struct kr_xpdr_control_msg_St {
  kr_xpdr_control_msg_type_t type;
  kr_xpdr_input_t *input;
  kr_xpdr_output_t *output;
};

struct kr_xpdr_input_St {
  krad_ringbuffer_t *msg_ring;
  int socketpair[2];
  kr_xpdr_subunit_t *subunit;  
  kr_xpdr_subunit_t *connected_to_subunit;
};

struct kr_xpdr_output_St {
  kr_xpdr_input_t **connections;
  kr_slice_t *slice[4];
  int headers;
  krad_codec_header_t *header;  
};

struct kr_xpdr_subunit_St {
  int destroy;
  kr_xpdr_t *xpdr;
  kr_xpdr_watch_t *watch;
  pthread_t thread;
  xpdr_subunit_type_t type;
  kr_xpdr_input_t *control;  
  kr_xpdr_input_t **inputs;
  kr_xpdr_output_t **outputs;
  kr_slice_t *slice;
};

struct krad_xpdr_St {
  kr_xpdr_subunit_t **subunits;
};

void kr_xpdr_subunit_connect2 (kr_xpdr_subunit_t *kr_xpdr_subunit,
                               kr_xpdr_subunit_t *from_kr_xpdr_subunit);
void kr_xpdr_subunit_connect (kr_xpdr_subunit_t *kr_xpdr_subunit,
                              kr_xpdr_subunit_t *from_kr_xpdr_subunit);
void kr_xpdr_subunit_connect3 (kr_xpdr_subunit_t *kr_xpdr_subunit,
                               kr_xpdr_subunit_t *from_kr_xpdr_subunit);

krad_codec_header_t *kr_xpdr_get_audio_header (kr_xpdr_subunit_t *xpdr_subunit);
krad_codec_header_t *kr_xpdr_get_header (kr_xpdr_subunit_t *xpdr_subunit);
krad_codec_header_t *kr_xpdr_get_subunit_output_header (kr_xpdr_subunit_t *xpdr_subunit, int port);
int kr_xpdr_set_header (kr_xpdr_subunit_t *xpdr_subunit,
                        krad_codec_header_t *header);

kr_slice_t *kr_xpdr_get_slice (kr_xpdr_subunit_t *xpdr_subunit);
int kr_xpdr_slice_broadcast (kr_xpdr_subunit_t *xpdr_subunit,
                             kr_slice_t **slice);

int kr_xpdr_count (kr_xpdr_t *xpdr);
int kr_xpdr_get_info (kr_xpdr_t *xpdr, int num, char *string);
void *kr_xpdr_get_link (kr_xpdr_t *xpdr, int num);
kr_xpdr_subunit_t *kr_xpdr_get_subunit (kr_xpdr_t *xpdr, int sid);
int kr_xpdr_add_raw (kr_xpdr_t *xpdr, kr_xpdr_watch_t *watch);
int kr_xpdr_add_muxer (kr_xpdr_t *xpdr, kr_xpdr_watch_t *watch);
int kr_xpdr_add_demuxer (kr_xpdr_t *xpdr, kr_xpdr_watch_t *watch);
int kr_xpdr_add_encoder (kr_xpdr_t *xpdr, kr_xpdr_watch_t *watch);
int kr_xpdr_add_decoder (kr_xpdr_t *xpdr, kr_xpdr_watch_t *watch);
void kr_xpdr_subunit_remove (kr_xpdr_t *xpdr, int sid);

void krad_xpdr_destroy (kr_xpdr_t **xpdr);
krad_xpdr_t *krad_xpdr_create ();

#endif
