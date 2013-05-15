#include "krad_encoder.h"
#include "krad_machine.h"

typedef struct kr_encoder_machine_St kr_encoder_machine_t;

int kr_encoder_machine_destroy (kr_encoder_machine_t **encoder_machine);
kr_encoder_machine_t *kr_encoder_machine_create ();

int kr_encoder_machine_encode (kr_encoder_machine_t *encoder_machine);