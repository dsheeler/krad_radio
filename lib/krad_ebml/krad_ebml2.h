#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct kr_ebml2_St kr_ebml2_t;

struct kr_ebml2_St {
  unsigned char *buf;
  unsigned char buffer[1024];
  size_t pos;
};


void kr_print_ebml (unsigned char *buffer, int len);
inline void kr_ebml2_advance (kr_ebml2_t *ebml, size_t bytes);
int kr_ebml2_destroy (kr_ebml2_t **ebml);
kr_ebml2_t *kr_ebml2_create ();
