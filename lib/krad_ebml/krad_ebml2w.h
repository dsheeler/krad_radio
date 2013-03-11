#include "krad_ebml2.h"

#ifndef KRAD_EBML2W_H
#define KRAD_EBML2W_H


inline int kr_ebml2_unpack_id (kr_ebml2_t *ebml, uint32_t *element, uint64_t *data_size);

inline int kr_ebml2_unpack_data (kr_ebml2_t *ebml, void *data, size_t len);
inline int kr_ebml2_unpack_string (kr_ebml2_t *ebml, char *string, size_t len);
inline int kr_ebml2_unpack_uint8 (kr_ebml2_t *ebml, uint8_t *integer, size_t len);
inline int kr_ebml2_unpack_uint16 (kr_ebml2_t *ebml, uint16_t *integer, size_t len);
inline int kr_ebml2_unpack_uint32 (kr_ebml2_t *ebml, uint32_t *integer, size_t len);
inline int kr_ebml2_unpack_uint64 (kr_ebml2_t *ebml, uint64_t *integer, size_t len);
inline int kr_ebml2_unpack_float (kr_ebml2_t *ebml, float *real, size_t len);
inline int kr_ebml2_unpack_double (kr_ebml2_t *ebml, double *real, size_t len);

inline int kr_ebml2_unpack_element_data (kr_ebml2_t *ebml, uint32_t *id, void *data, size_t maxlen);
inline int kr_ebml2_unpack_element_string (kr_ebml2_t *ebml, uint32_t *id, char *string, size_t maxlen);
inline int kr_ebml2_unpack_element_uint8 (kr_ebml2_t *ebml, uint32_t *id, uint8_t *integer);
inline int kr_ebml2_unpack_element_uint16 (kr_ebml2_t *ebml, uint32_t *id, uint16_t *integer);
inline int kr_ebml2_unpack_element_uint32 (kr_ebml2_t *ebml, uint32_t *id, uint32_t *integer);
inline int kr_ebml2_unpack_element_uint64 (kr_ebml2_t *ebml, uint32_t *id, uint64_t *integer);
inline int kr_ebml2_unpack_element_float (kr_ebml2_t *ebml, uint32_t *id, float *real);
inline int kr_ebml2_unpack_element_double (kr_ebml2_t *ebml, uint32_t *id, double *real);


int kr_ebml2_unpack_header (kr_ebml2_t *ebml, char *doctype, int doctype_maxlen,
                            uint32_t *version, uint32_t *read_version);


inline int kr_ebml2_at_end (kr_ebml2_t *ebml);

#endif
