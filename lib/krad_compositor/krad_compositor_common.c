#include "krad_compositor_common.h"

void krad_compositor_videoport_rep_to_ebml (kr_port_t *port, krad_ebml_t *ebml) {

  krad_ebml_write_string (ebml, EBML_ID_KRAD_COMPOSITOR_TEXT, port->sysname);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_X, port->direction);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_X, port->controls.x);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->controls.y);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->controls.z);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->source_width);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->source_height);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->crop_x);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->crop_y);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->crop_width);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->crop_height);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->controls.width);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, port->controls.height);  
  krad_ebml_write_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY, port->controls.opacity);
  krad_ebml_write_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION, port->controls.rotation);
}

void krad_compositor_text_rep_to_ebml (krad_text_rep_t *text, krad_ebml_t *ebml) {

  krad_ebml_write_string (ebml, EBML_ID_KRAD_COMPOSITOR_TEXT, text->text);
  krad_ebml_write_string (ebml, EBML_ID_KRAD_COMPOSITOR_FONT, text->font);
  
  krad_ebml_write_float (ebml,
                         EBML_ID_KRAD_COMPOSITOR_RED,
                         text->red);
  krad_ebml_write_float (ebml,
                         EBML_ID_KRAD_COMPOSITOR_GREEN,
                         text->green);
  krad_ebml_write_float (ebml,
                         EBML_ID_KRAD_COMPOSITOR_BLUE,
                         text->blue);

  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_X, text->controls.x);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, text->controls.y);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, text->controls.z);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, text->controls.width);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, text->controls.height);  
  krad_ebml_write_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_SCALE, text->controls.xscale);
  krad_ebml_write_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY, text->controls.opacity);
  krad_ebml_write_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION, text->controls.rotation);
}

void krad_compositor_sprite_rep_to_ebml (krad_sprite_rep_t *sprite, krad_ebml_t *ebml) {
  krad_ebml_write_string (ebml, EBML_ID_KRAD_COMPOSITOR_FILENAME, sprite->filename);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_X, sprite->controls.x);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, sprite->controls.y);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, sprite->controls.z);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_TICKRATE, sprite->controls.tickrate);
  krad_ebml_write_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_SCALE, sprite->controls.xscale);
  krad_ebml_write_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_SCALE, sprite->controls.yscale);
  krad_ebml_write_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY, sprite->controls.opacity);
  krad_ebml_write_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION, sprite->controls.rotation);
}

void krad_compositor_vector_rep_to_ebml (krad_vector_rep_t *vector, krad_ebml_t *ebml) {
  //krad_ebml_write_string (ebml, EBML_ID_KRAD_COMPOSITOR_FILENAME, krad_vector_type_to_string (vector->type));
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_X, vector->type);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_X, vector->controls.x);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, vector->controls.y);
  krad_ebml_write_int32 (ebml, EBML_ID_KRAD_COMPOSITOR_Y, vector->controls.z);
  krad_ebml_write_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_SCALE, vector->controls.xscale);
  krad_ebml_write_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_SCALE, vector->controls.yscale);
  krad_ebml_write_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY, vector->controls.opacity);
  krad_ebml_write_float (ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION, vector->controls.rotation);
}

kr_compositor_control_t krad_string_to_compositor_control (char *string) {

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

krad_vector_type_t krad_string_to_vector_type (char *string) {

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

char *krad_vector_type_to_string (krad_vector_type_t type) {

  switch ( type ) {
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

char *kr_compositor_subunit_type_to_string (kr_compositor_subunit_t type) {

  switch ( type ) {
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
