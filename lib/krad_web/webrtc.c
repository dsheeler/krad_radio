#include "krad_interweb.h"

void kr_webrtc_create_or_join(kr_iws_client_t *client) {
  char json[256];
  int i;

  if (client->webrtc_client.active == 0) {
    client->server->webrtc_server.num_clients++;
    client->webrtc_client.active = 1;
  }
  if (client->server->webrtc_server.num_clients == 1) {
    snprintf(json, sizeof(json), "[{\"com\":\"rtc\", \"ctrl\":\"created\"}]");
    interweb_ws_pack(client, (uint8_t *)json, strlen(json));
  }
  snprintf(json, sizeof(json), "[{\"com\":\"rtc\",\"ctrl\":\"joined\"}]");
  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void kr_webrtc_message(kr_iws_client_t *client, char *message) {
  char json[4096];
  char mod_message[4096];
  if (message[0] == '{') {
    strncpy(mod_message,message,4096);
  } else {
    snprintf(mod_message, 4096, "\"%s\"", message);
  }
  snprintf(json, sizeof(json), "[{\"com\":\"rtc\",\"ctrl\":\"message\","
   "\"message\":%s}]", mod_message);
  interweb_ws_pack(client, (uint8_t *)json, strlen(json));
}

void kr_webrtc_disconnect_client(kr_iws_client_t *client) {
  int i;

  if (client->webrtc_client.active == 1) {
    client->server->webrtc_server.num_clients--;
    client->webrtc_client.active = 0;
    for (i = 0; i < KR_IWS_MAX_CLIENTS; i++) {
      if (client->server->clients[i].webrtc_client.active == 1) {
        kr_webrtc_message(&client->server->clients[i], "bye");
      }
    }
  }
}


