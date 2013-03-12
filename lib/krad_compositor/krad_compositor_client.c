#include "krad_radio_client.h"
#include "krad_radio_client_internal.h"
#include "krad_compositor_common.h"

static void kr_ebml_to_compositor_rep (kr_ebml2_t *ebml, kr_compositor_t *kr_compositor_rep);
static int kr_compositor_crate_get_string_from_subunit (kr_crate_t *crate, char **string);

typedef struct kr_videoport_St kr_videoport_t;

struct kr_videoport_St {
  int width;
  int height;
  kr_shm_t *kr_shm;
  kr_client_t *client;
  int sd;
  
  int (*callback)(void *, void *);
  void *pointer;

  int active;
  pthread_t process_thread;
};

int kr_compositor_background (kr_client_t *client, char *filename) {

  unsigned char *command;
  unsigned char *background;

  if ((filename == NULL) || (!(strlen(filename)))) {
    return 0;
  }

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_SET_BACKGROUND, &background);

  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_COMPOSITOR_FILENAME, filename);

  kr_ebml2_finish_element (client->ebml2, background);
  kr_ebml2_finish_element (client->ebml2, command);
    
  kr_client_push (client);

  return 1;
}

void kr_compositor_close_display (kr_client_t *client) {

  unsigned char *command;
  unsigned char *display;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_CLOSE_DISPLAY, &display);
  kr_ebml2_finish_element (client->ebml2, display);
  kr_ebml2_finish_element (client->ebml2, command);
    
  kr_client_push (client);
}

void kr_compositor_open_display (kr_client_t *client, int width, int height) {

  unsigned char *compositor_command;
  unsigned char *display;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &compositor_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_OPEN_DISPLAY, &display);
  kr_ebml2_finish_element (client->ebml2, display);
  kr_ebml2_finish_element (client->ebml2, compositor_command);
    
  kr_client_push (client);
}

void kr_compositor_snapshot (kr_client_t *client) {

  unsigned char *command;
  unsigned char *snap_command;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_SNAPSHOT, &snap_command);
  kr_ebml2_finish_element (client->ebml2, snap_command);
  kr_ebml2_finish_element (client->ebml2, command);
    
  kr_client_push (client);
}

void kr_compositor_snapshot_jpeg (kr_client_t *client) {

  unsigned char *command;
  unsigned char *snap_command;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_SNAPSHOT_JPEG, &snap_command);
  kr_ebml2_finish_element (client->ebml2, snap_command);
  kr_ebml2_finish_element (client->ebml2, command);
  kr_client_push (client);
}

void kr_compositor_set_frame_rate (kr_client_t *client, int numerator, int denominator) {

  unsigned char *compositor_command;
  unsigned char *set_frame_rate;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &compositor_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_SET_FRAME_RATE, &set_frame_rate);

  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_COMPOSITOR_FPS_NUMERATOR, numerator);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_COMPOSITOR_FPS_DENOMINATOR, denominator);

  kr_ebml2_finish_element (client->ebml2, set_frame_rate);
  kr_ebml2_finish_element (client->ebml2, compositor_command);
    
  kr_client_push (client);
}

void kr_compositor_set_resolution (kr_client_t *client, int width, int height) {

  unsigned char *compositor_command;
  unsigned char *set_resolution;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &compositor_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_SET_RESOLUTION, &set_resolution);

  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_COMPOSITOR_WIDTH, width);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_COMPOSITOR_HEIGHT, height);

  kr_ebml2_finish_element (client->ebml2, set_resolution);
  kr_ebml2_finish_element (client->ebml2, compositor_command);
    
  kr_client_push (client);
}

void kr_compositor_info (kr_client_t *client) {

  unsigned char *command;
  unsigned char *info_command;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_INFO, &info_command);
  kr_ebml2_finish_element (client->ebml2, info_command);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}

static void kr_ebml_to_compositor_rep (kr_ebml2_t *ebml, kr_compositor_t *kr_compositor_rep) {

  kr_ebml2_unpack_element_uint32 (ebml, NULL, &kr_compositor_rep->width);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &kr_compositor_rep->height);

  kr_ebml2_unpack_element_uint32 (ebml, NULL, &kr_compositor_rep->fps_numerator);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &kr_compositor_rep->fps_denominator);

  kr_ebml2_unpack_element_uint32 (ebml, NULL, &kr_compositor_rep->sprites);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &kr_compositor_rep->texts);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &kr_compositor_rep->vectors);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &kr_compositor_rep->inputs);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &kr_compositor_rep->outputs);
  kr_ebml2_unpack_element_uint64 (ebml, NULL, &kr_compositor_rep->frames);

  kr_ebml2_unpack_element_string (ebml, NULL, kr_compositor_rep->background_filename,
                                  sizeof(kr_compositor_rep->background_filename));
}

static void kr_ebml_to_videoport_rep (kr_ebml2_t *ebml, kr_port_t *port) {
  kr_ebml2_unpack_element_string (ebml, NULL, port->sysname, sizeof(port->sysname));
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->direction );
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->controls.x);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->controls.y);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->controls.z);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->source_width);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->source_height);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->crop_x);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->crop_y);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->crop_width);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->crop_height);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->controls.width);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->controls.height);
  kr_ebml2_unpack_element_float (ebml, NULL, &port->controls.opacity);
  kr_ebml2_unpack_element_float (ebml, NULL, &port->controls.rotation);
}

static void kr_ebml_to_sprite_rep (kr_ebml2_t *ebml, kr_sprite_t *sprite) {

  kr_ebml2_unpack_element_string (ebml, NULL, sprite->filename, sizeof(sprite->filename));

  kr_ebml2_unpack_element_uint32 (ebml, NULL, &sprite->controls.x);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &sprite->controls.y);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &sprite->controls.z);
  
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &sprite->controls.tickrate);

  kr_ebml2_unpack_element_float (ebml, NULL, &sprite->controls.xscale);
  kr_ebml2_unpack_element_float (ebml, NULL, &sprite->controls.yscale);

  kr_ebml2_unpack_element_float (ebml, NULL, &sprite->controls.opacity);
  kr_ebml2_unpack_element_float (ebml, NULL, &sprite->controls.rotation);
}

static void kr_ebml_to_vector_rep (kr_ebml2_t *ebml, kr_vector_t *vector) {

  kr_ebml2_unpack_element_uint32 (ebml, NULL, &vector->type);

  kr_ebml2_unpack_element_uint32 (ebml, NULL, &vector->controls.width);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &vector->controls.height);

  kr_ebml2_unpack_element_uint32 (ebml, NULL, &vector->controls.x);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &vector->controls.y);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &vector->controls.z);

  kr_ebml2_unpack_element_float (ebml, NULL, &vector->controls.xscale);
  kr_ebml2_unpack_element_float (ebml, NULL, &vector->controls.yscale);

  kr_ebml2_unpack_element_float (ebml, NULL, &vector->controls.opacity);
  kr_ebml2_unpack_element_float (ebml, NULL, &vector->controls.rotation);
}

static void kr_ebml_to_text_rep (kr_ebml2_t *ebml, kr_text_t *text) {
  
  kr_ebml2_unpack_element_string (ebml, NULL, text->text, sizeof(text->text));
  kr_ebml2_unpack_element_string (ebml, NULL, text->font, sizeof(text->font));

  kr_ebml2_unpack_element_float (ebml, NULL, &text->red);
  kr_ebml2_unpack_element_float (ebml, NULL, &text->green);
  kr_ebml2_unpack_element_float (ebml, NULL, &text->blue);

  kr_ebml2_unpack_element_uint32 (ebml, NULL, &text->controls.x);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &text->controls.y);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &text->controls.z);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &text->controls.width);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &text->controls.height);      

  kr_ebml2_unpack_element_float (ebml, NULL, &text->controls.xscale);
  kr_ebml2_unpack_element_float (ebml, NULL, &text->controls.opacity);
  kr_ebml2_unpack_element_float (ebml, NULL, &text->controls.rotation);
}

int kr_compositor_crate_to_string_from_compositor (kr_crate_t *crate, char **string) {

  int pos;
  kr_compositor_t kr_compositor;

  pos = 0;

  kr_ebml_to_compositor_rep (&crate->payload_ebml, &kr_compositor);
  pos += sprintf (*string + pos, "Resolution: %ux%u\n", kr_compositor.width, kr_compositor.height);
  pos += sprintf (*string + pos, "Frame Rate: %u / %u\n",
                  kr_compositor.fps_numerator, kr_compositor.fps_denominator);
  pos += sprintf (*string + pos, "Sprites: %u\n", kr_compositor.sprites);
  pos += sprintf (*string + pos, "Vectors: %u\n", kr_compositor.vectors);
  pos += sprintf (*string + pos, "Texts: %u\n", kr_compositor.texts);
  pos += sprintf (*string + pos, "Inputs: %u\n", kr_compositor.inputs);
  pos += sprintf (*string + pos, "Outputs: %u\n", kr_compositor.outputs);
  pos += sprintf (*string + pos, "Frames: %"PRIu64"\n", kr_compositor.frames);
  if (strlen(kr_compositor.background_filename)) {
    pos += sprintf (*string + pos, "Background: %s\n", kr_compositor.background_filename);
  } else {
    pos += sprintf (*string + pos, "Background: Unset\n");
  }
  return pos; 
}

int kr_compositor_crate_to_string_from_sprite (kr_crate_t *crate, char **string) {

  int pos;
  kr_sprite_t sprite;

  pos = 0;

  kr_ebml_to_sprite_rep (&crate->payload_ebml, &sprite);
  pos += sprintf (*string + pos, "Sprite: %s\n", sprite.filename);
  pos += sprintf (*string + pos, "X: %d\n", sprite.controls.x);
  pos += sprintf (*string + pos, "Y: %d\n", sprite.controls.y);
  pos += sprintf (*string + pos, "Z: %d\n", sprite.controls.z);
  pos += sprintf (*string + pos, "Y Scale: %4.2f\n", sprite.controls.yscale);
  pos += sprintf (*string + pos, "X Scale: %4.2f\n", sprite.controls.xscale);
  pos += sprintf (*string + pos, "Opacity: %4.2f\n", sprite.controls.opacity);
  pos += sprintf (*string + pos, "Rotation: %4.2f\n", sprite.controls.rotation);
  pos += sprintf (*string + pos, "Tickrate: %d\n", sprite.controls.tickrate);
  
  return pos;
}

int kr_compositor_crate_to_string_from_text (kr_crate_t *crate, char **string) {

  int pos;
  kr_text_t text;

  pos = 0;

  kr_ebml_to_text_rep (&crate->payload_ebml, &text);
  pos += sprintf (*string + pos, "Text: %s\n", text.text);
  pos += sprintf (*string + pos, "Font: %s\n", text.font);
  pos += sprintf (*string + pos, "Red: %4.2f Green: %4.2f Blue: %4.2f\n",
                  text.red, text.green, text.blue);
  pos += sprintf (*string + pos, "X: %d\n", text.controls.x);
  pos += sprintf (*string + pos, "Y: %d\n", text.controls.y);
  pos += sprintf (*string + pos, "Z: %d\n", text.controls.z);
  pos += sprintf (*string + pos, "Width: %d\n", text.controls.width);
  pos += sprintf (*string + pos, "Height: %d\n", text.controls.height);
  pos += sprintf (*string + pos, "Size: %4.2f\n", text.controls.xscale);
  pos += sprintf (*string + pos, "Opacity: %4.2f\n", text.controls.opacity);
  pos += sprintf (*string + pos, "Rotation: %4.2f\n", text.controls.rotation);
 
  return pos;
}

int kr_compositor_crate_to_string_from_vector (kr_crate_t *crate, char **string) {

  int pos;
  kr_vector_t vector;

  pos = 0;

  kr_ebml_to_vector_rep (&crate->payload_ebml, &vector);
  pos += sprintf (*string + pos, "Vector: %s\n", krad_vector_type_to_string (vector.type));
  pos += sprintf (*string + pos, "Width: %d\n", vector.controls.width);
  pos += sprintf (*string + pos, "Height: %d\n", vector.controls.height);  
  pos += sprintf (*string + pos, "X: %d\n", vector.controls.x);
  pos += sprintf (*string + pos, "Y: %d\n", vector.controls.y);
  pos += sprintf (*string + pos, "Z: %d\n", vector.controls.z);
  pos += sprintf (*string + pos, "Y Scale: %4.2f\n", vector.controls.yscale);
  pos += sprintf (*string + pos, "X Scale: %4.2f\n", vector.controls.xscale);
  pos += sprintf (*string + pos, "Opacity: %4.2f\n", vector.controls.opacity);
  pos += sprintf (*string + pos, "Rotation: %4.2f\n", vector.controls.rotation);
  
  return pos;
}

int kr_compositor_crate_to_string_from_videoport (kr_crate_t *crate, char **string) {

  int pos;
  kr_port_t port;

  pos = 0;

  kr_ebml_to_videoport_rep (&crate->payload_ebml, &port);
  
  if (port.direction == OUTPUT) {
    pos += sprintf (*string + pos, "Video Output Port: %s\n", port.sysname);
    pos += sprintf (*string + pos, "Width: %d\n", port.controls.width);
    pos += sprintf (*string + pos, "Height: %d\n", port.controls.height);
  } else {
    pos += sprintf (*string + pos, "Video Input Port: %s\n", port.sysname);
    pos += sprintf (*string + pos, "X: %d\n", port.controls.x);
    pos += sprintf (*string + pos, "Y: %d\n", port.controls.y);
    pos += sprintf (*string + pos, "Z: %d\n", port.controls.z);
    pos += sprintf (*string + pos, "Source Width: %d\n", port.source_width);
    pos += sprintf (*string + pos, "Source Height: %d\n", port.source_height);
    pos += sprintf (*string + pos, "Crop X: %d\n", port.crop_x);
    pos += sprintf (*string + pos, "Crop Y: %d\n", port.crop_y);
    pos += sprintf (*string + pos, "Crop Width: %d\n", port.crop_width);
    pos += sprintf (*string + pos, "Crop Height: %d\n", port.crop_height);
    pos += sprintf (*string + pos, "Width: %d\n", port.controls.width);
    pos += sprintf (*string + pos, "Height: %d\n", port.controls.height);
    pos += sprintf (*string + pos, "Opacity: %4.2f\n", port.controls.opacity);
    pos += sprintf (*string + pos, "Rotation: %4.2f\n", port.controls.rotation);
  }
  
  return pos; 
}

static int kr_compositor_crate_get_string_from_subunit (kr_crate_t *crate, char **string) {

  switch ( crate->address.path.subunit.compositor_subunit ) {
    case KR_SPRITE:
      return kr_compositor_crate_to_string_from_sprite (crate, string);
    case KR_TEXT:
      return kr_compositor_crate_to_string_from_text (crate, string);
    case KR_VECTOR:
      return kr_compositor_crate_to_string_from_vector (crate, string);
    case KR_VIDEOPORT:
      return kr_compositor_crate_to_string_from_videoport (crate, string);
  }
  
  return 0; 
}

int kr_compositor_crate_to_rep (kr_crate_t *crate) {
  if ((crate->address.path.unit == KR_COMPOSITOR) && (crate->address.path.subunit.zero == KR_UNIT) && 
      (crate->notice == EBML_ID_KRAD_UNIT_INFO)) {
    crate->contains = KR_COMPOSITOR;
    kr_ebml_to_compositor_rep (&crate->payload_ebml, &crate->rep.compositor);
    return 1;
  }
  if ((crate->address.path.unit == KR_COMPOSITOR) && (crate->address.path.subunit.zero == KR_SPRITE) && 
      (crate->notice == EBML_ID_KRAD_SUBUNIT_INFO)) {
    crate->contains = KR_SPRITE;
    kr_ebml_to_sprite_rep (&crate->payload_ebml, &crate->rep.sprite);
    return 1;
  }
  if ((crate->address.path.unit == KR_COMPOSITOR) && (crate->address.path.subunit.zero == KR_TEXT) && 
      (crate->notice == EBML_ID_KRAD_SUBUNIT_INFO)) {
    crate->contains = KR_TEXT;
    kr_ebml_to_text_rep (&crate->payload_ebml, &crate->rep.text);
    return 1;
  }
  if ((crate->address.path.unit == KR_COMPOSITOR) && (crate->address.path.subunit.zero == KR_VECTOR) && 
      (crate->notice == EBML_ID_KRAD_SUBUNIT_INFO)) {
    crate->contains = KR_VECTOR;
    kr_ebml_to_vector_rep (&crate->payload_ebml, &crate->rep.vector);
    return 1;
  }
  if ((crate->address.path.unit == KR_COMPOSITOR) && (crate->address.path.subunit.zero == KR_VIDEOPORT) && 
      (crate->notice == EBML_ID_KRAD_SUBUNIT_INFO)) {
    crate->contains = KR_VIDEOPORT;
    kr_ebml_to_videoport_rep (&crate->payload_ebml, &crate->rep.videoport);
    return 1;
  }
  return 0;
}

int kr_compositor_crate_to_string (kr_crate_t *crate, char **string) {

  switch ( crate->notice ) {
    case EBML_ID_KRAD_UNIT_INFO:
      *string = kr_response_alloc_string (crate->size * 4);
      return kr_compositor_crate_to_string_from_compositor (crate, string);
    case EBML_ID_KRAD_SUBUNIT_INFO:
      *string = kr_response_alloc_string (crate->size * 6);
      return kr_compositor_crate_get_string_from_subunit (crate, string);
    case EBML_ID_KRAD_SUBUNIT_CREATED:
      *string = kr_response_alloc_string (crate->size * 4);
      return kr_compositor_crate_get_string_from_subunit (crate, string);
  }
  return 0;
}

int kr_compositor_subunit_create (kr_client_t *client,
                                  kr_compositor_subunit_t type,
                                  char *option,
                                  char *option2) {
  unsigned char *command;
  unsigned char *subunit;
  
  command = 0;
  subunit = 0;

  switch ( type ) {
    case KR_SPRITE:
    case KR_TEXT:
    case KR_VECTOR:
    case KR_VIDEOPORT:
      if ((option != NULL) && (strlen(option))) {
        break;
      }
    default:
      return 0;
  }

  if (option2 == NULL) {
    option2 = "";
  }

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_ADD_SUBUNIT, &subunit);

  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_SUBUNIT, type);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_SUBUNIT, option);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_SUBUNIT, option2);

  kr_ebml2_finish_element (client->ebml2, subunit);
  kr_ebml2_finish_element (client->ebml2, command);
    
  kr_client_push (client);
  
  return 1;
}

void kr_compositor_subunit_list (kr_client_t *client) {

  unsigned char *command;
  unsigned char *list;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_LIST_SUBUNITS, &list);
  kr_ebml2_finish_element (client->ebml2, list);
  kr_ebml2_finish_element (client->ebml2, command);
    
  kr_client_push (client);
}

void kr_compositor_subunit_destroy (kr_client_t *client, kr_address_t *address) {

  unsigned char *command;
  unsigned char *destroy;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_REMOVE_SUBUNIT, &destroy);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_SUBUNIT, address->path.subunit.compositor_subunit);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_SUBUNIT, address->id.number);
  kr_ebml2_finish_element (client->ebml2, destroy);
  kr_ebml2_finish_element (client->ebml2, command);
    
  kr_client_push (client);
}

void kr_videoport_destroy_cmd (kr_client_t *client) {

  unsigned char *compositor_command;
  unsigned char *destroy_videoport;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &compositor_command);
  kr_ebml2_start_element (client->ebml2,
                          EBML_ID_KRAD_COMPOSITOR_CMD_LOCAL_VIDEOPORT_DESTROY,
                          &destroy_videoport);
  kr_ebml2_finish_element (client->ebml2, destroy_videoport);
  kr_ebml2_finish_element (client->ebml2, compositor_command);
    
  kr_client_push (client);
}

void kr_videoport_create_cmd (kr_client_t *client) {

  unsigned char *compositor_command;
  unsigned char *create_videoport;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &compositor_command);
  kr_ebml2_start_element (client->ebml2,
                          EBML_ID_KRAD_COMPOSITOR_CMD_LOCAL_VIDEOPORT_CREATE,
                          &create_videoport);
  kr_ebml2_finish_element (client->ebml2, create_videoport);
  kr_ebml2_finish_element (client->ebml2, compositor_command);
    
  kr_client_push (client);
}

void kr_videoport_set_callback (kr_videoport_t *kr_videoport, int callback (void *, void *), void *pointer) {
  kr_videoport->callback = callback;
  kr_videoport->pointer = pointer;
}

void *kr_videoport_process_thread (void *arg) {

  kr_videoport_t *kr_videoport = (kr_videoport_t *)arg;
  int ret;
  char buf[1];

  krad_system_set_thread_name ("krc_videoport");

  while (kr_videoport->active == 1) {
  
    // wait for socket to have a byte
    ret = read (kr_videoport->sd, buf, 1);
    if (ret != 1) {
      printke ("compositor client: unexpected read return value %d in kr_videoport_process_thread", ret);
    }
    kr_videoport->callback (kr_videoport->kr_shm->buffer, kr_videoport->pointer);

    // write a byte to socket
    ret = write (kr_videoport->sd, buf, 1);
    if (ret != 1) {
      printke ("compositor client: unexpected write return value %d in kr_videoport_process_thread", ret);
    }

  }

  return NULL;
}

void kr_videoport_activate (kr_videoport_t *kr_videoport) {
  if ((kr_videoport->active == 0) && (kr_videoport->callback != NULL)) {
    pthread_create (&kr_videoport->process_thread, NULL, kr_videoport_process_thread, (void *)kr_videoport);
    kr_videoport->active = 1;
  }
}

void kr_videoport_deactivate (kr_videoport_t *kr_videoport) {

  if (kr_videoport->active == 1) {
    kr_videoport->active = 2;
    pthread_join (kr_videoport->process_thread, NULL);
    kr_videoport->active = 0;
  }
}

kr_videoport_t *kr_videoport_create (kr_client_t *client) {

  kr_videoport_t *kr_videoport;
  int sockets[2];

  if (!kr_client_local (client)) {
    // Local clients only
    return NULL;
  }

  kr_videoport = calloc (1, sizeof(kr_videoport_t));

  if (kr_videoport == NULL) {
    return NULL;
  }

  kr_videoport->client = client;

  kr_videoport->kr_shm = kr_shm_create (kr_videoport->client);

  sprintf (kr_videoport->kr_shm->buffer, "waa hoo its yaytime");

  if (kr_videoport->kr_shm == NULL) {
    free (kr_videoport);
    return NULL;
  }

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
    kr_shm_destroy (kr_videoport->kr_shm);
    free (kr_videoport);
    return NULL;
    }

  kr_videoport->sd = sockets[0];
  
  printf ("sockets %d and %d\n", sockets[0], sockets[1]);
  
  kr_videoport_create_cmd (kr_videoport->client);
  //FIXME use a return message from daemon to indicate ready to receive fds
  usleep (33000);
  kr_send_fd (kr_videoport->client, kr_videoport->kr_shm->fd);
  usleep (33000);
  kr_send_fd (kr_videoport->client, sockets[1]);
  usleep (33000);

  return kr_videoport;
}

void kr_videoport_destroy (kr_videoport_t *kr_videoport) {

  if (kr_videoport->active == 1) {
    kr_videoport_deactivate (kr_videoport);
  }

  kr_videoport_destroy_cmd (kr_videoport->client);

  if (kr_videoport != NULL) {
    if (kr_videoport->sd != 0) {
      close (kr_videoport->sd);
      kr_videoport->sd = 0;
    }
    if (kr_videoport->kr_shm != NULL) {
      kr_shm_destroy (kr_videoport->kr_shm);
      kr_videoport->kr_shm = NULL;
    }
    free(kr_videoport);
  }
}

