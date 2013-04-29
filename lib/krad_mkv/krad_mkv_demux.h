#include "krad_mkv.h"
#include "krad_ebml2w.h"
#include "krad_codec_header.h"

#ifndef KRAD_MKV_DEMUX_H
#define KRAD_MKV_DEMUX_H

#define KRAD_MKV_CODEC_HDR_MAX_SANE_SZ 25000

kr_mkv_t *kr_mkv_open_file (char *filename);

kr_mkv_t *kr_mkv_open_stream (char *host, int port, char *mount);

int kr_mkv_track_count (kr_mkv_t *mkv);
krad_codec_t kr_mkv_track_codec (kr_mkv_t *mkv, uint32_t track);
int kr_mkv_track_header_count (kr_mkv_t *mkv, uint32_t track);
int kr_mkv_track_header_size (kr_mkv_t *mkv, uint32_t track, uint32_t header);
int kr_mkv_read_track_header (kr_mkv_t *mkv, uint8_t *buffer,
                             uint32_t track, uint32_t header);
int kr_mkv_track_active (kr_mkv_t *mkv, uint32_t track);
int kr_mkv_track_changed (kr_mkv_t *mkv, uint32_t track);

int kr_mkv_read_packet (kr_mkv_t *mkv, uint32_t *track,
                        uint64_t *timecode, uint8_t *flags, uint8_t *buffer);
#endif
