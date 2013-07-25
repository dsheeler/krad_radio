#include "krad_sfx_common.h"
#include "krad_eq.h"
#include "krad_pass.h"
#include "krad_analog.h"
#include "krad_hardlimiter.h"
#include "krad_system.h"

#ifndef KRAD_SFX_H
#define KRAD_SFX_H

typedef struct kr_sfx kr_sfx;
typedef struct kr_sfx_effect kr_sfx_effect;

struct kr_sfx_effect {
  kr_sfx_type type;
  void *effect[KR_SFX_MAX_CHANNELS];
  int active;
};

struct kr_sfx {
  float sample_rate;
  int channels;
  kr_sfx_effect *effect;
};

kr_sfx *kr_sfx_create(int channels, int sample_rate);
void kr_sfx_destroy(kr_sfx *sfx);

void kr_sfx_set_sample_rate(kr_sfx *sfx, uint32_t sample_rate);
void kr_sfx_process(kr_sfx *sfx, float **input, float **output, int nframes);

/* OpControls */
void kr_sfx_effect_add2(kr_sfx *sfx, kr_effect_type_t effect, kr_mixer *mixer,
 char *pathname);
void kr_sfx_effect_add(kr_effects *sfx, kr_sfx_type effect);
void kr_sfx_effect_remove(kr_sfx *sfx, int effect_num);

/* Controls */
void kr_sfx_control(kr_sfx *sfx, int effect_num, int control_id, int control,
 float value, int duration, kr_easing easing, void *user);

/* Utils */
kr_sfx_type kr_sfx_strtosfx(char *string);
int kr_sfx_strtoctl(kr_sfx_type type, char *string);

#endif
