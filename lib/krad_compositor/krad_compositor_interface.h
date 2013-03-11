#include "krad_compositor.h"

void krad_compositor_sprite_to_ebml2 ( kr_ebml2_t *ebml, krad_sprite_t *sprite );
void krad_compositor_text_to_ebml2 ( kr_ebml2_t *ebml, krad_text_t *text );
void krad_compositor_vector_to_ebml2 ( kr_ebml2_t *ebml, krad_vector_t *vector );
void krad_compositor_videoport_to_ebml2 ( kr_ebml2_t *ebml, krad_compositor_port_t *videoport );

int krad_compositor_handler ( krad_compositor_t *krad_compositor, krad_ipc_server_t *krad_ipc );
