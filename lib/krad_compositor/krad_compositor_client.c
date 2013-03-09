#include "krad_radio_client.h"
#include "krad_radio_client_internal.h"
#include "krad_compositor_common.h"

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
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }

  return 1;
}

void kr_compositor_close_display (kr_client_t *client) {

  unsigned char *command;
  unsigned char *display;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_CLOSE_DISPLAY, &display);
  kr_ebml2_finish_element (client->ebml2, display);
  kr_ebml2_finish_element (client->ebml2, command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_compositor_open_display (kr_client_t *client, int width, int height) {

  unsigned char *compositor_command;
  unsigned char *display;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &compositor_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_OPEN_DISPLAY, &display);
  kr_ebml2_finish_element (client->ebml2, display);
  kr_ebml2_finish_element (client->ebml2, compositor_command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_compositor_snapshot (kr_client_t *client) {

  unsigned char *command;
  unsigned char *snap_command;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_SNAPSHOT, &snap_command);
  kr_ebml2_finish_element (client->ebml2, snap_command);
  kr_ebml2_finish_element (client->ebml2, command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_compositor_snapshot_jpeg (kr_client_t *client) {

  unsigned char *command;
  unsigned char *snap_command;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_SNAPSHOT_JPEG, &snap_command);
  kr_ebml2_finish_element (client->ebml2, snap_command);
  kr_ebml2_finish_element (client->ebml2, command);
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
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
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
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
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_compositor_info (kr_client_t *client) {

  unsigned char *command;
  unsigned char *info_command;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_INFO, &info_command);
  kr_ebml2_finish_element (client->ebml2, info_command);
  kr_ebml2_finish_element (client->ebml2, command);

  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_ebml_to_compositor_rep (unsigned char *ebml_frag, kr_compositor_t *kr_compositor_rep) {

  uint32_t ebml_id;
  uint64_t ebml_data_size;
  int item_pos;

  item_pos = 0;
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  kr_compositor_rep->width = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  kr_compositor_rep->height = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  kr_compositor_rep->fps_numerator = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  kr_compositor_rep->fps_denominator = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  kr_compositor_rep->sprites = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  kr_compositor_rep->texts = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  kr_compositor_rep->vectors = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  kr_compositor_rep->inputs = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  kr_compositor_rep->outputs = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  kr_compositor_rep->frames = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, kr_compositor_rep->background_filename);
}

void kr_ebml_to_comp_controls (unsigned char *ebml_frag, kr_comp_controls_t *controls) {

  uint32_t ebml_id;
  uint64_t ebml_data_size;
  int item_pos;

  item_pos = 0;
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  controls->number = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  controls->x = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  controls->y = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  controls->z = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  controls->tickrate = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  controls->xscale = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
 
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  controls->yscale = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  controls->opacity = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  controls->rotation = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
}

void kr_ebml_to_videoport_rep (unsigned char *ebml_frag, kr_port_t *port) {

  uint32_t ebml_id;
  uint64_t ebml_data_size;
  int item_pos;

  item_pos = 0;

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, port->sysname);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  port->direction = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  port->controls.x = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  port->controls.y = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  port->controls.z = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  port->source_width = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  port->source_height = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  port->crop_x = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  port->crop_y = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  port->crop_width = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  port->crop_height = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  port->controls.width = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  port->controls.height = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  port->controls.opacity = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  port->controls.rotation = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
}

void kr_ebml_to_sprite_rep (unsigned char *ebml_frag, kr_sprite_t *sprite) {

  uint32_t ebml_id;
  uint64_t ebml_data_size;
  int item_pos;

  item_pos = 0;
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, sprite->filename);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  sprite->controls.x = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  sprite->controls.y = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  sprite->controls.z = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  sprite->controls.tickrate = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  sprite->controls.xscale = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
 
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  sprite->controls.yscale = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  sprite->controls.opacity = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  sprite->controls.rotation = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
}

void kr_ebml_to_vector_rep (unsigned char *ebml_frag, kr_vector_t *vector) {

  uint32_t ebml_id;
  uint64_t ebml_data_size;
  int item_pos;

  item_pos = 0;

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  vector->type = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  vector->controls.width = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  vector->controls.height = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  vector->controls.x = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  vector->controls.y = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  vector->controls.z = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  vector->controls.xscale = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
 
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  vector->controls.yscale = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  vector->controls.opacity = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  vector->controls.rotation = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
}

void kr_ebml_to_text_rep (unsigned char *ebml_frag, kr_text_t *text) {
  
  uint32_t ebml_id;
  uint64_t ebml_data_size;
  int item_pos;

  item_pos = 0;
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, text->text);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, text->font);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  text->red = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  text->green = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  text->blue = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  text->controls.x = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  text->controls.y = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  text->controls.z = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  text->controls.width = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  text->controls.height = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  text->controls.xscale = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  text->controls.opacity = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  text->controls.rotation = krad_ebml_read_float_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
}

int kr_compositor_response_get_string_from_compositor (unsigned char *ebml_frag, char **string) {

  int pos;
  kr_compositor_t kr_compositor;

  pos = 0;
  //kr_compositor = NULL;

  kr_ebml_to_compositor_rep (ebml_frag, &kr_compositor);
  pos += sprintf (*string + pos, "Resolution: %ux%u\n", kr_compositor.width, kr_compositor.height);
  pos += sprintf (*string + pos, "Frame Rate: %u / %u\n", kr_compositor.fps_numerator, kr_compositor.fps_denominator);
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

int kr_compositor_response_get_string_from_subunit_controls (kr_comp_controls_t *controls, char *string) {

  int pos;

  pos = 0;

  pos += sprintf (string + pos, "X: %d\n", controls->x);
  pos += sprintf (string + pos, "Y: %d\n", controls->y);
  pos += sprintf (string + pos, "Z: %d\n", controls->z);

  pos += sprintf (string + pos, "Y Scale: %f\n", controls->yscale);
  pos += sprintf (string + pos, "X Scale: %f\n", controls->xscale);

  pos += sprintf (string + pos, "Opacity: %f\n", controls->opacity);
  pos += sprintf (string + pos, "Rotation: %f\n", controls->rotation);
  pos += sprintf (string + pos, "Tickrate: %d\n", controls->tickrate);
  
  return pos;
}

int kr_compositor_response_get_string_from_sprite (unsigned char *ebml_frag, char **string) {

  int pos;
  kr_sprite_t sprite;

  pos = 0;
  //memset (&sprite, 0, sizeof(kr_sprite_t));

  kr_ebml_to_sprite_rep (ebml_frag, &sprite);
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

int kr_compositor_response_get_string_from_text (unsigned char *ebml_frag, char **string) {

  int pos;
  kr_text_t text;

  pos = 0;
  //memset (&text, 0, sizeof(kr_text_t));

  kr_ebml_to_text_rep (ebml_frag, &text);
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

int kr_compositor_response_get_string_from_vector (unsigned char *ebml_frag, char **string) {

  int pos;
  kr_vector_t vector;

  pos = 0;
  //memset (&vector, 0, sizeof(kr_vector_t));

  kr_ebml_to_vector_rep (ebml_frag, &vector);
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

int kr_compositor_response_get_string_from_videoport (unsigned char *ebml_frag, char **string) {

  int pos;
  kr_port_t port;

  pos = 0;

  kr_ebml_to_videoport_rep (ebml_frag, &port);
  
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

int kr_compositor_response_get_string_from_subunit (kr_response_t *kr_response, unsigned char *ebml_frag, char **string) {

  switch ( kr_response->address.path.subunit.compositor_subunit ) {
    case KR_SPRITE:
      return kr_compositor_response_get_string_from_sprite (ebml_frag, string);
    case KR_TEXT:
      return kr_compositor_response_get_string_from_text (ebml_frag, string);
    case KR_VECTOR:
      return kr_compositor_response_get_string_from_vector (ebml_frag, string);
    case KR_VIDEOPORT:
      return kr_compositor_response_get_string_from_videoport (ebml_frag, string);
  }
  
  return 0; 
}

int kr_compositor_response_to_string (kr_response_t *kr_response, char **string) {

  switch ( kr_response->notice ) {
    case EBML_ID_KRAD_UNIT_INFO:
      *string = kr_response_alloc_string (kr_response->size * 4);
      return kr_compositor_response_get_string_from_compositor (kr_response->buffer, string);
    case EBML_ID_KRAD_SUBUNIT_INFO:
      *string = kr_response_alloc_string (kr_response->size * 6);
      return kr_compositor_response_get_string_from_subunit (kr_response, kr_response->buffer, string);
    case EBML_ID_KRAD_SUBUNIT_CREATED:
      *string = kr_response_alloc_string (kr_response->size * 4);
      return kr_compositor_response_get_string_from_subunit (kr_response, kr_response->buffer, string);
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
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
  
  return 1;
}

void kr_compositor_subunit_list (kr_client_t *client) {

  unsigned char *command;
  unsigned char *list;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_LIST_SUBUNITS, &list);
  kr_ebml2_finish_element (client->ebml2, list);
  kr_ebml2_finish_element (client->ebml2, command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
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
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_videoport_destroy_cmd (kr_client_t *client) {

  unsigned char *compositor_command;
  unsigned char *destroy_videoport;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &compositor_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_LOCAL_VIDEOPORT_DESTROY, &destroy_videoport);
  kr_ebml2_finish_element (client->ebml2, destroy_videoport);
  kr_ebml2_finish_element (client->ebml2, compositor_command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_videoport_create_cmd (kr_client_t *client) {

  unsigned char *compositor_command;
  unsigned char *create_videoport;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &compositor_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_LOCAL_VIDEOPORT_CREATE, &create_videoport);
  kr_ebml2_finish_element (client->ebml2, create_videoport);
  kr_ebml2_finish_element (client->ebml2, compositor_command);
    
  if (client->autosync == 1) {
    kr_client_sync (client);
  }
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
      printke ("krad compositor client: unexpected read return value %d in kr_videoport_process_thread", ret);
    }
    kr_videoport->callback (kr_videoport->kr_shm->buffer, kr_videoport->pointer);

    // write a byte to socket
    ret = write (kr_videoport->sd, buf, 1);
    if (ret != 1) {
      printke ("krad compositor client: unexpected write return value %d in kr_videoport_process_thread", ret);
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

