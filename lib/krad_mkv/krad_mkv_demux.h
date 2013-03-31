#include "krad_mkv.h"
#include "krad_ebml2w.h"

#ifndef KRAD_MKV_DEMUX_H
#define KRAD_MKV_DEMUX_H

kr_mkv_t *kr_mkv_open_file (char *filename);


int kr_mkv_track_count(kr_mkv_t *kr_mkv);
krad_codec_t kr_mkv_track_codec (kr_mkv_t *kr_mkv, int track);
int kr_mkv_track_header_count (kr_mkv_t *kr_mkv, int track);
int kr_mkv_track_header_size (kr_mkv_t *kr_mkv, int track, int header);
int kr_mkv_read_track_header (kr_mkv_t *kr_mkv, unsigned char *buffer,
                             int track, int header);
int kr_mkv_track_active (kr_mkv_t *kr_mkv, int track);
int kr_mkv_track_changed (kr_mkv_t *kr_mkv, int track);


/*
int kr_mkv_read_packet (kr_mkv_t *kr_mkv, int *track,
                        uint64_t *timecode, unsigned char *buffer);
*/
#endif
