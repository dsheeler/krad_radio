/**
 * @file krad_transponder_client.h
 * @brief Krad Radio Transponder Controller API
 */

/**
 * @mainpage Krad Radio Transponder Controller
 *
 * Krad Radio Transponder Controller (Kripton this is where you might want to hold back beacause it will change a bit)
 *
 */
 
 
/** @defgroup krad_transponder_client Krad Radio Transponder Control
  @{
  */

#ifndef KRAD_TRANSPONDER_CLIENT_H
#define KRAD_TRANSPONDER_CLIENT_H

#include "krad_transponder_common.h"

void kr_transponder_response_print (kr_response_t *kr_response);
int krad_link_rep_to_string (krad_link_rep_t *krad_link, char *text);
int kr_transponder_crate_to_string (kr_crate_t *crate, char **string);

void kr_transponder_info (kr_client_t *client);
void kr_transponder_adapters (kr_client_t *client);

/** Transponder **/

int kr_transponder_transmitter_enable (kr_client_t *client, int port);
void kr_transponder_transmitter_disable (kr_client_t *client);
int kr_transponder_receiver_enable (kr_client_t *client, int port);
void kr_transponder_receiver_disable (kr_client_t *client);

void kr_transponder_list (kr_client_t *client);
void kr_transponder_destroy (kr_client_t *client, int number);
void kr_transponder_update (kr_client_t *client, int number, uint32_t ebml_id, int newval);
void kr_transponder_update_str (kr_client_t *client, int number, uint32_t ebml_id, char *newval);

void kr_transponder_play (kr_client_t *client, char *path);
void kr_transponder_play_remote (kr_client_t *client, char *host, int port, char *mount);
void kr_transponder_record (kr_client_t *client, krad_link_av_mode_t av_mode, char *filename, char *codecs,
								  int video_width, int video_height, int video_bitrate, char *audio_bitrate);
void kr_transponder_capture (kr_client_t *client, krad_link_video_source_t video_source, char *device,
								   int width, int height, int fps_numerator, int fps_denominator,
								   krad_link_av_mode_t av_mode, char *audio_input, char *passthru_codec);
void kr_transponder_transmit (kr_client_t *client, krad_link_av_mode_t av_mode, char *host, int port,
									char *mount, char *password, char *codecs,
									int video_width, int video_height, int video_bitrate, char *audio_bitrate);


/**@}*/

#endif
