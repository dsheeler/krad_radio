#include "krad_ogg2.h"

typedef struct kr_ogg_page_params_St kr_ogg_page_params_t;

struct kr_ogg_track_St {
  int64_t granule_position;
  uint32_t bitstream_serial;
  uint32_t page_sequence_number;
  krad_codec_header_t *hdr;
};

struct kr_ogg_page_params_St {
  int64_t granule_position;
  uint32_t bitstream_serial;
  uint32_t page_sequence_number;
  uint8_t header_type;
  uint8_t *data;
  size_t size;
};

static int kr_ogg_generate_page (kr_ogg_page_params_t *params, uint8_t *page);

#include "krad_ogg2_io.c"

int kr_ogg_destroy (kr_ogg_t **ogg) {

  if ((ogg == NULL) || (*ogg == NULL)) {
    return -1;
  }
  if ((*ogg)->hdr_sz > 0) {
    free ((*ogg)->hdr);
  }
  free ((*ogg)->tracks);
  free (*ogg);
  *ogg = NULL;
  return 0;
}

kr_ogg_t *kr_ogg_create () {

  kr_ogg_t *ogg;

  ogg = calloc (1, sizeof(kr_ogg_t));
  ogg->tracks = calloc (KRAD_OGG_MAX_TRACKS, sizeof(kr_ogg_track_t));

  return ogg;
}

int kr_ogg_add_track (kr_ogg_t *ogg, krad_codec_header_t *hdr) {

  int t;
  
  if (ogg->hdr_sz > 0) {
    return -2;
  }
  
  for (t = 0; t < KRAD_OGG_MAX_TRACKS; t++) {
    if (ogg->tracks[t].bitstream_serial == 0) {
      ogg->tracks[t].bitstream_serial = rand () + time(NULL);
      ogg->tracks[t].hdr = hdr;
      return t;
    } 
  }
  return -1;
}

int kr_ogg_generate_header (kr_ogg_t *ogg) {

  int t;
  int h;
  size_t sz; 
  
  sz = 0;
  
  for (t = 0; t < KRAD_OGG_MAX_TRACKS; t++) {
    if (ogg->tracks[t].bitstream_serial != 0) {
      for (h = 0; h < ogg->tracks[t].hdr->count; h++) {
        sz += ogg->tracks[t].hdr->sz[h];
      }
    }
  }

  if (sz == 0) {
    return -1;
  }

  sz += 2048;
  
  ogg->hdr = malloc (sz);
  
  for (t = 0; t < KRAD_OGG_MAX_TRACKS; t++) {
    if (ogg->tracks[t].bitstream_serial != 0) {
      for (h = 0; h < 1; h++) {
        ogg->hdr_sz += kr_ogg_add_data (ogg, t, 0,
                                        ogg->tracks[t].hdr->data[h],
                                        ogg->tracks[t].hdr->sz[h],
                                        ogg->hdr + ogg->hdr_sz);
      }
    }
  }
  
  for (t = 0; t < KRAD_OGG_MAX_TRACKS; t++) {
    if (ogg->tracks[t].bitstream_serial != 0) {
      for (h = 1; h < ogg->tracks[t].hdr->count; h++) {
        ogg->hdr_sz += kr_ogg_add_data (ogg, t, 0,
                                        ogg->tracks[t].hdr->data[h],
                                        ogg->tracks[t].hdr->sz[h],
                                        ogg->hdr + ogg->hdr_sz);
      }
    }
  }

  return ogg->hdr_sz;
}

void kr_ogg_page_crc_set (uint8_t *page, size_t size) {

  uint32_t crc_reg = 0;
  int i;

  page[22] = 0;
  page[23] = 0;
  page[24] = 0;
  page[25] = 0;

  for (i=0; i < size; i++) {
    crc_reg = (crc_reg<<8)^crc_lookup[((crc_reg >> 24)&0xff)^page[i]];
  }

  page[22] = (uint8_t)(crc_reg&0xff);
  page[23] = (uint8_t)((crc_reg>>8)&0xff);
  page[24] = (uint8_t)((crc_reg>>16)&0xff);
  page[25] = (uint8_t)((crc_reg>>24)&0xff);
}

static int kr_ogg_generate_page (kr_ogg_page_params_t *params, uint8_t *page) {

  uint8_t oggs[4] = {0x4f, 0x67, 0x67, 0x53};
  uint8_t ogg_version = 0;
  uint32_t header_size = 27;
  size_t page_size;
  uint8_t page_segments;
  uint16_t size_rem;
  size_t seg_pos;

  page_segments = 1;

  memcpy (page, &oggs, 4);
  page[4] = ogg_version;
  page[5] = params->header_type;
  memcpy (page + 6, &params->granule_position, 8);
  memcpy (page + 14, &params->bitstream_serial, 4);
  memcpy (page + 18, &params->page_sequence_number, 4);
  memset (page + 22, 0, 4);
  if (params->size < 255) {
    if (params->size == 0) {
      page[26] = 0;
      page_segments = 0;
    } else {
      page[26] = page_segments;
      page[27] = params->size;
      memcpy (page + 28, params->data, params->size);
    }
  } else {
    size_rem = params->size;
    seg_pos = 27;
    while (size_rem >= 255) {
      page[seg_pos] = 255;
      size_rem -= 255;
      page_segments++;
      seg_pos++;
    }
    page[26] = page_segments;
    page[seg_pos] = size_rem;
    memcpy (page + seg_pos + 1, params->data, params->size);
  }

  page_size = params->size + header_size + page_segments;

  kr_ogg_page_crc_set (page, page_size);

  //fprintf (stderr, "CRC is %1u %1u %1u %1u\n",
  //         page[22], page[23], page[24], page[25]);

  return page_size;
}

int kr_ogg_add_data (kr_ogg_t *ogg, int track, int64_t granule_position,
                     uint8_t *data, size_t size, uint8_t *page) {

  kr_ogg_page_params_t params;
  size_t page_size;

  params.granule_position = ogg->tracks[track].granule_position;
  params.bitstream_serial = ogg->tracks[track].bitstream_serial;
  params.page_sequence_number = ogg->tracks[track].page_sequence_number;
  params.granule_position = granule_position;
  params.data = data;
  params.size = size;

  if (params.page_sequence_number == 0) {
    params.header_type = 0x02;
  } else {
    if (params.size == 0) {
      params.header_type = 0x04;
    } else {
      params.header_type = 0x00;
    }
  }
  
  page_size = kr_ogg_generate_page (&params, page);

  ogg->tracks[track].page_sequence_number++;

  return page_size;
}
