#include "krad_compositor.h"
#include "krad_radio_interface.h"

void kr_sprite_to_ebml(kr_ebml *ebml, kr_sprite *sprite);
void kr_text_to_ebml(kr_ebml *ebml, kr_text *text);
void kr_vector_to_ebml(kr_ebml *ebml, kr_vector *vector);
void kr_videoport_to_ebml(kr_ebml *ebml, krad_compositor_port_t *videoport);

int kr_compositor_cmd(kr_io2_t *in, kr_io2_t *out, kr_radio_client *client);
