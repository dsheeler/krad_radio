#ifndef KRAD_CODEC_HEADER_H
#define KRAD_CODEC_HEADER_H

#ifndef KRAD_CODEC_T
typedef enum {
  NOCODEC = 6666,
  OPUS,
  FLAC,
  VP8,
  THEORA,
  MJPEG,
  PNG,
  CODEC2,
  SKELETON,
  Y4M,
  DAALA,
  KVHS,
  H264,
  VORBIS,
} krad_codec_t;

typedef krad_codec_t kr_codec_t;

#define KRAD_CODEC_T 1
#endif

typedef enum {
  MKV = 100,
  OGG,
  TOGG,
  NATIVEFLAC,
  Y4MFILE,
  KUDP,
} krad_container_type_t;

typedef krad_container_type_t kr_container_type_t;
typedef struct krad_codec_header_St krad_codec_header_t;
typedef struct krad_codec_header_St kr_codec_hdr_t;

struct krad_codec_header_St {
  krad_codec_t codec;
  uint8_t *lacing;
  size_t lacing_len;
  uint8_t *data[4];
  size_t sz[4];
  uint32_t count;
};

#include "gen/krad_container_to_ebml.h"
#include "gen/krad_container_from_ebml.h"
#include "gen/krad_container_to_text.h"
#include "gen/krad_container_helpers.h"

#endif
