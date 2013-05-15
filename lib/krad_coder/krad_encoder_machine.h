#include "krad_encoder.h"
#include "krad_machine.h"

typedef struct kr_encoder_machine_St kr_encoder_machine_t;

typedef struct kr_encoder_machine_sector_St kr_encoder_machine_sector_t;
typedef struct kr_encoder_machine_sector_St kr_em_sector_t;

struct kr_encoder_machine_sector_St {
  kr_medium_t *medium;
  kr_codeme_t *codeme;
};

typedef int (*kr_encoder_machine_process_cb)(kr_encoder_machine_t *, kr_em_sector_t *, void *);

//void *pointer;

//int client_handler (kr_io2_t *in,
//                    kr_io2_t *out,
//                    void *)


int kr_encoder_machine_destroy (kr_encoder_machine_t **encoder_machine);
kr_encoder_machine_t *kr_encoder_machine_create (kr_encoder_machine_process_cb process_callback, void *user);

int kr_encoder_machine_encode (kr_encoder_machine_t *encoder_machine,
                               kr_em_sector_t *sector);