/**
 * @file krad_mixer_client.h
 * @brief Krad Radio Mixer Controller API
 */

/**
 * @mainpage Krad Radio Mixer Controller
 *
 * Krad Radio Mixer Controller (Kripton this is where you come in agin really need ya here)
 *
 */

#ifndef KRAD_MIXER_CLIENT_H
#define KRAD_MIXER_CLIENT_H

/** @defgroup krad_mixer_client Krad Radio Mixer Control
  @{
  */

typedef struct kr_audioport_St kr_audioport_t;

#include "krad_sfx_common.h"
#include "krad_mixer_common.h"


/** Mixer **/

int kr_mixer_crate_to_string (kr_crate_t *kr_crate, char **string);
int kr_mixer_crate_to_rep (kr_crate_t *crate);

void kr_mixer_portgroup_xmms2_cmd (kr_client_t *client, char *portgroupname, char *xmms2_cmd);
void kr_mixer_set_sample_rate (kr_client_t *client, int sample_rate);
void kr_mixer_plug_portgroup (kr_client_t *client, char *name, char *remote_name);
void kr_mixer_unplug_portgroup (kr_client_t *client, char *name, char *remote_name);
void kr_mixer_set_portgroup_crossfade_group (kr_client_t *client, char *portgroupname, char *crossfade_group);
void kr_mixer_update_portgroup_map_channel (kr_client_t *client, char *portgroupname, int in_channel, int out_channel);
void kr_mixer_update_portgroup_mixmap_channel (kr_client_t *client, char *portgroupname, int in_channel, int out_channel);
void kr_mixer_push_tone (kr_client_t *client, char *tone);
void kr_mixer_bind_portgroup_xmms2 (kr_client_t *client, char *portgroupname, char *ipc_path);
void kr_mixer_unbind_portgroup_xmms2 (kr_client_t *client, char *portgroupname);
void kr_mixer_create_portgroup (kr_client_t *client, char *name, char *direction, int channels);
void kr_mixer_remove_portgroup (kr_client_t *client, char *portgroupname);
void kr_mixer_portgroup_list (kr_client_t *client);
void kr_mixer_info (kr_client_t *client);
void kr_mixer_portgroup_info (kr_client_t *client, char *portgroupname);
void kr_mixer_set_control (kr_client_t *client, char *portgroup_name, char *control_name, float control_value, uint32_t duration);
void kr_mixer_set_effect_control (kr_client_t *client, char *portgroup_name, int effect_num, 
                                  int control_id, char *control_name, float control_value, int duration,
                                  krad_ease_t ease);

int kr_mixer_get_info_wait (kr_client_t *client,
                            uint32_t *sample_rate,
                            uint32_t *period_size);

/* Mixer Local Audio Ports */
int kr_audioport_error (kr_audioport_t *audioport);
float *kr_audioport_get_buffer (kr_audioport_t *kr_audioport, int channel);
void kr_audioport_set_callback (kr_audioport_t *kr_audioport, int callback (uint32_t, void *), void *pointer);
void kr_audioport_activate (kr_audioport_t *kr_audioport);
void kr_audioport_deactivate (kr_audioport_t *kr_audioport);
kr_audioport_t *kr_audioport_create (kr_client_t *client, krad_mixer_portgroup_direction_t direction);
void kr_audioport_destroy (kr_audioport_t *kr_audioport);

/**@}*/
#endif
