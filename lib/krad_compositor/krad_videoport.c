#include "krad_videoport.h"

void krad_compositor_videoport_set_view_top_left_x (krad_compositor_port_t *videoport, int view_top_left_x) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }

  videoport->view.top_left.x = view_top_left_x;

  if (kr_perspective_set(videoport->perspective, &videoport->view) < 0) {
    videoport->view = videoport->perspective->view;
    videoport->view.top_left.x = videoport->perspective->view.top_left.x;
  }

}

void krad_compositor_videoport_set_view_top_left_y (krad_compositor_port_t *videoport, int view_top_left_y) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }

  videoport->view.top_left.y = view_top_left_y;

  if (kr_perspective_set(videoport->perspective, &videoport->view) < 0) {
    videoport->view.top_left.y = videoport->perspective->view.top_left.y;
  }

}

void krad_compositor_videoport_set_view_top_right_x (krad_compositor_port_t *videoport, int view_top_right_x) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }

  videoport->view.top_right.x = view_top_right_x;

  if (kr_perspective_set(videoport->perspective, &videoport->view) < 0) {
    videoport->view.top_right.x = videoport->perspective->view.top_right.x;
  }

}

void krad_compositor_videoport_set_view_top_right_y (krad_compositor_port_t *videoport, int view_top_right_y) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }

  videoport->view.top_right.y = view_top_right_y;

  if (kr_perspective_set(videoport->perspective, &videoport->view) < 0) {
    videoport->view.top_right.y = videoport->perspective->view.top_right.y;
  }
}

void krad_compositor_videoport_set_view_bottom_left_x (krad_compositor_port_t *videoport, int view_bottom_left_x) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }

  videoport->view.bottom_left.x = view_bottom_left_x;

  if (kr_perspective_set(videoport->perspective, &videoport->view) < 0) {
    videoport->view.bottom_left.x = videoport->perspective->view.bottom_left.x;
  }

}

void krad_compositor_videoport_set_view_bottom_left_y (krad_compositor_port_t *videoport, int view_bottom_left_y) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }

  videoport->view.bottom_left.y = view_bottom_left_y;

  if (kr_perspective_set(videoport->perspective, &videoport->view) < 0) {
    videoport->view.bottom_left.y = videoport->perspective->view.bottom_left.y;
  }

}

void krad_compositor_videoport_set_view_bottom_right_x (krad_compositor_port_t *videoport, int view_bottom_right_x) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }

  videoport->view.bottom_right.x = view_bottom_right_x;

  if (kr_perspective_set(videoport->perspective, &videoport->view) < 0) {
    videoport->view.bottom_right.x = videoport->perspective->view.bottom_right.x;
  }

}

void krad_compositor_videoport_set_view_bottom_right_y (krad_compositor_port_t *videoport, int view_bottom_right_y) {

  if (videoport->perspective == NULL) {
      videoport->perspective = kr_perspective_create (960,540);
      videoport->view = videoport->perspective->view;
  }

  videoport->view.bottom_right.y = view_bottom_right_y;

  if (kr_perspective_set(videoport->perspective, &videoport->view) < 0) {
    videoport->view.bottom_right.y = videoport->perspective->view.bottom_right.y;
  }

}
