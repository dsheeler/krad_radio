#include "krad_videoport.h"

void krad_compositor_videoport_set_view_top_left_x (krad_compositor_port_t *videoport, int view_top_left_x) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }
  videoport->view.top_left.x = view_top_left_x;
  printk("hello x %d", videoport->view.top_left.x);

  printk("output of kr_perspective %d", kr_perspective_set(videoport->perspective, &videoport->view));
}

void krad_compositor_videoport_set_view_top_left_y (krad_compositor_port_t *videoport, int view_top_left_y) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }

  videoport->view.top_left.y = view_top_left_y;
  printk("hello y %d", videoport->view.top_left.y);
  kr_perspective_set(videoport->perspective, &videoport->view);
}

void krad_compositor_videoport_set_view_top_right_x (krad_compositor_port_t *videoport, int view_top_right_x) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }

  videoport->view.top_right.x = view_top_right_x;
  kr_perspective_set(videoport->perspective, &videoport->view);
}

void krad_compositor_videoport_set_view_top_right_y (krad_compositor_port_t *videoport, int view_top_right_y) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }

  videoport->view.top_right.y = view_top_right_y;
  kr_perspective_set(videoport->perspective, &videoport->view);
}

void krad_compositor_videoport_set_view_bottom_left_x (krad_compositor_port_t *videoport, int view_bottom_left_x) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }

  videoport->view.bottom_left.x = view_bottom_left_x;
  kr_perspective_set(videoport->perspective, &videoport->view);
}

void krad_compositor_videoport_set_view_bottom_left_y (krad_compositor_port_t *videoport, int view_bottom_left_y) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }

  videoport->view.bottom_left.y = view_bottom_left_y;
  kr_perspective_set(videoport->perspective, &videoport->view);
}

void krad_compositor_videoport_set_view_bottom_right_x (krad_compositor_port_t *videoport, int view_bottom_right_x) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }

  videoport->view.bottom_right.x = view_bottom_right_x;
  kr_perspective_set(videoport->perspective, &videoport->view);
}

void krad_compositor_videoport_set_view_bottom_right_y (krad_compositor_port_t *videoport, int view_bottom_right_y) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }

  videoport->view.bottom_right.y = view_bottom_right_y;
  kr_perspective_set(videoport->perspective, &videoport->view);
}
