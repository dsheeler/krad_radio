#include "krad_sfx_common.h"
#include "krad_eq.h"
#include "krad_pass.h"
#include "krad_analog.h"
#include "krad_hardlimiter.h"
#include "krad_system.h"

#ifndef KRAD_SFX_H
#define KRAD_SFX_H

typedef struct kr_effect_St kr_effect_t;
typedef struct kr_effects_St kr_effects_t;
typedef struct kr_effects_St kr_effects;
typedef struct kr_effects_St kr_adsp;

struct kr_effect_St {
	kr_effect_type_t effect_type;
	void *effect[KRAD_EFFECTS_MAX_CHANNELS];
  int active;
};

struct kr_effects_St {
  float sample_rate;
  int channels;
	kr_effect_t *effect;
};

kr_effects *kr_effects_create(int channels, int sample_rate);
void kr_effects_destroy(kr_effects *effects);

void kr_effects_set_sample_rate(kr_effects *effects,
 uint32_t sample_rate);
void kr_effects_process(kr_effects *effects, float **input,
 float **output, int num_samples);

/* OpControls */
void kr_effects_effect_add2(kr_effects *effects, kr_effect_type_t effect,
 kr_mixer *mixer, char *portgroupname);
void kr_effects_effect_add(kr_effects *effects, kr_effect_type_t effect);
void kr_effects_effect_remove(kr_effects_t *kr_effects, int effect_num);

/* Controls */
void kr_effects_control(kr_effects *effects, int effect_num, int control_id,
 int control, float value, int duration, kr_easing easing, void *user);

/* Utils */
kr_effect_type_t kr_effects_strtoeffect(char *string);
int kr_effects_strtoctrl(kr_effect_type_t effect_type, char *string);

#endif
