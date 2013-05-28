#include "krad_compositor_port.h"

#ifndef KRAD_VIDEOPORT_H
#define KRAD_VIDEOPORT_H

void krad_compositor_videoport_set_view_top_left_x (krad_compositor_port_t *port, int view_top_left_x);
void krad_compositor_videoport_set_view_top_left_y (krad_compositor_port_t *port, int view_top_left_y);
void krad_compositor_videoport_set_view_top_right_x (krad_compositor_port_t *port, int view_top_right_x);
void krad_compositor_videoport_set_view_top_right_y (krad_compositor_port_t *port, int view_top_right_y);
void krad_compositor_videoport_set_view_bottom_left_x (krad_compositor_port_t *port, int view_bottom_left_x);
void krad_compositor_videoport_set_view_bottom_left_y (krad_compositor_port_t *port, int view_bottom_left_y);
void krad_compositor_videoport_set_view_bottom_right_x (krad_compositor_port_t *port, int view_bottom_right_x);
void krad_compositor_videoport_set_view_bottom_right_y (krad_compositor_port_t *port, int view_bottom_right_y);

#endif
