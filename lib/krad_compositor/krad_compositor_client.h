/**
 * @file krad_compositor_client.h
 * @brief Krad Radio Compositor Controller API
 */

#include "krad_compositor_common.h"

#ifndef KRAD_COMPOSITOR_CLIENT_H
#define KRAD_COMPOSITOR_CLIENT_H


/** @defgroup krad_compositor_client Krad Radio Compositor Control
  @{
  */

typedef struct kr_videoport_St kr_videoport_t;

int kr_compositor_crate_to_rep (kr_crate_t *crate);
int kr_compositor_crate_to_string (kr_crate_t *crate, char **string);

// Public

void kr_compositor_subunit_list (kr_client_t *client);
int kr_compositor_subunit_create (kr_client_t *client,
                                  kr_compositor_subunit_t type,
                                  char *option,
                                  char *option2);
void kr_compositor_subunit_destroy (kr_client_t *client, kr_address_t *address);

void kr_compositor_info (kr_client_t *client);
void kr_compositor_set_frame_rate (kr_client_t *client, int numerator, int denominator);
void kr_compositor_set_resolution (kr_client_t *client, int width, int height);

void kr_compositor_close_display (kr_client_t *client);
void kr_compositor_open_display (kr_client_t *client, int width, int height);

int kr_compositor_background (kr_client_t *client, char *filename);
void kr_compositor_snapshot (kr_client_t *client);
void kr_compositor_snapshot_jpeg (kr_client_t *client);

/* Compositor Local Video Ports */

void kr_videoport_set_callback (kr_videoport_t *kr_videoport, int callback (void *, void *), void *pointer);
void kr_videoport_activate (kr_videoport_t *kr_videoport);
void kr_videoport_deactivate (kr_videoport_t *kr_videoport);
kr_videoport_t *kr_videoport_create (kr_client_t *client);
void kr_videoport_destroy (kr_videoport_t *kr_videoport);

/**@}*/
#endif
