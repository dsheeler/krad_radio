#include "krad_radio_client_internal.h"
#include "krad_radio_client.h"

#include "krad_radio_common.h"
#include "krad_compositor_common.h"
#include "krad_transponder_common.h"
#include "krad_mixer_common.h"

#include "krad_compositor_client.h"
#include "krad_transponder_client.h"
#include "krad_mixer_client.h"

static int kr_ebml_to_radio_rep (unsigned char *ebml_frag, kr_radio_t *radio_rep);

int kr_client_sync (kr_client_t *client) {
  kr_io2_advance (client->io, client->ebml2->pos);
  kr_io2_flush (client->io);
  kr_ebml2_set_buffer ( client->ebml2, client->io->buf, client->io->space );
  return 0;
}

static int kr_remote_port_valid (int port);

kr_client_t *kr_client_create (char *client_name) {

  kr_client_t *client;
  int len;
  
  if (client_name == NULL) {
    return NULL;
  }
  len = strlen (client_name);
  if ((len == 0) || (len > 255)) {
    return NULL;
  }

  client = calloc (1, sizeof(kr_client_t));
  client->name = strdup (client_name);

  client->autosync = 1;

  return client;
}

int kr_connect_remote (kr_client_t *client, char *host, int port) {
  
  char url[532];
  int len;
  if ((client == NULL) || (host == NULL) || 
      ((port < 1) || (port > 65535))) {
    return 0;
  }
  len = strlen (host);
  if ((len == 0) || (len > 512)) {
    return 0;
  }

  snprintf (url, sizeof(url), "%s:%d", host, port);

  return kr_connect (client, url);
}

int kr_connect (kr_client_t *client, char *sysname) {

  if (client == NULL) {
    return 0;
  }
  if (kr_connected (client)) {
    kr_disconnect (client);
  }
  client->krad_ipc_client = krad_ipc_connect (sysname);
  if (client->krad_ipc_client != NULL) {

    client->krad_ebml = client->krad_ipc_client->krad_ebml;
    client->ebml2 = client->krad_ipc_client->ebml2;
    client->io = client->krad_ipc_client->io;
    
    return 1;
  }

  return 0;
}

int kr_connected (kr_client_t *client) {
  if (client->krad_ipc_client != NULL) {
    return 1; 
  }
  return 0;
}

int kr_disconnect (kr_client_t *client) {
  if (client != NULL) {
    if (kr_connected (client)) {
      krad_ipc_disconnect (client->krad_ipc_client);
      client->krad_ipc_client = NULL;
      return 1;
    }
    return -2;
  }
  return -1;
}

int kr_client_destroy (kr_client_t **client) {
  if (*client != NULL) {
    if (kr_connected (*client)) {
      kr_disconnect (*client);
    }
    if ((*client)->name != NULL) {
      free ((*client)->name);
      (*client)->name = NULL;
    }
    free (*client);
    *client = NULL;
    return 1;
  }
  return -1;
}

krad_ebml_t *kr_client_get_ebml (kr_client_t *client) {
  return client->krad_ipc_client->krad_ebml;
}

int kr_client_local (kr_client_t *client) {
  if (client != NULL) {
    if (kr_connected (client)) {
      if (client->krad_ipc_client->tcp_port == 0) {
        return 1;
      }
      return 0;
    }
  }
  return -1;
}

int kr_client_get_fd (kr_client_t *client) {
  if (client != NULL) {
    if (kr_connected (client)) {
      return client->krad_ipc_client->sd;
    }
  }
  return -1;
}

void kr_subscribe_all (kr_client_t *client) {
  kr_subscribe (client, EBML_ID_KRAD_RADIO_GLOBAL_BROADCAST);
}

void kr_broadcast_subscribe (kr_client_t *client, uint32_t broadcast_id) {

  uint64_t radio_command;

  krad_ebml_start_element (client->krad_ebml, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  krad_ebml_write_int32 (client->krad_ebml, EBML_ID_KRAD_RADIO_CMD_BROADCAST_SUBSCRIBE, broadcast_id);
  krad_ebml_finish_element (client->krad_ebml, radio_command);
    
  krad_ebml_write_sync (client->krad_ebml);

  client->subscriber = 1;
}

void kr_shm_destroy (kr_shm_t *kr_shm) {

  if (kr_shm != NULL) {
    if (kr_shm->buffer != NULL) {
      munmap (kr_shm->buffer, kr_shm->size);
      kr_shm->buffer = NULL;
    }
    if (kr_shm->fd != 0) {
      close (kr_shm->fd);
    }
    free(kr_shm);
  }
}

kr_shm_t *kr_shm_create (kr_client_t *client) {

  char filename[] = "/tmp/krad-shm-XXXXXX";
  kr_shm_t *kr_shm;

  kr_shm = calloc (1, sizeof(kr_shm_t));

  if (kr_shm == NULL) {
    return NULL;
  }

  kr_shm->size = 960 * 540 * 4 * 2;

  kr_shm->fd = mkstemp (filename);
  if (kr_shm->fd < 0) {
    fprintf(stderr, "open %s failed: %m\n", filename);
    kr_shm_destroy (kr_shm);
    return NULL;
  }

  if (ftruncate (kr_shm->fd, kr_shm->size) < 0) {
    fprintf (stderr, "ftruncate failed: %m\n");
    kr_shm_destroy (kr_shm);
    return NULL;
  }

  kr_shm->buffer = mmap (NULL, kr_shm->size, PROT_READ | PROT_WRITE, MAP_SHARED, kr_shm->fd, 0);
  unlink (filename);

  if (kr_shm->buffer == MAP_FAILED) {
    fprintf (stderr, "mmap failed: %m\n");
    kr_shm_destroy (kr_shm);
    return NULL;
  }

  return kr_shm;

}

int kr_send_fd (kr_client_t *client, int fd) {
  return krad_ipc_client_send_fd (client->krad_ipc_client, fd);
}

void kr_response_free_string (char **string) {
  free (*string);
}

char *kr_response_alloc_string (int length) {
  return calloc (1, length + 16);
}

int kr_poll (kr_client_t *client, uint32_t timeout_ms) {

  struct pollfd pollfds[1];

  pollfds[0].fd = client->krad_ipc_client->sd;
  pollfds[0].events = POLLIN;

  return poll (pollfds, 1, timeout_ms);
}

int kr_delivery_final (kr_client_t *client) {
  return client->last_delivery_was_final;
}

void kr_delivery_final_reset (kr_client_t *client) {
  client->last_delivery_was_final = 0;
}

int kr_delivery_wait_until_final (kr_client_t *client, uint32_t timeout_ms) {
  if (kr_delivery_final (client)) {
    kr_delivery_final_reset (client);
    return 0;
  }
  return kr_poll (client, timeout_ms);
}

int kr_radio_uptime_to_string (uint64_t uptime, char *string) {

  int days;
  int hours;
  int minutes;
  int seconds;
  int pos;
  
  pos = 0;

  days = uptime / (60*60*24);
  minutes = uptime / 60;
  hours = (minutes / 60) % 24;
  minutes %= 60;
  seconds = uptime % 60;

  if (days) {
    pos += sprintf (string + pos, "%d day%s,", days, (days != 1) ? "s" : "");
  }
  if (hours) {
    pos += sprintf (string + pos, "%d:%02d", hours, minutes);
  } else {
    if (minutes) {
      pos += sprintf (string + pos, "%02d min", minutes);
    } else {
      pos += sprintf (string + pos, "%02d seconds", seconds);
    }
  }
  return pos;
}

int kr_response_read_into_string (unsigned char *ebml_frag, uint64_t ebml_data_size, char *string) {

  int pos;
  pos = 0;

  if (ebml_data_size > 0) {
    pos += krad_ebml_read_string_from_frag (ebml_frag, ebml_data_size, string);
    string[pos++] = '\0';
  }
  return pos;
}

int kr_response_get_string (unsigned char *ebml_frag, uint64_t ebml_data_size, char **string) {

  int pos;
  pos = 0;

  if (ebml_data_size > 0) {
    *string = kr_response_alloc_string (ebml_data_size + 1);
    pos += krad_ebml_read_string_from_frag (ebml_frag + pos, ebml_data_size, *string);
    (*string)[pos] = '\0';
  }
  return pos;
}

int kr_radio_response_get_string_from_tag (unsigned char *ebml_frag, uint64_t item_size, char **string) {

  int item_pos;
  int string_pos;
  int ret;
  uint32_t ebml_id;
  uint64_t ebml_data_size;
  
  ret = 0;
  item_pos = 0;
  string_pos = 0;

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  string_pos += sprintf (*string + string_pos, "Tag on ");
  ret = krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, *string + string_pos);
  item_pos += ret;
  string_pos += ret;
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  string_pos += sprintf (*string + string_pos, ": ");
  ret = krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, *string + string_pos);
  item_pos += ret;
  string_pos += ret;

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  string_pos += sprintf (*string + string_pos, " - ");
  ret = krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, *string + string_pos);
  item_pos += ret;
  string_pos += ret;

  string_pos += sprintf (*string + string_pos, "\n");

  return string_pos;
}

int kr_radio_response_get_string_from_remote (unsigned char *ebml_frag, uint64_t item_size, char **string) {

  int item_pos;
  int string_pos;
  int ret;
  uint32_t ebml_id;
  uint64_t ebml_data_size;
  int number;
  
  ret = 0;
  item_pos = 0;
  string_pos = 0;

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  string_pos += sprintf (*string + string_pos, "Interface: ");
  ret = krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, *string + string_pos);
  item_pos += ret;
  string_pos += ret;
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  number = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);
  string_pos += sprintf (*string + string_pos, " Port: %d", number);

  return string_pos;
}

rep_to_string_t kr_response_get_rep_to_string_converter (kr_address_t *address) {

  switch ( address->path.unit ) {
    case KR_MIXER:
      if (address->path.subunit.mixer_subunit == 0) {
        return NULL;
      }
      switch ( address->path.subunit.mixer_subunit ) {
        case KR_PORTGROUP:
          return kr_mixer_response_get_string_from_portgroup;
        default:
          return NULL;
      }
    case KR_COMPOSITOR:
    case KR_TRANSPONDER:
    case KR_STATION:
    default:
      return NULL;
  }
}

int kr_radio_response_get_string_from_radio (unsigned char *ebml_frag, uint64_t item_size, char **string) {

  int pos;
  kr_radio_t kr_radio;

  pos = 0;

  kr_ebml_to_radio_rep (ebml_frag, &kr_radio);
  pos += sprintf (*string + pos, "Krad Radio System Info:\n%s\n", kr_radio.sysinfo);
  if (kr_radio.logname[0] != '\0') {
    pos += sprintf (*string + pos, "Logname: %s\n", kr_radio.logname);
  }
  pos += sprintf (*string + pos, "Station Uptime: ");
  pos += kr_radio_uptime_to_string (kr_radio.uptime, *string + pos);
  pos += sprintf (*string + pos, "\n");  
  pos += sprintf (*string + pos, "System CPU Usage: %u%%", kr_radio.cpu_usage);  
  
  return pos; 
}

int kr_radio_response_get_string_from_cpu (unsigned char *ebml_frag, uint64_t item_size, char **string) {

  int ebml_pos;
  int pos;
  uint32_t ebml_id;
  uint64_t ebml_data_size;

  ebml_pos = 0;
  pos = 0;

  uint32_t usage;

  ebml_pos += krad_ebml_read_element_from_frag (ebml_frag + ebml_pos, &ebml_id, &ebml_data_size);
  usage = krad_ebml_read_number_from_frag (ebml_frag + ebml_pos, ebml_data_size);
  pos += sprintf (*string + pos, "System CPU Usage: %u%%", usage);
  return pos; 
}

int kr_radio_response_get_string_from_tags (unsigned char *ebml_frag, uint64_t item_size, char **string) {

  int pos;
  int ebml_pos;

  ebml_pos = 0;
  pos = 0;

  char tag_item_actual[256];  
  char tag_name_actual[256];
  char tag_value_actual[256];
  
  char *tag_item = tag_item_actual;  
  char *tag_name = tag_name_actual;
  char *tag_value = tag_value_actual;
  
  tag_item_actual[0] = '\0';  
  tag_name_actual[0] = '\0';
  tag_value_actual[0] = '\0';
  
  while (ebml_pos < item_size) {
    if (ebml_pos > 0) {
      pos += sprintf (*string + pos, "\n");  
    }
    ebml_pos += kr_read_tag_frag ( ebml_frag + ebml_pos, &tag_item, &tag_name, &tag_value );
    pos += sprintf (*string + pos, "%s tag: %s - %s", tag_item, tag_name, tag_value);  
  }
  return pos; 
}

int kr_radio_response_to_string (kr_response_t *response, char **string) {

  if (response->address.path.subunit.zero == KR_TAGS) {
    *string = kr_response_alloc_string (response->size * 4);
    return kr_radio_response_get_string_from_tags (response->buffer, response->size, string);
  }

  switch ( response->address.path.subunit.station_subunit ) {
    case KR_STATION_UNIT:
      *string = kr_response_alloc_string (response->size * 4);
      return kr_radio_response_get_string_from_radio (response->buffer, response->size, string);
    case KR_CPU:
      *string = kr_response_alloc_string (response->size * 8);
      return kr_radio_response_get_string_from_cpu (response->buffer, response->size, string);
    case KR_REMOTE:
      *string = kr_response_alloc_string (response->size * 8);
      return kr_radio_response_get_string_from_remote (response->buffer, response->size, string);
  }
  
  return 0;  
}

int kr_radio_response_to_int (kr_response_t *kr_response, int *number) {

  int pos;
  uint32_t ebml_id;
  uint64_t ebml_data_size;

  pos = 0;
  
  pos += krad_ebml_read_element_from_frag (kr_response->buffer + pos, &ebml_id, &ebml_data_size);

  switch ( ebml_id ) {
    case EBML_ID_KRAD_RADIO_UPTIME:
      *number = krad_ebml_read_number_from_frag (kr_response->buffer + pos, ebml_data_size);
      return 1;
      break;
    case EBML_ID_KRAD_RADIO_SYSTEM_CPU_USAGE:
      *number = krad_ebml_read_number_from_frag (kr_response->buffer + pos, ebml_data_size);
      return 1;
      break;
  }

  *number = 0;
  return 0;  
}

int kr_response_to_int (kr_response_t *response, int *number) {

  if (response->size == 0) {
    return 0;
  }

  switch ( response->address.path.unit ) {
    case KR_STATION:
      return kr_radio_response_to_int (response, number);
    case KR_MIXER:
      //
      break;
    case KR_COMPOSITOR:
      //
      break;
    case KR_TRANSPONDER:
      //
      break;
  }
  return 0;
}

int kr_response_to_float (kr_response_t *response, float *number) {

  uint32_t ebml_id;
  uint64_t ebml_data_size;
  int pos;
  
  pos = 0;
  
  if (response->size == 0) {
    return 0;
  }

  if (response->notice != EBML_ID_KRAD_SUBUNIT_CONTROL) {
    return 0;
  }

  switch ( response->address.path.unit ) {
    case KR_STATION:
    
      break;
    case KR_MIXER:
      if (response->address.path.subunit.mixer_subunit == KR_PORTGROUP) {
        if ((response->address.control.portgroup_control == KR_VOLUME) ||
            (response->address.control.portgroup_control == KR_CROSSFADE) ||
            (response->address.control.portgroup_control == KR_PEAK)) {
           pos += krad_ebml_read_element_from_frag (response->buffer, &ebml_id, &ebml_data_size);  
          *number = krad_ebml_read_float_from_frag_add (response->buffer + pos, ebml_data_size, &pos);
          return 1;
        }
      }
      if (response->address.path.subunit.mixer_subunit == KR_EFFECT) {
        pos += krad_ebml_read_element_from_frag (response->buffer, &ebml_id, &ebml_data_size);  
        *number = krad_ebml_read_float_from_frag_add (response->buffer + pos, ebml_data_size, &pos);
        return 1;
      }
    case KR_COMPOSITOR:
      //
      break;
    case KR_TRANSPONDER:
      //
      break;
  }
  return 0;
}

int kr_response_to_string (kr_response_t *response, char **string) {

  if (response->notice == EBML_ID_KRAD_RADIO_UNIT_DESTROYED) {
    return 0;
  }
  
  if (response->size == 0) {
    return 0;
  }

  switch ( response->address.path.unit ) {
    case KR_STATION:
      return kr_radio_response_to_string (response, string);
    case KR_MIXER:
      return kr_mixer_response_to_string (response, string);
    case KR_COMPOSITOR:
      return kr_compositor_response_to_string (response, string);
    case KR_TRANSPONDER:
      return kr_transponder_response_to_string (response, string);
      break;
  }
  return 0;
}

int kr_ebml_to_remote_status_rep (unsigned char *ebml_frag, kr_remote_t *remote) {
    
  int item_pos;
  uint32_t ebml_id;
  uint64_t ebml_data_size;
  
  item_pos = 0;

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, remote->interface);
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  remote->port = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  return item_pos;
}

int kr_ebml_to_tag_rep (unsigned char *ebml_frag, kr_tag_t *tag) {
    
  int item_pos;
  uint32_t ebml_id;
  uint64_t ebml_data_size;
  
  item_pos = 0;

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, tag->unit);
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, tag->name);
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, tag->value);

  return item_pos;
}

void kr_response_address (kr_response_t *response, kr_address_t **address) {
  *address = &response->address;
}

static int kr_ebml_to_radio_rep (unsigned char *ebml_frag, kr_radio_t *radio_rep) {

  uint32_t ebml_id;
  uint64_t ebml_data_size;
  int item_pos;

  item_pos = 0;

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, radio_rep->sysinfo);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, radio_rep->logname);
  
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  radio_rep->uptime = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);  
  radio_rep->cpu_usage = krad_ebml_read_number_from_frag_add (ebml_frag + item_pos, ebml_data_size, &item_pos);

  return 1;
}

int kr_response_to_rep (kr_crate_t *crate) {

  if (crate->notice == EBML_ID_KRAD_RADIO_UNIT_DESTROYED) {
    return 0;
  }
  
  if (crate->size == 0) {
    return 0;
  }

  switch ( crate->address.path.unit ) {
    case KR_STATION:
      crate->contains = KR_STATION;
      switch ( crate->address.path.subunit.station_subunit ) {
        case KR_STATION_UNIT:
          kr_ebml_to_radio_rep (crate->buffer, &crate->rep.radio);
          return 1;
        default:
          break;
      }
      break;
    case KR_MIXER:
      if ((crate->address.path.subunit.mixer_subunit != KR_PORTGROUP) && 
          (crate->notice == EBML_ID_KRAD_UNIT_INFO)) {
        crate->contains = KR_MIXER;
        kr_ebml_to_mixer_rep (crate->buffer, &crate->rep.mixer);
        return 1;
      }
      switch ( crate->address.path.subunit.mixer_subunit ) {
        case KR_EFFECT:

          break;
        case KR_PORTGROUP:
          if ((crate->notice == EBML_ID_KRAD_SUBUNIT_CREATED) || ((crate->notice == EBML_ID_KRAD_SUBUNIT_INFO))) {
            crate->contains = KR_PORTGROUP;
            kr_ebml_to_mixer_portgroup_rep (crate->buffer, &crate->rep.portgroup);
            return 1;
          }
          break;
        default:
          break;
      }
      break;
    case KR_COMPOSITOR:
      if ((crate->address.path.unit == KR_COMPOSITOR) && (crate->address.path.subunit.zero == KR_UNIT) && 
          (crate->notice == EBML_ID_KRAD_UNIT_INFO)) {
        crate->contains = KR_COMPOSITOR;
        kr_ebml_to_compositor_rep (crate->buffer, &crate->rep.compositor);
        return 1;
      }
      break;
    case KR_TRANSPONDER:
      break;
  }

  return 0;
}

uint32_t kr_response_size (kr_response_t *kr_response) {
  return kr_response->size;
}

void kr_response_free (kr_response_t **kr_response) {
  if (*kr_response != NULL) {
    if ((*kr_response)->buffer != NULL) {
      free ((*kr_response)->buffer);
    }
    free ((*kr_response));
  }
}

kr_response_t *kr_response_alloc () {
  return calloc (1, sizeof(kr_response_t));
}

int krad_radio_address_to_ebml (krad_ebml_t *krad_ebml, uint64_t *element_loc, kr_address_t *address) {

  switch (address->path.unit) {
    case KR_MIXER:
      krad_ebml_start_element (krad_ebml, EBML_ID_KRAD_MIXER_MSG, element_loc);
      krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT, address->path.subunit.mixer_subunit);
      if (address->path.subunit.mixer_subunit != KR_UNIT) {
        krad_ebml_write_string (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NAME, address->id.name);
        if (address->path.subunit.mixer_subunit == KR_EFFECT) {
          krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, address->sub_id);
          krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, address->sub_id2);
          krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_CONTROL, address->control.effect_control);
        }
        if (address->path.subunit.mixer_subunit == KR_PORTGROUP) {
          krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_CONTROL, address->control.portgroup_control);
        }
      }
      return 1;
    case KR_COMPOSITOR:
      krad_ebml_start_element (krad_ebml, EBML_ID_KRAD_COMPOSITOR_MSG, element_loc);
      krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT, address->path.subunit.compositor_subunit);
      if (address->path.subunit.compositor_subunit != KR_UNIT) {
        krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, address->id.number);
      }
      return 1;
    case KR_TRANSPONDER:
      krad_ebml_start_element (krad_ebml, EBML_ID_KRAD_TRANSPONDER_MSG, element_loc);
      krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT, address->path.subunit.transponder_subunit);
      if (address->path.subunit.transponder_subunit != KR_UNIT) {
        //if (address->path.subunit.transponder_subunit == KR_ADAPTER) {
          krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, address->id.number);
        //}
      }
      return 1;
    case KR_STATION:
      krad_ebml_start_element (krad_ebml, EBML_ID_KRAD_RADIO_MSG, element_loc);
      krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT, address->path.subunit.station_subunit);
      if (address->path.subunit.station_subunit != KR_UNIT) {
        if (address->path.subunit.station_subunit == KR_REMOTE) {
          krad_ebml_write_int32 (krad_ebml, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, address->id.number);
        }
      }
      return 1;
  }
  
  return 0;
}

inline int krad_read_message_notice_from_ebml (krad_ebml_t *ebml, uint32_t *message_notice) {

  uint32_t ebml_id;
  uint64_t ebml_data_size;

  ebml_id = 0;
  ebml_data_size = 0;

  krad_ebml_read_element (ebml, &ebml_id, &ebml_data_size);
  *message_notice = krad_ebml_read_number ( ebml, ebml_data_size);

  return 0;
}

int krad_read_address_from_ebml (krad_ebml_t *ebml, kr_address_t *address) {

  uint32_t ebml_id;
  uint64_t ebml_data_size;

  ebml_id = 0;
  ebml_data_size = 0;

  krad_ebml_read_element (ebml, &ebml_id, &ebml_data_size);

  switch ( ebml_id ) {
    case EBML_ID_KRAD_RADIO_MSG:
      address->path.unit = KR_STATION;
      krad_ebml_read_element (ebml, &ebml_id, &ebml_data_size);
      address->path.subunit.zero = krad_ebml_read_number ( ebml, ebml_data_size );
      if (address->path.subunit.station_subunit == KR_REMOTE) {
        krad_ebml_read_element (ebml, &ebml_id, &ebml_data_size);
        address->id.number = krad_ebml_read_number ( ebml, ebml_data_size );
      }
      break;
    case EBML_ID_KRAD_MIXER_MSG:
      address->path.unit = KR_MIXER;
      krad_ebml_read_element (ebml, &ebml_id, &ebml_data_size);
      address->path.subunit.mixer_subunit = krad_ebml_read_number ( ebml, ebml_data_size );
      if (address->path.subunit.mixer_subunit != KR_UNIT) {
        krad_ebml_read_element (ebml, &ebml_id, &ebml_data_size);
        krad_ebml_read_string (ebml, address->id.name, ebml_data_size);
        if (address->path.subunit.mixer_subunit == KR_EFFECT) {
          krad_ebml_read_element (ebml, &ebml_id, &ebml_data_size);
          address->sub_id = krad_ebml_read_number ( ebml, ebml_data_size );
          krad_ebml_read_element (ebml, &ebml_id, &ebml_data_size);
          address->sub_id2 = krad_ebml_read_number ( ebml, ebml_data_size );
          krad_ebml_read_element (ebml, &ebml_id, &ebml_data_size);
          address->control.effect_control = krad_ebml_read_number ( ebml, ebml_data_size );
        }
        if (address->path.subunit.mixer_subunit == KR_PORTGROUP) {
          krad_ebml_read_element (ebml, &ebml_id, &ebml_data_size);
          address->control.portgroup_control = krad_ebml_read_number ( ebml, ebml_data_size) ;
        }
      }
      break;
    case EBML_ID_KRAD_COMPOSITOR_MSG:
      address->path.unit = KR_COMPOSITOR;
      krad_ebml_read_element (ebml, &ebml_id, &ebml_data_size);
      address->path.subunit.zero = krad_ebml_read_number ( ebml, ebml_data_size );
      if (address->path.subunit.mixer_subunit != KR_UNIT) {
        krad_ebml_read_element (ebml, &ebml_id, &ebml_data_size);
        address->id.number = krad_ebml_read_number ( ebml, ebml_data_size );
      }
      break;
    case EBML_ID_KRAD_TRANSPONDER_MSG:
      address->path.unit = KR_TRANSPONDER;
      krad_ebml_read_element (ebml, &ebml_id, &ebml_data_size);
      address->path.subunit.transponder_subunit = krad_ebml_read_number ( ebml, ebml_data_size );
      if (address->path.subunit.transponder_subunit != KR_UNIT) {
        krad_ebml_read_element (ebml, &ebml_id, &ebml_data_size);
        address->id.number = krad_ebml_read_number ( ebml, ebml_data_size );
      }
      break;
  }
  
  return 1;
}

int kr_crate_loaded (kr_crate_t *crate) {
  if (kr_response_to_rep (crate)) {
    return 1;
  }
  return 0;
}

inline int krad_message_notice_has_payload (uint32_t notice) {

  if (notice == EBML_ID_KRAD_SHIPMENT_TERMINATOR) {
    return 0;
  }

  if (notice != EBML_ID_KRAD_RADIO_UNIT_DESTROYED) {
    return 1;
  }
  return 0;
}

char *kr_message_notice_to_string (uint32_t notice) {
  switch (notice) {
    case EBML_ID_KRAD_SUBUNIT_CONTROL:
      return "Subunit Controlled";
    case EBML_ID_KRAD_SUBUNIT_CREATED:
      return "Subunit Created";
    case EBML_ID_KRAD_UNIT_INFO:
      return "Unit Info";
    case EBML_ID_KRAD_SUBUNIT_INFO:
      return "Subunit Info";
    case EBML_ID_KRAD_RADIO_UNIT_DESTROYED:
      return "Subunit Destroyed";
  }
  
  return "Unknown Message Notice";
}

void kr_message_notice_debug_print (uint32_t notice) {
  printf ("Message Notice: %s\n", kr_message_notice_to_string (notice));
}

void kr_response_payload_print_raw_ebml (kr_response_t *response) {

  int i;

  printf ("\nRaw EBML: \n");
  for (i = 0; i < response->size; i++) {
    printf ("%02X", response->buffer[i]);
  }
  printf ("\nEnd EBML\n");
}

uint32_t kr_response_get_event (kr_response_t *response) {
  return response->notice;
}

void kr_client_response_get (kr_client_t *client, kr_response_t **kr_response) {

  krad_ipc_client_t *kr_ipc_client;
  kr_response_t *response;

  uint32_t ebml_id;
  uint64_t ebml_data_size;

  ebml_id = 0;
  ebml_data_size = 0;

  kr_ipc_client = client->krad_ipc_client;
  response = kr_response_alloc ();
  *kr_response = response;

  response->inside.actual = &response->rep.actual;

  //printf ("KR Client Response get start\n");

  krad_read_address_from_ebml (kr_ipc_client->krad_ebml, &response->address);
  krad_read_message_notice_from_ebml (kr_ipc_client->krad_ebml, &response->notice);
  
  response->addr = &response->address;
  
  if (!client->subscriber) {
    if (response->notice == EBML_ID_KRAD_SHIPMENT_TERMINATOR) {
      client->last_delivery_was_final = 1;
    } else {
      client->last_delivery_was_final = 0;
    }
  }

  //kr_address_debug_print (&response->address);
  //kr_message_notice_debug_print (response->notice);

  if (krad_message_notice_has_payload (response->notice)) {
    krad_ebml_read_element (kr_ipc_client->krad_ebml, &ebml_id, &ebml_data_size);
    if (ebml_data_size > 0) {
      //printf ("KR Client Response payload size: %"PRIu64"\n", ebml_data_size);
      response->size = ebml_data_size;
      response->buffer = malloc (2048);
      krad_ebml_read (kr_ipc_client->krad_ebml, response->buffer, ebml_data_size);
    }
  }
  
  if (kr_uncrate_int (response, &response->integer)) {
    response->has_int = 1;
  }
  if (kr_uncrate_float (response, &response->real)) {
    response->has_float = 1;
  }

  //kr_response_payload_print_raw_ebml (response);
 
  //printf ("KR Client Response get finish\n");
}

int kr_crate_has_float (kr_crate_t *crate) {
  return crate->has_float;
}

int kr_crate_has_int (kr_crate_t *crate) {
  return crate->has_int;
}

void kr_client_response_wait (kr_client_t *client, kr_response_t **kr_response) {
  kr_poll (client, 250);
  kr_client_response_get (client, kr_response);
}

void kr_client_response_wait_print (kr_client_t *client) {

  kr_response_t *response;
  char *string;
  int wait_time_ms;
  int length;
  int got_a_response;

  got_a_response = 0;
  string = NULL;
  response = NULL;  
  wait_time_ms = 250;

  while (kr_delivery_wait_until_final (client, wait_time_ms)) {
  //if (kr_poll (client, wait_time_ms)) {
    kr_client_response_get (client, &response);
    if (response != NULL) {
      got_a_response = 1;
      length = kr_response_to_string (response, &string);
      if (length > 0) {
        printf ("%s\n", string);
        kr_response_free_string (&string);
      }
      kr_response_free (&response);
    }
  }

  if (got_a_response == 0) {
    printf ("No response after waiting %dms\n", wait_time_ms);
  }
}

void kr_set_dir (kr_client_t *client, char *dir) {

  unsigned char *command;
  unsigned char *setdir;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_SET_DIR, &setdir);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_DIR, dir);
  kr_ebml2_finish_element (client->ebml2, setdir);
  kr_ebml2_finish_element (client->ebml2, command);

  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_system_info (kr_client_t *client) {

  unsigned char *command;
  unsigned char *info_command;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_GET_SYSTEM_INFO, &info_command);
  kr_ebml2_finish_element (client->ebml2, info_command);
  kr_ebml2_finish_element (client->ebml2, command);

  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

static int kr_remote_port_valid (int port) {
  if ((port >= 0) && (port <= 65535)) {
    return 1;
  }
  return 0;
}

void kr_remote_list (kr_client_t *client) {

  unsigned char *command;
  unsigned char *remote_status_command;
  
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_GET_REMOTE_STATUS, &remote_status_command);
  kr_ebml2_finish_element (client->ebml2, remote_status_command);
  kr_ebml2_finish_element (client->ebml2, command);

  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

int kr_remote_on (kr_client_t *client, char *interface, int port) {

  unsigned char *radio_command;
  unsigned char *enable_remote;

  if (!kr_remote_port_valid (port)) {
    return -1;
  }
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_REMOTE_ENABLE, &enable_remote);
  if ((interface != NULL) && (strlen(interface))) {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_REMOTE_INTERFACE, interface);
  } else {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_REMOTE_INTERFACE, "");
  }
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_TCP_PORT, port);
  kr_ebml2_finish_element (client->ebml2, enable_remote);
  kr_ebml2_finish_element (client->ebml2, radio_command);

  if (client->autosync == 1) {
    kr_client_sync (client);
  }
  return 1;
}

int kr_remote_off (kr_client_t *client, char *interface, int port) {

  unsigned char *radio_command;
  unsigned char *disable_remote;

  if (!kr_remote_port_valid (port)) {
    return -1;
  }

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_REMOTE_DISABLE, &disable_remote);
  if ((interface != NULL) && (strlen(interface))) {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_REMOTE_INTERFACE, interface);
  } else {
    kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_REMOTE_INTERFACE, "");
  }
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_TCP_PORT, port);
  kr_ebml2_finish_element (client->ebml2, disable_remote);
  kr_ebml2_finish_element (client->ebml2, radio_command);

  if (client->autosync == 1) {
    kr_client_sync (client);
  }
  return 1;
}

void kr_web_enable (kr_client_t *client, int http_port, int websocket_port,
                    char *headcode, char *header, char *footer) {

  unsigned char *radio_command;
  unsigned char *webon;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_WEB_ENABLE, &webon);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_HTTP_PORT, http_port);
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_WEBSOCKET_PORT, websocket_port);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_WEB_HEADCODE, headcode);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_WEB_HEADER, header);
  kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_RADIO_WEB_FOOTER, footer);
  kr_ebml2_finish_element (client->ebml2, webon);
  kr_ebml2_finish_element (client->ebml2, radio_command);


  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_web_disable (kr_client_t *client) {

  unsigned char *radio_command;
  unsigned char *weboff;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_WEB_DISABLE, &weboff);
  kr_ebml2_finish_element (client->ebml2, weboff);
  kr_ebml2_finish_element (client->ebml2, radio_command);

  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_osc_enable (kr_client_t *client, int port) {

  unsigned char *radio_command;
  unsigned char *enable_osc;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_OSC_ENABLE, &enable_osc);  
  kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_UDP_PORT, port);
  kr_ebml2_finish_element (client->ebml2, enable_osc);
  kr_ebml2_finish_element (client->ebml2, radio_command);

  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_osc_disable (kr_client_t *client) {

  unsigned char *radio_command;
  unsigned char *disable_osc;

  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_RADIO_CMD_OSC_DISABLE, &disable_osc);
  kr_ebml2_finish_element (client->ebml2, disable_osc);
  kr_ebml2_finish_element (client->ebml2, radio_command);

  if (client->autosync == 1) {
    kr_client_sync (client);
  }
}

void kr_read_tag_inner ( kr_client_t *client, char **tag_item, char **tag_name, char **tag_value ) {

  uint32_t ebml_id;
  uint64_t ebml_data_size;

  krad_ebml_read_element (client->krad_ebml, &ebml_id, &ebml_data_size);
  krad_ebml_read_string (client->krad_ebml, *tag_item, ebml_data_size);
  krad_ebml_read_element (client->krad_ebml, &ebml_id, &ebml_data_size);  
  krad_ebml_read_string (client->krad_ebml, *tag_name, ebml_data_size);
  krad_ebml_read_element (client->krad_ebml, &ebml_id, &ebml_data_size);  
  krad_ebml_read_string (client->krad_ebml, *tag_value, ebml_data_size);
}

int kr_read_tag_frag ( unsigned char *ebml_frag, char **tag_item, char **tag_name, char **tag_value ) {

  uint32_t ebml_id;
  uint64_t ebml_data_size;
  int item_pos;
  
  item_pos = 0;

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, *tag_item);

  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, *tag_name);
 
  item_pos += krad_ebml_read_element_from_frag (ebml_frag + item_pos, &ebml_id, &ebml_data_size);
  item_pos += krad_ebml_read_string_from_frag (ebml_frag + item_pos, ebml_data_size, *tag_value);
  
  return item_pos;
}

int kr_read_tag ( kr_client_t *client, char **tag_item, char **tag_name, char **tag_value ) {

  uint32_t ebml_id;
  uint64_t ebml_data_size;
  int bytes_read;

  bytes_read = 0;

  krad_ebml_read_element (client->krad_ebml, &ebml_id, &ebml_data_size);
  bytes_read += ebml_data_size + 9;
  krad_ebml_read_element (client->krad_ebml, &ebml_id, &ebml_data_size);
  krad_ebml_read_string (client->krad_ebml, *tag_item, ebml_data_size);
  krad_ebml_read_element (client->krad_ebml, &ebml_id, &ebml_data_size);  
  krad_ebml_read_string (client->krad_ebml, *tag_name, ebml_data_size);
  krad_ebml_read_element (client->krad_ebml, &ebml_id, &ebml_data_size);  
  krad_ebml_read_string (client->krad_ebml, *tag_value, ebml_data_size);

  return bytes_read;
}

void kr_tags (kr_client_t *client, char *item) {

  uint64_t radio_command;
  uint64_t get_tags;

  krad_ebml_start_element (client->krad_ebml, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  krad_ebml_start_element (client->krad_ebml, EBML_ID_KRAD_RADIO_CMD_LIST_TAGS, &get_tags);  

  if (item == NULL) {
    item = "station";
  }

  krad_ebml_write_string (client->krad_ebml, EBML_ID_KRAD_RADIO_TAG_ITEM, item);

  krad_ebml_finish_element (client->krad_ebml, get_tags);
  krad_ebml_finish_element (client->krad_ebml, radio_command);
    
  krad_ebml_write_sync (client->krad_ebml);
}

void kr_tag (kr_client_t *client, char *item, char *tag_name) {

  uint64_t radio_command;
  uint64_t get_tag;
  uint64_t tag;

  krad_ebml_start_element (client->krad_ebml, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  krad_ebml_start_element (client->krad_ebml, EBML_ID_KRAD_RADIO_CMD_GET_TAG, &get_tag);  
  krad_ebml_start_element (client->krad_ebml, EBML_ID_KRAD_RADIO_TAG, &tag);  

  if (item == NULL) {
    item = "station";
  }

  krad_ebml_write_string (client->krad_ebml, EBML_ID_KRAD_RADIO_TAG_ITEM, item);
  krad_ebml_write_string (client->krad_ebml, EBML_ID_KRAD_RADIO_TAG_NAME, tag_name);
  krad_ebml_write_string (client->krad_ebml, EBML_ID_KRAD_RADIO_TAG_VALUE, "");  

  krad_ebml_finish_element (client->krad_ebml, tag);
  krad_ebml_finish_element (client->krad_ebml, get_tag);
  krad_ebml_finish_element (client->krad_ebml, radio_command);
    
  krad_ebml_write_sync (client->krad_ebml);
}

void kr_set_tag (kr_client_t *client, char *item, char *tag_name, char *tag_value) {

  uint64_t radio_command;
  uint64_t set_tag;
  uint64_t tag;

  krad_ebml_start_element (client->krad_ebml, EBML_ID_KRAD_RADIO_CMD, &radio_command);
  krad_ebml_start_element (client->krad_ebml, EBML_ID_KRAD_RADIO_CMD_SET_TAG, &set_tag);  
  krad_ebml_start_element (client->krad_ebml, EBML_ID_KRAD_RADIO_TAG, &tag);  

  if (item == NULL) {
    item = "station";
  }

  krad_ebml_write_string (client->krad_ebml, EBML_ID_KRAD_RADIO_TAG_ITEM, item);
  krad_ebml_write_string (client->krad_ebml, EBML_ID_KRAD_RADIO_TAG_NAME, tag_name);
  krad_ebml_write_string (client->krad_ebml, EBML_ID_KRAD_RADIO_TAG_VALUE, tag_value);  

  krad_ebml_finish_element (client->krad_ebml, tag);
  krad_ebml_finish_element (client->krad_ebml, set_tag);
  krad_ebml_finish_element (client->krad_ebml, radio_command);
    
  krad_ebml_write_sync (client->krad_ebml);
}

void kr_address_debug_print (kr_address_t *addr) {

  kr_unit_t *unit;
  kr_subunit_t *subunit;
  kr_unit_id_t *id;
  kr_unit_control_name_t *control;
    
  unit = &addr->path.unit;
  subunit = &addr->path.subunit;
  id = &addr->id;
  control = &addr->control;

  printf ("Address: ");

  switch (*unit) {
    case KR_STATION:
      printf ("Station ");
      break;
    case KR_MIXER:
      printf ("Mixer ");
      break;
    case KR_COMPOSITOR:
      printf ("Compositor ");
      break;
    case KR_TRANSPONDER:
      printf ("Transponder ");
      break;
  }

  if (subunit->zero == KR_UNIT) {
    printf ("Unit Info\n");
    return;
  }

  switch (*unit) {
    case KR_STATION:
      if (subunit->zero == KR_TAGS) {
        printf ("Station Tags");
        break;
      }
      switch (subunit->station_subunit) {
        case KR_CPU:
          printf ("CPU Usage");
          break;
        case KR_REMOTE:
          printf ("Remote %d", id->number);
          break;
        default:
          break;
      }
      break;
    case KR_MIXER:
      switch (subunit->mixer_subunit) {
        case KR_PORTGROUP:
          printf ("%s ", id->name);
          if (control->portgroup_control == 0) {
            break;
          }
          if (control->portgroup_control == KR_VOLUME) {
            printf ("Volume");
          }
          if (control->portgroup_control == KR_CROSSFADE) {
            printf ("Crossfade");
          }
          if (control->portgroup_control == KR_PEAK) {
            printf ("Peak");
          }
          break;
        case KR_EFFECT:
          printf ("%s ", id->name);
          printf ("Effect: %d Control ID: %d Control: %s", addr->sub_id, addr->sub_id2,
                  effect_control_to_string(addr->control.effect_control));
          break;
      }
      break;
    case KR_COMPOSITOR:
      switch (subunit->compositor_subunit) {
        case KR_VIDEOPORT:
          printf ("Unit, Videoport");
          break;
        case KR_SPRITE:
          printf ("Unit, Sprite");
          break;
        case KR_TEXT:
          printf ("Unit, Text");
          break;
        case KR_VECTOR:
          printf ("Unit, Vector");
          break;
      }
      break;
    case KR_TRANSPONDER:
      switch (subunit->transponder_subunit) {
        case KR_ADAPTER:
          printf ("Adapter: %d", id->number);
          break;
        case KR_TRANSMITTER:
          printf ("Transmitter");
          break;
        case KR_RECEIVER:
          printf ("Receiver");
          break;
        case KR_DEMUXER:
          printf ("Demuxer");
          break;
        case KR_MUXER:
          printf ("Muxer");
          break;
        case KR_ENCODER:
          printf ("Encoder");
          break;
        case KR_DECODER:
          printf ("Decoder");
          break;
      }
      break;
  }

  printf ("\n");
}

#define MAX_TOKENS 8

int kr_string_to_address (char *string, kr_address_t *addr) {

  char *pch;
  char *save;
  char *tokens[MAX_TOKENS];
  int t;
  int i;
  char *effect_num;
  char *effect_control_id;
  char *effect_control;

  kr_unit_t *unit;
  kr_subunit_t *subunit;
  kr_unit_id_t *id;
  kr_unit_control_name_t *control;
    
  unit = &addr->path.unit;
  subunit = &addr->path.subunit;
  id = &addr->id;
  control = &addr->control;
  
  i = 0;
  t = 0;
  save = NULL;

  //printf ("string was %s\n", string);

  pch = strtok_r (string, "/ ", &save);
  while ((pch != NULL) && (t < MAX_TOKENS)) {
    tokens[t] = pch;
    t++;
    //printf ("%s ", pch);
    pch = strtok_r (NULL, "/ ", &save);
  }

  //printf ("\n%d tokens\n", t);

  for (i = 0; i < t; i++) {
    //printf ("%d: %s  ", i + 1, tokens[i]);
  }
  
  //printf ("\n");
  
  if (t < 2) {
    printf ("Invalid Unit Control Address: Less than 2 parts\n");
    return -1;
  }
  if (t > 5) {
    printf ("Invalid Unit Control Address: More than 5 parts\n");
    return -1;
  }
    
  // Determine Unit
  
  if (tokens[0][1] == '\0') {
    switch (tokens[0][0]) {
      case 'm':
        *unit = KR_MIXER;
        break;
      case 'c':
        *unit = KR_COMPOSITOR;
        break;
      case 't':
        *unit = KR_TRANSPONDER;
        break;
      case 'e':
        *unit = KR_MIXER;
        subunit->mixer_subunit = KR_EFFECT;
        if ((t != 4) && (t != 5)) {
          printf ("Invalid Mixer Effect control address, not 4 or 5 parts\n");
          return -1;
        }
        break;
      case 's':
        *unit = KR_COMPOSITOR;
        subunit->compositor_subunit = KR_SPRITE;
        break;
      case 'v':
        *unit = KR_COMPOSITOR;
        subunit->compositor_subunit = KR_VIDEOPORT;
        break;
    }
  } else {
    if (tokens[0][2] == '\0') {
      if ((tokens[0][0] == 'c') && (tokens[0][1] == 's')) {
        *unit = KR_COMPOSITOR;
        subunit->compositor_subunit = KR_SPRITE;
      }
      if ((tokens[0][0] == 'v') && (tokens[0][1] == 'e')) {
        *unit = KR_COMPOSITOR;
        subunit->compositor_subunit = KR_VECTOR;
      }
      if ((tokens[0][0] == 't') && (tokens[0][1] == 't')) {
        *unit = KR_COMPOSITOR;
        subunit->compositor_subunit = KR_TEXT;
      }
    } else {
      if (tokens[0][3] == '\0') {
        if ((tokens[0][0] == 'v') && (tokens[0][1] == 'e') && (tokens[0][2] == 'c')) {
          *unit = KR_COMPOSITOR;
        }
        if ((tokens[0][0] == 'm') && (tokens[0][1] == 'i') && (tokens[0][2] == 'x')) {
          *unit = KR_MIXER;
        }
        if ((tokens[0][0] == 'c') && (tokens[0][1] == 'o') && (tokens[0][2] == 'm')) {
          *unit = KR_COMPOSITOR;
        }
      } else {
        if (tokens[0][4] == '\0') {
          if ((tokens[0][0] == 't') && (tokens[0][1] == 'e') &&
              (tokens[0][2] == 'x') && (tokens[0][3] == 't')) {
            *unit = KR_COMPOSITOR;
            subunit->compositor_subunit = KR_TEXT;
          }
          if ((tokens[0][0] == 'c') && (tokens[0][1] == 'o') &&
              (tokens[0][2] == 'm') && (tokens[0][3] == 'p')) {
            *unit = KR_COMPOSITOR;
          }
        } else {
          if (tokens[0][5] == '\0') {
            if ((tokens[0][0] == 'v') && (tokens[0][1] == 'e') &&
                (tokens[0][2] == 'c') && (tokens[0][3] == 'o') && (tokens[0][4] == 'r')) {
              *unit = KR_COMPOSITOR;
              subunit->compositor_subunit = KR_VECTOR;
            }
           if ((tokens[0][0] == 'm') && (tokens[0][1] == 'i') &&
                (tokens[0][2] == 'x') && (tokens[0][3] == 'e') && (tokens[0][4] == 'r')) {
              *unit = KR_MIXER;
            }
          } else {
            if (tokens[0][6] == '\0') {
              if ((tokens[0][0] == 's') && (tokens[0][1] == 'p') && (tokens[0][2] == 'r') &&
                  (tokens[0][3] == 'i') && (tokens[0][4] == 't') && (tokens[0][5] == 'e')) {
                *unit = KR_COMPOSITOR;
                subunit->compositor_subunit = KR_SPRITE;
              }
            } else {
              if ((strlen(tokens[0]) == 10) && (strncmp(tokens[0], "compositor", 10) == 0)) {
                *unit = KR_COMPOSITOR;
              } else {
                if ((strlen(tokens[0]) == 11) && (strncmp(tokens[0], "transponder", 11) == 0)) {
                  *unit = KR_TRANSPONDER;
                }
              }
            }
          }
        }
      }
    }
  }

  if (*unit == 0) {
    printf ("Unit could not be identified\n");
    return -1;
  }
  
  switch (*unit) {
    case KR_MIXER:
      strncpy (id->name, tokens[1], sizeof (id->name));
      if (t == 2) {
        subunit->mixer_subunit = KR_PORTGROUP;
        control->portgroup_control = KR_VOLUME;
      }
      if (t == 3) {
        subunit->mixer_subunit = KR_PORTGROUP;
        if ((tokens[2][0] == 'f') || (tokens[2][0] == 'c')) {
          control->portgroup_control = KR_CROSSFADE;
        } else {
          if ((tokens[2][0] == 'v')) {
            control->portgroup_control = KR_VOLUME;
          } else {
            printf ("Invalid Mixer Portgroup Control\n");
            return -1;
          }
        }
      }
      if ((t == 4) || (t == 5)) {
        subunit->mixer_subunit = KR_EFFECT;
        if (t == 4) {
          effect_num = tokens[2];
          effect_control = tokens[3];
        }
        if (t == 5) {
          effect_num = tokens[2];
          effect_control_id = tokens[3];
          effect_control = tokens[4];
        }
        if (effect_num[0] == 'e') {
          addr->sub_id = 0;
        } else {
          if (effect_num[0] == 'l') {
            addr->sub_id = 1;
          } else {
            if (effect_num[0] == 'h') {
              addr->sub_id = 2;
            } else {
              if (effect_num[0] == 'a') {
                addr->sub_id = 3;
              } else {
                addr->sub_id = atoi(effect_num);
              }
            }
          }
        }
        if ((addr->sub_id == 0) && (t != 5)) {
          printf ("Invalid Mixer Portgroup Effect Control\n");
          return -1;
        }
        if ((addr->sub_id == 0) && (t == 5)) {
          addr->sub_id2 = atoi(effect_control_id);
        }
        if ((effect_control[0] == 'd') && (effect_control[1] == 'b')) {
          addr->control.effect_control = DB;
        } else {
          if ((effect_control[0] == 'h') && (effect_control[1] == 'z')) {
            addr->control.effect_control = HZ;
          } else {
            if ((effect_control[0] == 'b') && (effect_control[1] == 'w')) {
              addr->control.effect_control = BANDWIDTH;
            } else {
              if (effect_control[0] == 't') {
                addr->control.effect_control = TYPE;
              } else {
                if ((effect_control[0] == 'd') && (effect_control[1] == 'r')) {
                  addr->control.effect_control = DRIVE;
                } else {
                  if ((effect_control[0] == 'b') && (effect_control[1] == 'l')) {
                    addr->control.effect_control = BLEND;
                  }
                }
              }
            }
          }
        }
      }
      break;
    case KR_COMPOSITOR:
      if ((subunit->compositor_subunit > 0) && (t == 2)) {
          addr->id.number = atoi(tokens[1]);
          return 1;
      }
      if ((subunit->compositor_subunit > 0) && (t == 3)) {
          addr->id.number = atoi(tokens[1]);
          addr->control.compositor_control = krad_string_to_compositor_control (tokens[2]);
          return 1;
      }
      printf ("Invalid COMPOSITOR Control\n");
      return -1;
      break;
    case KR_TRANSPONDER:
      printf ("Invalid TRANSPONDER Control\n");
      return -1;
      break;
    case KR_STATION:
      printf ("Invalid RADIO Control\n");
      return -1;
      break;
  }

  //kr_address_debug_print (addr); 

  return 1;
}

void kr_unit_destroy (kr_client_t *client, kr_address_t *address) {

  if ((client == NULL) || (address == NULL)) {
    return;
  }
  if (address->path.unit == KR_COMPOSITOR) {
    kr_compositor_subunit_destroy (client, address);
  }
  if (address->path.unit == KR_MIXER) {
    kr_mixer_remove_portgroup (client, address->id.name);
  }
  if (address->path.unit == KR_TRANSPONDER) {
    kr_transponder_destroy (client, address->id.number);
  }
}

int kr_unit_control_data_type_from_address (kr_address_t *address, kr_unit_control_data_t *data_type) {

  switch (address->path.unit) {
    case KR_MIXER:
      *data_type = KR_FLOAT;
      return 1;
    case KR_COMPOSITOR:
      if ((address->control.compositor_control == KR_X) || (address->control.compositor_control == KR_Y) ||
          (address->control.compositor_control == KR_Z) || (address->control.compositor_control == KR_WIDTH) ||
          (address->control.compositor_control == KR_HEIGHT) || (address->control.compositor_control == KR_TICKRATE)) {
          *data_type = KR_INT32;
      } else {
        *data_type = KR_FLOAT;
      }  
      return 1;
    default:
      break;
  }

  return 0;
}

void kr_print_ebml (unsigned char *buffer, int len) {

  int i;

  i = 0;

  printf ("Raw EBML: \n");
  for (i = 0; i < len; i++) {
    printf ("%02X", buffer[i]);
  }
  printf ("\nEnd Raw EBML\n");
}

int kr_unit_control_set (kr_client_t *client, kr_unit_control_t *uc) {

  unsigned char *my_command;
  unsigned char *my_set_control;

  my_command = NULL;
  my_set_control = NULL;
  
  kr_unit_control_data_type_from_address (&uc->address, &uc->data_type);

  switch (uc->address.path.unit) {
    case KR_MIXER:
      switch (uc->address.path.subunit.mixer_subunit) {
        case KR_PORTGROUP:
          if (uc->address.control.portgroup_control == KR_VOLUME) {
            kr_mixer_set_control (client, uc->address.id.name, "volume", uc->value.real, uc->duration);
          } else {
            kr_mixer_set_control (client, uc->address.id.name, "crossfade", uc->value.real, uc->duration);
          }
          break;
        case KR_EFFECT:
          //printf( "eff id %d c %s val %f\n", uc->address.sub_id, effect_control_to_string(uc->address.control.effect_control), uc->value.real);
          kr_mixer_set_effect_control (client, uc->address.id.name, uc->address.sub_id, uc->address.sub_id2,
                                       effect_control_to_string(uc->address.control.effect_control),
                                       uc->value.real, uc->duration, EASEINOUTSINE);
          break;
      }
      break;
    case KR_COMPOSITOR:

      kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD, &my_command);
      kr_ebml2_start_element (client->ebml2, EBML_ID_KRAD_COMPOSITOR_CMD_SET_SUBUNIT, &my_set_control);
      kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_SUBUNIT, uc->address.path.subunit.compositor_subunit);
      kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_RADIO_SUBUNIT_ID_NUMBER, uc->address.id.number);
      kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_SUBUNIT_CONTROL, uc->address.control.compositor_control);
      if (uc->data_type == KR_FLOAT) {
        kr_ebml2_pack_float (client->ebml2, EBML_ID_KRAD_SUBUNIT_CONTROL, uc->value.real);
      }
      if (uc->data_type == KR_INT32) {
        kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_SUBUNIT_CONTROL, uc->value.integer);
      }
      if (uc->data_type == KR_STRING) {
        kr_ebml2_pack_string (client->ebml2, EBML_ID_KRAD_SUBUNIT_CONTROL, uc->value.string);
      }
      kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_SUBUNIT_CONTROL, uc->duration);
      kr_ebml2_pack_int32 (client->ebml2, EBML_ID_KRAD_SUBUNIT_CONTROL, 666);
      kr_ebml2_finish_element (client->ebml2, my_set_control);
      kr_ebml2_finish_element (client->ebml2, my_command);

      if (client->autosync == 1) {
        kr_client_sync (client);
      }

      break;
    case KR_STATION:
      break;
    case KR_TRANSPONDER:
      return -1;
      break;
  }

  return 0;
}

