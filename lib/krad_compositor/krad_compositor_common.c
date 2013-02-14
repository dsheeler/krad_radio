#include "krad_compositor_common.h"

void krad_compositor_subunit_controls_to_ebml (krad_ebml_t *krad_ebml, kr_compositor_subunit_controls_t *krad_compositor_subunit_controls) {
  
  krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_NUMBER, krad_compositor_subunit_controls->number);
  krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_COMPOSITOR_X, krad_compositor_subunit_controls->x);
  krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_COMPOSITOR_Y, krad_compositor_subunit_controls->y);
  krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_COMPOSITOR_Y, krad_compositor_subunit_controls->z);
  krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_TICKRATE, krad_compositor_subunit_controls->tickrate);
  
  krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_SCALE, krad_compositor_subunit_controls->xscale);
  krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_SCALE, krad_compositor_subunit_controls->yscale);
  krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY, krad_compositor_subunit_controls->opacity);
  krad_ebml_write_float (krad_ebml, EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION, krad_compositor_subunit_controls->rotation);

}
void krad_compositor_subunit_controls_read (krad_ebml_t *krad_ebml, kr_compositor_subunit_controls_t *subunit_controls) {
  uint32_t ebml_id;
  
  uint64_t ebml_data_size;
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_SPRITE_NUMBER) {
    subunit_controls->number = krad_ebml_read_number (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_X) {
    subunit_controls->x = krad_ebml_read_number (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_Y) {
    subunit_controls->y = krad_ebml_read_number (krad_ebml, ebml_data_size);
  }
      
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_Y) {
    subunit_controls->z = krad_ebml_read_number (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_SPRITE_TICKRATE) {
    subunit_controls->tickrate = krad_ebml_read_number (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_SPRITE_SCALE) {
    subunit_controls->xscale = krad_ebml_read_float (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_SPRITE_SCALE) {
     subunit_controls->yscale = krad_ebml_read_float (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_SPRITE_OPACITY) {
    subunit_controls->opacity = krad_ebml_read_float (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_SPRITE_ROTATION) {
    subunit_controls->rotation = krad_ebml_read_float (krad_ebml, ebml_data_size);
  }    
  
}

void krad_compositor_subunit_controls_init (kr_comp_controls_t *controls, int number, int x, int y, int z, int tickrate, int width, int height,
                                                                                    float scale, float opacity, float rotation) {

  krad_compositor_subunit_controls_reset (controls);
  
  controls->x = x;
  controls->y = y;
  controls->z = z;
  
  controls->number = number;
  controls->tickrate = tickrate;
  
  controls->width = width;
  controls->height = height;
  
  controls->xscale = scale;
  controls->yscale = scale;
  
  controls->rotation = rotation;
  controls->opacity = opacity;
}

void krad_compositor_subunit_controls_reset (kr_compositor_subunit_controls_t *krad_compositor_subunit_controls) {
  krad_compositor_subunit_controls->x = 0;
  krad_compositor_subunit_controls->y = 0;
  krad_compositor_subunit_controls->z = 0;
  
  krad_compositor_subunit_controls->tickrate = KRAD_COMPOSITOR_SUBUNIT_DEFAULT_TICKRATE;
  
  krad_compositor_subunit_controls->number = 0;
  
  krad_compositor_subunit_controls->width = 0;
  krad_compositor_subunit_controls->height = 0;
  
  krad_compositor_subunit_controls->xscale = 1.0f;
  krad_compositor_subunit_controls->yscale = 1.0f;
  
  krad_compositor_subunit_controls->rotation = 0.0f;
  krad_compositor_subunit_controls->opacity = 0.0f;

}

void krad_compositor_validate_text_rep (krad_text_rep_t *krad_text_rep) {
  
  if ((krad_text_rep->red < 0.0) || (krad_text_rep->red > 0.255)) {
    printk("bad red value: %f", krad_text_rep->red);
  }
  
  if ((krad_text_rep->green < 0.0) || (krad_text_rep->green > 0.255)) {
    printk("bad green value: %f", krad_text_rep->green);
  }
  
  if ((krad_text_rep->blue < 0.0) || (krad_text_rep->blue > 0.255)) {
    printk("bad blue value: %f", krad_text_rep->blue);
  }

}

krad_text_rep_t *krad_compositor_text_rep_create () {
  
  krad_text_rep_t *krad_text_rep = calloc(1, sizeof (krad_text_rep_t));
  return krad_text_rep;
}

void krad_compositor_text_rep_destroy (krad_text_rep_t *krad_text_rep) {
  free (krad_text_rep);
}

void krad_compositor_text_rep_to_ebml (krad_text_rep_t *krad_text_rep, krad_ebml_t *krad_ebml) {
  
  //uint64_t cmd;
  
  //krad_ebml_start_element (krad_ebml, EBML_ID_KRAD_COMPOSITOR_TEXT, &cmd);  
  
  krad_ebml_write_string (krad_ebml, EBML_ID_KRAD_COMPOSITOR_TEXT, krad_text_rep->text);
  krad_ebml_write_string (krad_ebml, EBML_ID_KRAD_COMPOSITOR_FONT, krad_text_rep->font);
  
  krad_ebml_write_float (krad_ebml,
                         EBML_ID_KRAD_COMPOSITOR_RED,
                         krad_text_rep->red);
  krad_ebml_write_float (krad_ebml,
                         EBML_ID_KRAD_COMPOSITOR_GREEN,
                         krad_text_rep->green);
  krad_ebml_write_float (krad_ebml,
                         EBML_ID_KRAD_COMPOSITOR_BLUE,
                         krad_text_rep->blue);
  

  krad_compositor_subunit_controls_to_ebml (krad_ebml, &krad_text_rep->controls);

  //krad_ebml_finish_element (krad_ebml, cmd);
}

krad_text_rep_t *krad_compositor_ebml_to_krad_text_rep (krad_ebml_t *krad_ebml, uint64_t *data_read, krad_text_rep_t *krad_text_rep) {
  
  uint32_t ebml_id;
  uint64_t ebml_data_size;
  krad_text_rep_t *krad_text_rep_ret;
  
  if (krad_text_rep == NULL) {
    krad_text_rep_ret = krad_compositor_text_rep_create();
  } else {
    krad_text_rep_ret = krad_text_rep;
  }
  //krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  //if (ebml_id == EBML_ID_KRAD_COMPOSITOR_TEXT) {
  //  *data_read = ebml_data_size;
  //}
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);  
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_TEXT) {
    krad_ebml_read_string (krad_ebml, krad_text_rep_ret->text, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);  
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_FONT) {
    krad_ebml_read_string (krad_ebml, krad_text_rep_ret->font, ebml_data_size);      
  } 
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_RED) {
    krad_text_rep_ret->red = krad_ebml_read_float (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_GREEN) {
    krad_text_rep_ret->green = krad_ebml_read_float (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_BLUE) {
    krad_text_rep_ret->blue = krad_ebml_read_float (krad_ebml, ebml_data_size);
  }

  krad_compositor_subunit_controls_read (krad_ebml, &krad_text_rep_ret->controls);
  
  return krad_text_rep_ret;
}

void krad_compositor_text_rep_reset (krad_text_rep_t *krad_text_rep) {
  //kr_compositor_subunit_controls_t *krad_controls = krad_text_rep->controls;
  //krad_compositor_subunit_controls_reset (krad_controls);
  //memset (krad_text_rep, 0, sizeof (krad_text_rep_t));
  //krad_text_rep->controls = krad_controls;
}


krad_text_rep_t *krad_compositor_text_rep_create_and_init (int number, char *text, char *font, float red, float green, float blue, int x, int y, int z, int tickrate, float scale, float opacity, float rotation) {

  krad_text_rep_t *krad_text_rep = calloc(1, sizeof (krad_text_rep_t));
  strcpy (krad_text_rep->text, text);
  strcpy (krad_text_rep->font, font);
  krad_text_rep->red = red;
  krad_text_rep->green = green;
  krad_text_rep->blue = blue;
  
  krad_compositor_subunit_controls_init (&krad_text_rep->controls, number, x, y, z, tickrate, 0, 0, scale, opacity, rotation); 
  return krad_text_rep;
}

kr_sprite_t *kr_compositor_sprite_rep_create () {
  kr_sprite_t *sprite = calloc (1, sizeof (kr_sprite_t));
  return sprite;
}

void kr_compositor_sprite_rep_destroy (kr_sprite_t *sprite) {
  free (sprite);
}


krad_sprite_rep_t *krad_compositor_sprite_rep_create_and_init ( int number, char *filename, int x, int y, int z, int tickrate, 
                                                            float scale, float opacity, float rotation) {
  
  krad_sprite_rep_t *krad_sprite_rep = calloc(1, sizeof (krad_sprite_rep_t));
  
  strcpy (krad_sprite_rep->filename, filename);
      
  krad_compositor_subunit_controls_init (&krad_sprite_rep->controls, number, x, y, z, tickrate, 0, 0, scale, opacity, rotation); 
  return krad_sprite_rep;
}

void krad_compositor_sprite_rep_to_ebml (krad_sprite_rep_t *krad_sprite_rep, krad_ebml_t *krad_ebml) {
  krad_ebml_write_string (krad_ebml, EBML_ID_KRAD_COMPOSITOR_FILENAME, krad_sprite_rep->filename);
  krad_compositor_subunit_controls_to_ebml (krad_ebml, &krad_sprite_rep->controls);
}

krad_sprite_rep_t *krad_compositor_ebml_to_new_krad_sprite_rep (krad_ebml_t *krad_ebml, uint64_t *bytes_read) {

  uint32_t ebml_id;
  uint64_t ebml_data_size;

  krad_sprite_rep_t *krad_sprite_rep = kr_compositor_sprite_rep_create ();

  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);  
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_FILENAME) {
    krad_ebml_read_string (krad_ebml, krad_sprite_rep->filename, ebml_data_size);
  }

  krad_compositor_subunit_controls_read (krad_ebml, &krad_sprite_rep->controls);

  return krad_sprite_rep;
}

void krad_compositor_validate_vector_rep (krad_vector_rep_t *krad_vector_rep) {
  
  if ((krad_vector_rep->red < 0.0) || (krad_vector_rep->red > 0.255)) {
    printk("bad red value: %f", krad_vector_rep->red);
  }
  
  if ((krad_vector_rep->green < 0.0) || (krad_vector_rep->green > 0.255)) {
    printk("bad green value: %f", krad_vector_rep->green);
  }
  
  if ((krad_vector_rep->blue < 0.0) || (krad_vector_rep->blue > 0.255)) {
    printk("bad blue value: %f", krad_vector_rep->blue);
  }

}

krad_vector_rep_t *krad_compositor_vector_rep_create () {
  krad_vector_rep_t *krad_vector_rep = calloc(1, sizeof (krad_vector_rep_t));
  return krad_vector_rep;
}

void krad_compositor_vector_rep_destroy (krad_vector_rep_t *krad_vector_rep) {
  free (krad_vector_rep);
}

void krad_compositor_vector_rep_to_ebml (krad_vector_rep_t *krad_vector_rep, krad_ebml_t *krad_ebml) {

  uint64_t cmd;

  krad_ebml_start_element (krad_ebml, EBML_ID_KRAD_COMPOSITOR_TEXT, &cmd);

  krad_ebml_write_string (krad_ebml,
                          EBML_ID_KRAD_COMPOSITOR_TEXT,
                          krad_vector_type_to_string (krad_vector_rep->krad_vector_type));
  
  krad_ebml_write_float (krad_ebml,
                         EBML_ID_KRAD_COMPOSITOR_RED,
                         krad_vector_rep->red);
  krad_ebml_write_float (krad_ebml,
                         EBML_ID_KRAD_COMPOSITOR_GREEN,
                         krad_vector_rep->green);
  krad_ebml_write_float (krad_ebml,
                         EBML_ID_KRAD_COMPOSITOR_BLUE,
                         krad_vector_rep->blue);

  krad_compositor_subunit_controls_to_ebml (krad_ebml, &krad_vector_rep->controls);
  krad_ebml_finish_element (krad_ebml, cmd);
}

krad_vector_rep_t *krad_compositor_ebml_to_krad_vector_rep (krad_ebml_t *krad_ebml, uint64_t *data_read, krad_vector_rep_t *krad_vector_rep) {
  
  uint32_t ebml_id;
  uint64_t ebml_data_size;
  krad_vector_rep_t *krad_vector_rep_ret;
  char vector_type_string[128];
  
  if (krad_vector_rep == NULL) {
    krad_vector_rep_ret = krad_compositor_vector_rep_create();
  } else {
    krad_vector_rep_ret = krad_vector_rep;
  }
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_TEXT) {
    *data_read = ebml_data_size;
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);  
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_TEXT) {
    krad_ebml_read_string (krad_ebml, vector_type_string, ebml_data_size);
  }
  
  krad_vector_rep->krad_vector_type = krad_string_to_vector_type (vector_type_string);
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_RED) {
    krad_vector_rep_ret->red = krad_ebml_read_float (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_GREEN) {
    krad_vector_rep_ret->green = krad_ebml_read_float (krad_ebml, ebml_data_size);
  }
  
  krad_ebml_read_element (krad_ebml, &ebml_id, &ebml_data_size);
  if (ebml_id == EBML_ID_KRAD_COMPOSITOR_BLUE) {
    krad_vector_rep_ret->blue = krad_ebml_read_float (krad_ebml, ebml_data_size);
  }

  krad_compositor_subunit_controls_read (krad_ebml, &krad_vector_rep_ret->controls);
  
  return krad_vector_rep_ret;
}

void krad_compositor_vector_rep_reset (krad_vector_rep_t *krad_vector_rep) {
  //kr_compositor_subunit_controls_t *krad_controls = krad_vector_rep->controls;
  //krad_compositor_subunit_controls_reset (krad_controls);
  //memset (krad_vector_rep, 0, sizeof (krad_vector_rep_t));
  //krad_vector_rep->controls = krad_controls;
}


krad_vector_rep_t *krad_compositor_vector_rep_create_and_init (int number, char *vector_type_string, float red, float green, float blue, int x, int y, int z, int tickrate, float scale, float opacity, float rotation) {

  krad_vector_rep_t *krad_vector_rep = calloc(1, sizeof (krad_vector_rep_t));
  krad_vector_rep->krad_vector_type = krad_string_to_vector_type (vector_type_string);
  krad_vector_rep->red = red;
  krad_vector_rep->green = green;
  krad_vector_rep->blue = blue;

  krad_compositor_subunit_controls_init (&krad_vector_rep->controls, number, x, y, z, tickrate, 0, 0, scale, opacity, rotation); 
  return krad_vector_rep;
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

kr_compositor_t *kr_compositor_rep_create () {
  kr_compositor_t *compositor = (kr_compositor_t *) calloc (1, sizeof (kr_compositor_t));
  return compositor;
}

void kr_compositor_rep_destroy (kr_compositor_t *compositor) {
  free (compositor);
}

