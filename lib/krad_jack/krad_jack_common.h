#ifndef KR_JACK_COMMON_H
#define KR_JACK_COMMON_H

#include "inttypes.h"

typedef enum {
  KR_JACK_INPUT = 1,
  KR_JACK_OUTPUT
} kr_jack_direction;

typedef enum {
  KR_JACK_OFFLINE,
  KR_JACK_ONLINE
} kr_jack_state;

typedef struct kr_jack_path_info kr_jack_input_info;
typedef struct kr_jack_path_info kr_jack_output_info;

typedef struct {
  char client_name[64];
  char server_name[64];
} kr_jack_setup_info;

typedef struct {
  char client_name[64];
  char server_name[64];
  kr_jack_state state;
  uint32_t inputs;
  uint32_t outputs;
  uint32_t sample_rate;
  uint32_t period_size;
  uint32_t xruns;
  uint64_t frames;
} kr_jack_info;

typedef struct {
  char name[64];
  int channels;
  kr_jack_direction direction;
  /* Connections? */
} kr_jack_path_info;

#include "gen/krad_jack_to_ebml.h"
#include "gen/krad_jack_from_ebml.h"
#include "gen/krad_jack_to_text.h"
#include "gen/krad_jack_helpers.h"

#endif
