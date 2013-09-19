#include "krad_radio_client.h"
#include "krad_radio_client_internal.h"
#include "krad_compositor_common.h"

static void kr_ebml_to_compositor_info(kr_ebml2_t *ebml, kr_compositor_info *comp);
static int kr_compositor_crate_get_string_from_subunit(kr_crate_t *crate, char **string);

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
  int error;
  pthread_t process_thread;
};

void kr_compositor_set_frame_rate(kr_client_t *client, int numerator, int denominator) {

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

int kr_compositor_set_resolution (kr_client_t *client, uint32_t width, uint32_t height) {

  unsigned char *compositor_command;
  unsigned char *set_resolution;

  if ((width == 0) || (height == 0) || (width > 8192) || (height > 8192)) {
    return -1;
  }

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &compositor_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_SET_RESOLUTION, &set_resolution);

  kr_ebml2_pack_uint32 (client->ebml2, EBML_ID_KRAD_COMPOSITOR_WIDTH, width);
  kr_ebml2_pack_uint32 (client->ebml2, EBML_ID_KRAD_COMPOSITOR_HEIGHT, height);

  kr_ebml2_finish_element (client->ebml2, set_resolution);
  kr_ebml2_finish_element (client->ebml2, compositor_command);

  kr_client_push (client);

  return 0;
}

void kr_compositor_info_get(kr_client_t *client) {

  unsigned char *command;
  unsigned char *info_command;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_INFO, &info_command);
  kr_ebml2_finish_element (client->ebml2, info_command);
  kr_ebml2_finish_element (client->ebml2, command);

  kr_client_push (client);
}

static void kr_ebml_to_compositor_info(kr_ebml2_t *ebml, kr_compositor_info *comp) {

  kr_ebml2_unpack_element_uint32 (ebml, NULL, &comp->width);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &comp->height);

  kr_ebml2_unpack_element_uint32 (ebml, NULL, &comp->fps_numerator);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &comp->fps_denominator);

  kr_ebml2_unpack_element_uint32 (ebml, NULL, &comp->sprites);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &comp->texts);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &comp->vectors);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &comp->inputs);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &comp->outputs);
  kr_ebml2_unpack_element_uint64 (ebml, NULL, &comp->frames);

  kr_ebml2_unpack_element_string (ebml, NULL,
                                  comp->background_filename,
                                  sizeof(comp->background_filename));
}

static void kr_ebml_to_videoport_info(kr_ebml2_t *ebml, kr_compositor_path_info *port) {
  kr_ebml2_unpack_element_string(ebml, NULL, port->name, sizeof(port->name));
  kr_ebml2_unpack_element_int32(ebml, NULL, &port->type);
  kr_ebml2_unpack_element_int32(ebml, NULL, &port->controls.x);
  kr_ebml2_unpack_element_int32(ebml, NULL, &port->controls.y);
  kr_ebml2_unpack_element_uint32(ebml, NULL, &port->controls.z);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->width);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->height);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->crop_x);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->crop_y);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->crop_width);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->crop_height);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->view.top_left.x);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->view.top_left.y);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->view.top_right.x);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->view.top_right.y);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->view.bottom_left.x);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->view.bottom_left.y);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->view.bottom_right.x);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->view.bottom_right.y);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->controls.width);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &port->controls.height);
  kr_ebml2_unpack_element_float (ebml, NULL, &port->controls.opacity);
  kr_ebml2_unpack_element_float (ebml, NULL, &port->controls.rotation);
}

static void kr_ebml_to_sprite_info(kr_ebml2_t *ebml, kr_sprite_info *sprite) {

  kr_ebml2_unpack_element_string (ebml, NULL,
                                  sprite->filename, sizeof(sprite->filename));

  kr_ebml2_unpack_element_int32 (ebml, NULL, &sprite->controls.x);
  kr_ebml2_unpack_element_int32 (ebml, NULL, &sprite->controls.y);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &sprite->controls.z);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &sprite->controls.width);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &sprite->controls.height);
  kr_ebml2_unpack_element_int32 (ebml, NULL, &sprite->controls.tickrate);
  kr_ebml2_unpack_element_float (ebml, NULL, &sprite->controls.opacity);
  kr_ebml2_unpack_element_float (ebml, NULL, &sprite->controls.rotation);
}

static void kr_ebml_to_vector_info(kr_ebml2_t *ebml, kr_vector_info *vector) {

  kr_ebml2_unpack_element_uint32 (ebml, NULL, &vector->type);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &vector->controls.width);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &vector->controls.height);
  kr_ebml2_unpack_element_int32 (ebml, NULL, &vector->controls.x);
  kr_ebml2_unpack_element_int32 (ebml, NULL, &vector->controls.y);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &vector->controls.z);
  kr_ebml2_unpack_element_float (ebml, NULL, &vector->controls.opacity);
  kr_ebml2_unpack_element_float (ebml, NULL, &vector->controls.rotation);
}

static void kr_ebml_to_text_info(kr_ebml2_t *ebml, kr_text_info *text) {

  kr_ebml2_unpack_element_string (ebml, NULL, text->text, sizeof(text->text));
  kr_ebml2_unpack_element_string (ebml, NULL, text->font, sizeof(text->font));

  kr_ebml2_unpack_element_float (ebml, NULL, &text->red);
  kr_ebml2_unpack_element_float (ebml, NULL, &text->green);
  kr_ebml2_unpack_element_float (ebml, NULL, &text->blue);

  kr_ebml2_unpack_element_int32 (ebml, NULL, &text->controls.x);
  kr_ebml2_unpack_element_int32 (ebml, NULL, &text->controls.y);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &text->controls.z);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &text->controls.width);
  kr_ebml2_unpack_element_uint32 (ebml, NULL, &text->controls.height);
  kr_ebml2_unpack_element_float (ebml, NULL, &text->controls.opacity);
  kr_ebml2_unpack_element_float (ebml, NULL, &text->controls.rotation);
}

int kr_compositor_crate_to_string_from_compositor (kr_crate_t *crate, char **string) {

  int pos;
  kr_compositor_info kr_compositor;

  pos = 0;

  kr_ebml_to_compositor_info (&crate->payload_ebml, &kr_compositor);
  pos += sprintf (*string + pos, "Compositor Status:\n");
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
    pos += sprintf (*string + pos, "Background: %s", kr_compositor.background_filename);
  } else {
    pos += sprintf (*string + pos, "Background: Unset");
  }
  return pos;
}

int kr_compositor_crate_to_string_from_sprite (kr_crate_t *crate, char **string) {

  int pos;
  kr_sprite_info sprite;

  pos = 0;

  kr_ebml_to_sprite_info (&crate->payload_ebml, &sprite);
  pos += sprintf (*string + pos, "Sprite: %s\n", sprite.filename);
  pos += sprintf (*string + pos, "X: %d\n", sprite.controls.x);
  pos += sprintf (*string + pos, "Y: %d\n", sprite.controls.y);
  pos += sprintf (*string + pos, "Z: %d\n", sprite.controls.z);
  pos += sprintf (*string + pos, "Opacity: %4.2f\n", sprite.controls.opacity);
  pos += sprintf (*string + pos, "Rotation: %4.2f\n", sprite.controls.rotation);
  pos += sprintf (*string + pos, "Tickrate: %d\n", sprite.controls.tickrate);

  return pos;
}

int kr_compositor_crate_to_string_from_text (kr_crate_t *crate, char **string) {

  int pos;
  kr_text_info text;

  pos = 0;

  kr_ebml_to_text_info (&crate->payload_ebml, &text);
  pos += sprintf (*string + pos, "Text: %s\n", text.text);
  pos += sprintf (*string + pos, "Font: %s\n", text.font);
  pos += sprintf (*string + pos, "Red: %4.2f Green: %4.2f Blue: %4.2f\n",
                  text.red, text.green, text.blue);
  pos += sprintf (*string + pos, "X: %d\n", text.controls.x);
  pos += sprintf (*string + pos, "Y: %d\n", text.controls.y);
  pos += sprintf (*string + pos, "Z: %d\n", text.controls.z);
  pos += sprintf (*string + pos, "Width: %d\n", text.controls.width);
  pos += sprintf (*string + pos, "Height: %d\n", text.controls.height);
  pos += sprintf (*string + pos, "Opacity: %4.2f\n", text.controls.opacity);
  pos += sprintf (*string + pos, "Rotation: %4.2f\n", text.controls.rotation);

  return pos;
}

int kr_compositor_crate_to_string_from_vector (kr_crate_t *crate, char **string) {

  int pos;
  kr_vector_info vector;

  pos = 0;

  kr_ebml_to_vector_info (&crate->payload_ebml, &vector);
  pos += sprintf (*string + pos, "Vector: %s\n", krad_vector_type_to_string (vector.type));
  pos += sprintf (*string + pos, "Width: %d\n", vector.controls.width);
  pos += sprintf (*string + pos, "Height: %d\n", vector.controls.height);
  pos += sprintf (*string + pos, "X: %d\n", vector.controls.x);
  pos += sprintf (*string + pos, "Y: %d\n", vector.controls.y);
  pos += sprintf (*string + pos, "Z: %d\n", vector.controls.z);
  pos += sprintf (*string + pos, "Opacity: %4.2f\n", vector.controls.opacity);
  pos += sprintf (*string + pos, "Rotation: %4.2f\n", vector.controls.rotation);

  return pos;
}

int kr_compositor_crate_to_string_from_videoport (kr_crate_t *crate, char **string) {

  int pos;
  kr_compositor_path_info port;

  pos = 0;

  kr_ebml_to_videoport_info (&crate->payload_ebml, &port);
  if (port.type == KR_CMP_OUTPUT) {
    pos += sprintf (*string + pos, "Video Output Port: %s\n", port.name);
    pos += sprintf (*string + pos, "Width: %d\n", port.controls.width);
    pos += sprintf (*string + pos, "Height: %d\n", port.controls.height);
  } else {
    pos += sprintf (*string + pos, "Video Input Port: %s\n", port.name);
    pos += sprintf (*string + pos, "X: %d\n", port.controls.x);
    pos += sprintf (*string + pos, "Y: %d\n", port.controls.y);
    pos += sprintf (*string + pos, "Z: %d\n", port.controls.z);
    pos += sprintf (*string + pos, "Source Width: %d\n", port.width);
    pos += sprintf (*string + pos, "Source Height: %d\n", port.height);
    pos += sprintf (*string + pos, "Crop X: %d\n", port.crop_x);
    pos += sprintf (*string + pos, "Crop Y: %d\n", port.crop_y);
    pos += sprintf (*string + pos, "Crop Width: %d\n", port.crop_width);
    pos += sprintf (*string + pos, "Crop Height: %d\n", port.crop_height);

    pos += sprintf (*string + pos, "PC View: %u,%u, %u,%u\n",
                    port.view.top_left.x, port.view.top_left.y,
                    port.view.top_right.x, port.view.top_right.y);
    pos += sprintf (*string + pos, "         %u,%u, %u,%u\n",
                    port.view.bottom_left.x, port.view.bottom_left.y,
                    port.view.bottom_right.x, port.view.bottom_right.y);

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

int kr_compositor_crate_to_info (kr_crate_t *crate) {
  if ((crate->address.path.unit == KR_COMPOSITOR) && (crate->address.path.subunit.zero == KR_UNIT) &&
      (crate->notice == EBML_ID_KRAD_UNIT_INFO)) {
    crate->contains = KR_COMPOSITOR;
    kr_ebml_to_compositor_info (&crate->payload_ebml, &crate->rep.compositor);

    crate->client->width = crate->rep.compositor.width;
    crate->client->height = crate->rep.compositor.height;
    crate->client->fps_num = crate->rep.compositor.fps_numerator;
    crate->client->fps_den = crate->rep.compositor.fps_denominator;

    return 1;
  }
  if ((crate->address.path.unit == KR_COMPOSITOR) && (crate->address.path.subunit.zero == KR_SPRITE) &&
      ((crate->notice == EBML_ID_KRAD_SUBUNIT_CREATED) || (crate->notice == EBML_ID_KRAD_SUBUNIT_INFO))) {
    crate->contains = KR_SPRITE;
    kr_ebml_to_sprite_info (&crate->payload_ebml, &crate->rep.sprite);
    return 1;
  }
  if ((crate->address.path.unit == KR_COMPOSITOR) && (crate->address.path.subunit.zero == KR_TEXT) &&
      ((crate->notice == EBML_ID_KRAD_SUBUNIT_CREATED) || (crate->notice == EBML_ID_KRAD_SUBUNIT_INFO))) {
    crate->contains = KR_TEXT;
    kr_ebml_to_text_info (&crate->payload_ebml, &crate->rep.text);
    return 1;
  }
  if ((crate->address.path.unit == KR_COMPOSITOR) && (crate->address.path.subunit.zero == KR_VECTOR) &&
      ((crate->notice == EBML_ID_KRAD_SUBUNIT_CREATED) || (crate->notice == EBML_ID_KRAD_SUBUNIT_INFO))) {
    crate->contains = KR_VECTOR;
    kr_ebml_to_vector_info (&crate->payload_ebml, &crate->rep.vector);
    return 1;
  }
  if ((crate->address.path.unit == KR_COMPOSITOR) && (crate->address.path.subunit.zero == KR_VIDEOPORT) &&
      ((crate->notice == EBML_ID_KRAD_SUBUNIT_CREATED) || (crate->notice == EBML_ID_KRAD_SUBUNIT_INFO))) {
    crate->contains = KR_VIDEOPORT;
    kr_ebml_to_videoport_info (&crate->payload_ebml, &crate->rep.videoport);
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

void kr_compositor_subunit_info (kr_client_t *client, kr_address_t *address) {

  unsigned char *command;
  unsigned char *getinfo;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_SUBUNIT_INFO, &getinfo);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_SUBUNIT, address->path.subunit.compositor_subunit);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_SUBUNIT, address->id.number);
  kr_ebml2_finish_element (client->ebml2, getinfo);
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

void kr_videoport_create_cmd (kr_client_t *client, int32_t type) {

  unsigned char *compositor_command;
  unsigned char *create_videoport;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &compositor_command);
  kr_ebml2_start_element (client->ebml2,
                          EBML_ID_KRAD_COMPOSITOR_CMD_LOCAL_VIDEOPORT_CREATE,
                          &create_videoport);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_COMPOSITOR_PORT_DIRECTION, type);
  kr_ebml2_finish_element (client->ebml2, create_videoport);
  kr_ebml2_finish_element (client->ebml2, compositor_command);

  kr_client_push (client);
}

void kr_videoport_set_callback (kr_videoport_t *kr_videoport, int callback (void *, void *), void *pointer) {
  kr_videoport->callback = callback;
  kr_videoport->pointer = pointer;
}

void *kr_videoport_process_thread (void *arg) {

  kr_videoport_t *videoport = (kr_videoport_t *)arg;
  int ret;
  char buf[1];
  int max_timeout_ms;
  int timeout_total_ms;
  int timeout_ms;
  struct pollfd pollfds[1];

  timeout_ms = 45;
  timeout_total_ms = 0;
  max_timeout_ms = 1000;

  pollfds[0].fd = videoport->sd;

  krad_system_set_thread_name ("krc_videoport");

  while (videoport->active == 1) {

    pollfds[0].events = POLLIN;
    ret = poll (pollfds, 1, timeout_ms);

    if (ret < 0) {
      printke ("krad compositor client: poll failure %d", ret);
      break;
    }

    if (ret == 0) {
      if (videoport->active == 1) {
        printke ("krad compositor client: videoport poll read timeout", ret);
      }
      timeout_total_ms += timeout_ms;
      if (timeout_total_ms > max_timeout_ms) {
        break;
      }
      continue;
    } else {
      timeout_total_ms = 0;
    }

    if (pollfds[0].revents & POLLHUP) {
      printke ("krad compositor client: videoport poll hangup", ret);
      break;
    }
    if (pollfds[0].revents & POLLERR) {
      printke ("krad compositor client: videoport poll error", ret);
      break;
    }
    if (!(pollfds[0].revents & POLLIN)) {
      printke ("krad compositor client: could not polling", ret);
      break;
    }
    ret = read (videoport->sd, buf, 1);
    if (ret != 1) {
      printke ("krad compositor client: unexpected read return value %d in kr_compositor_process_thread", ret);
      break;
    }

    videoport->callback (videoport->kr_shm->buffer, videoport->pointer);

    pollfds[0].events = POLLOUT;
    ret = poll (pollfds, 1, timeout_ms);

    if (ret == 0) {
      printke ("krad compositor client: videoport poll write timeout", ret);
      break;
    }

    if (pollfds[0].revents & POLLHUP) {
      printke ("krad compositor client: videoport poll hangup", ret);
      break;
    }
    if (pollfds[0].revents & POLLERR) {
      printke ("krad compositor client: videoport poll error", ret);
      break;
    }

    ret = write (videoport->sd, buf, 1);
    if (ret != 1) {
      printke ("krad videoport client: unexpected write return value %d in kr_videoport_process_thread", ret);
      break;
    }
  }

  if (videoport->active == 1) {
    videoport->error = 1;
  }

  return NULL;
}

void kr_videoport_activate (kr_videoport_t *videoport) {
  if ((videoport->active == 0) && (videoport->callback != NULL)) {
    videoport->active = 1;
    pthread_create (&videoport->process_thread, NULL, kr_videoport_process_thread, (void *)videoport);
  }
}

void kr_videoport_deactivate (kr_videoport_t *videoport) {
  if (videoport->active == 1) {
    videoport->active = 2;
    pthread_join (videoport->process_thread, NULL);
    videoport->error = 0;
    videoport->active = 0;
  }
}

int kr_videoport_error (kr_videoport_t *videoport) {
  if (videoport != NULL) {
    return videoport->error;
  }
  return -1;
}

kr_videoport_t *kr_videoport_create (kr_client_t *client, int32_t type) {

  kr_videoport_t *videoport;
  int sockets[2];

  if (!kr_client_local (client)) {
    // Local clients only
    return NULL;
  }

  videoport = calloc (1, sizeof(kr_videoport_t));

  if (videoport == NULL) {
    return NULL;
  }

  videoport->client = client;

  videoport->kr_shm = kr_shm_create (videoport->client);

  sprintf (videoport->kr_shm->buffer, "waa hoo its yaytime");

  if (videoport->kr_shm == NULL) {
    free (videoport);
    return NULL;
  }

  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
    kr_shm_destroy (videoport->kr_shm);
    free (videoport);
    return NULL;
  }

  videoport->sd = sockets[0];

  //printf ("sockets %d and %d\n", sockets[0], sockets[1]);
  krad_system_set_socket_nonblocking (videoport->sd);

  krad_system_set_socket_blocking (videoport->client->krad_app_client->sd);
  kr_videoport_create_cmd (videoport->client, type);
  usleep (5000);
  kr_send_fd (videoport->client, videoport->kr_shm->fd);
  kr_send_fd (videoport->client, sockets[1]);
  krad_system_set_socket_nonblocking (videoport->client->krad_app_client->sd);

  return videoport;
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

int kr_compositor_get_info_wait(kr_client_t *client,
                            uint32_t *width,
                            uint32_t *height,
                            uint32_t *fps_num,
                            uint32_t *fps_den) {

  int wait_ms;
  int ret;
  kr_crate_t *crate;

  ret = 0;
  crate = NULL;
  wait_ms = 750;

  kr_compositor_info_get(client);

  while (kr_delivery_get_until_final (client, &crate, wait_ms)) {
    if (crate != NULL) {
      if (kr_crate_loaded (crate)) {
        if (kr_crate_addr_path_match(crate, KR_COMPOSITOR, KR_UNIT)) {
          if (width != NULL) {
            *width = crate->inside.compositor->width;
          }
          if (height != NULL) {
            *height = crate->inside.compositor->height;
          }
          if (fps_den != NULL) {
            *fps_den = crate->inside.compositor->fps_denominator;
          }
          if (fps_num != NULL) {
            *fps_num = crate->inside.compositor->fps_numerator;
          }
          ret = 1;
        }
      }
      kr_crate_recycle (&crate);
    }
  }

  return ret;
}

