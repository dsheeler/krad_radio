#include "krad_compositor_common.h"

void kr_aspect_upscale(int sw, int sh, int dw, int dh, int *w, int *h) {

  double scale_x, scale_y, scale;

  scale_x = (float)dw  / sw;
  scale_y = (float)dh / sh;
  scale = MIN(scale_x, scale_y);

  *w = sw * scale;
  *h = sh * scale;
}

kr_compositor_control_t krad_string_to_compositor_control(char *string) {
  if (strncmp (string, "xscale", 2) == 0) {
    return KR_XSCALE;
  }
  if (strncmp (string, "yscale", 2) == 0) {
    return KR_YSCALE;
  }
  if (strncmp (string, "red", 2) == 0) {
    return KR_RED;
  }
  if (strncmp (string, "x", 1) == 0) {
    return KR_X;
  }
  if (strncmp (string, "y", 1) == 0) {
    return KR_Y;
  }
  if (strncmp (string, "z", 1) == 0) {
    return KR_Z;
  }
  if (strncmp (string, "width", 1) == 0) {
    return KR_WIDTH;
  }
  if (strncmp (string, "height", 1) == 0) {
    return KR_HEIGHT;
  }
  if (strncmp (string, "rotation", 1) == 0) {
    return KR_ROTATION;
  }
  if (strncmp (string, "opacity", 1) == 0) {
    return KR_OPACITY;
  }
  if (strncmp (string, "green", 1) == 0) {
    return KR_GREEN;
  }
  if (strncmp (string, "blue", 1) == 0) {
    return KR_BLUE;
  }
  if (strncmp (string, "alpha", 1) == 0) {
    return KR_ALPHA;
  }
  if (strncmp (string, "tickrate", 1) == 0) {
    return KR_TICKRATE;
  }
  return 0;
}

krad_vector_type_t krad_string_to_vector_type(char *string) {
  if (strncmp (string, "hex", 3) == 0) {
    return HEX;
  }
  if (strncmp (string, "circle", 6) == 0) {
    return CIRCLE;
  }
  if (strncmp (string, "rect", 4) == 0) {
    return RECT;
  }
  if (strncmp (string, "triangle", 3) == 0) {
    return TRIANGLE;
  }
  if (strncmp (string, "viper", 5) == 0) {
    return VIPER;
  }
  if (strncmp (string, "meter", 5) == 0) {
    return METER;
  }
  if (strncmp (string, "grid", 4) == 0) {
    return GRID;
  }
  if (strncmp (string, "curve", 5) == 0) {
    return CURVE;
  }
  if (strncmp (string, "arrow", 5) == 0) {
    return ARROW;
  }
  if (strncmp (string, "clock", 5) == 0) {
    return CLOCK;
  }
  if (strncmp (string, "shadow", 6) == 0) {
    return SHADOW;
  }
  return NOTHING;
}

char *kr_compositor_control_to_string(kr_compositor_control_t control) {
  switch ( control ) {
    case KR_NO:
      break;
    case KR_X:
      return "x";
    case KR_Y:
      return "y";
    case KR_Z:
      return "z";
    case KR_WIDTH:
      return "width";
    case KR_HEIGHT:
      return "height";
    case KR_ROTATION:
      return "rotation";
    case KR_OPACITY:
      return "opacity";
    case KR_XSCALE:
      return "xscale";
    case KR_YSCALE:
      return "yscale";
    case KR_RED:
      return "red";
    case KR_GREEN:
      return "green";
    case KR_BLUE:
      return "blue";
    case KR_ALPHA:
      return "alpha";
    case KR_TICKRATE:
      return "tickrate";
  }

  return "Unknown";
}

char *krad_vector_type_to_string(krad_vector_type_t type) {
  switch (type) {
    case HEX:
      return "hex";
    case CIRCLE:
      return "circle";
    case RECT:
      return "rect";
    case TRIANGLE:
      return "triangle";
    case VIPER:
      return "viper";
    case METER:
      return "meter";
    case GRID:
      return "grid";
    case CURVE:
      return "curve";
    case ARROW:
      return "arrow";
    case CLOCK:
      return "clock";
    case SHADOW:
      return "shadow";
    default:
      break;
  }

  return "Unknown";
}

char *kr_compositor_subunit_type_to_string(kr_compositor_subunit_t type) {
  switch (type) {
    case KR_SPRITE:
      return "sprite";
    case KR_TEXT:
      return "text";
    case KR_VECTOR:
      return "vector";
    case KR_VIDEOPORT:
      return "videoport";
    default:
      break;
  }
  return "Unknown";
}

kr_compositor_subunit_t kr_string_to_comp_subunit_type(char *string) {
  if (strncmp (string, "sprite", 6) == 0) {
    return KR_SPRITE;
  }
  if (strncmp (string, "text", 4) == 0) {
    return KR_TEXT;
  }
  if (strncmp (string, "vector", 6) == 0) {
    return KR_VECTOR;
  }
  if (strncmp (string, "videoport", 9) == 0) {
    return KR_VIDEOPORT;
  }
  return 0;
}
