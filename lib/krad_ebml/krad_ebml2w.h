#include "krad_ebml2.h"

#ifndef KRAD_EBML2W_H
#define KRAD_EBML2W_H

inline void kr_ebml2_unpack (kr_ebml2_t *ebml, void *buffer, size_t len);
int kr_ebml2_unpack_id (kr_ebml2_t *ebml, uint32_t *element, uint64_t *data_size);

//float kr_ebml2_unpack_float (unsigned char *ebml_frag, uint64_t ebml_data_size, int *pos);
//uint64_t kr_ebml2_unpack_number (unsigned char *ebml_frag, uint64_t ebml_data_size, int *pos);

int kr_ebml2_unpack_header (kr_ebml2_t *ebml, char *doctype, int doctype_maxlen, uint32_t *version, uint32_t *read_version);

#endif
