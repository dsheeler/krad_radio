#ifndef KR_JACK_COMMON_H
#define KR_JACK_COMMON_H

#include "inttypes.h"

typedef enum {
  KR_JACK_INPUT = 1,
  KR_JACK_OUTPUT
} kr_jack_direction;

typedef struct kr_jack_path_info kr_jack_path_info;
typedef struct kr_jack_path_info kr_jack_input_info;
typedef struct kr_jack_path_info kr_jack_output_info;
typedef struct kr_jack_info kr_jack_info;

struct kr_jack_info {
  char client_name[64];
  char server_name[64];
  int inputs;
  int outputs;
  uint32_t xruns;
  int active;
  uint32_t sample_rate;
  uint32_t period_size;
  uint64_t frames;
};

struct kr_jack_path_info {
  char name[64];
  int channels;
  kr_jack_direction direction;
  /* Connections? */
};

#endif
