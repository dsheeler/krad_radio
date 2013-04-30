#include "krad_ogg2.h"

int kr_ogg_destroy (kr_ogg_t **ogg) {

  if ((ogg == NULL) || (*ogg == NULL)) {
    return -1;
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

int kr_ogg_add_track (kr_ogg_t *ogg) {

  int t;
  
  for (t = 0; t < KRAD_OGG_MAX_TRACKS; t++) {
    if (ogg->tracks[t].bitstream_serial == 0) {
      ogg->tracks[t].bitstream_serial = rand () + time(NULL);
      return t;
    } 
  }
  return -1;
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

int kr_ogg_generate_page (kr_ogg_page_params_t *page_params, uint8_t *page) {

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
  page[5] = page_params->header_type;
  memcpy (page + 6, &page_params->granule_position, 8);
  memcpy (page + 14, &page_params->bitstream_serial, 4);
  memcpy (page + 18, &page_params->page_sequence_number, 4);
  memset (page + 22, 0, 4);
  if (page_params->size < 255) {
    page[26] = page_segments;
    page[27] = page_params->size;
    memcpy (page + 28, page_params->data, page_params->size);
  } else {
    size_rem = page_params->size;
    seg_pos = 27;
    while (size_rem >= 255) {
      page[seg_pos] = 255;
      size_rem -= 255;
      page_segments++;
      seg_pos++;
    }
    page[26] = page_segments;
    page[seg_pos] = size_rem;
    memcpy (page + seg_pos + 1, page_params->data, page_params->size);
  }

  page_size = page_params->size + header_size + page_segments;

  kr_ogg_page_crc_set (page, page_size);

  //fprintf (stderr, "CRC is %1u %1u %1u %1u\n",
  //         page[22], page[23], page[24], page[25]);

  return page_size;
}

int kr_ogg_add_data (kr_ogg_t *ogg, int track,
                     uint8_t *data, size_t size, uint8_t *page) {

  kr_ogg_page_params_t page_params;
  size_t page_size;

  page_params.granule_position = ogg->tracks[track].granule_position;
  page_params.bitstream_serial = ogg->tracks[track].bitstream_serial;
  page_params.page_sequence_number = ogg->tracks[track].page_sequence_number;
  page_params.data = data;
  page_params.size = size;

  if (page_params.page_sequence_number == 0) {
    page_params.header_type = 0x02;
  } else {
    if (page_params.size == 0) {
      page_params.header_type = 0x04;
    } else {
      page_params.header_type = 0x00;
    }
  }
  
  if (page_params.size == 0) {
    page_params.granule_position = -1;
  }

  page_size = kr_ogg_generate_page (&page_params, page);

  ogg->tracks[track].page_sequence_number++;
  if (page_params.page_sequence_number > 3) {
    ogg->tracks[track].granule_position += 1;
  }

  return page_size;
}
