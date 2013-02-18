#include "kr_client.h"
#include "krad_http.h"
#include "krad_websocket.h"

int main (int argc, char *argv[]) {

  kr_client_t *client;
  krad_websocket_t *websocket;
  krad_http_t *http;
  char *sysname;
  int port[2];
  char buf;

  sysname = NULL;
  client = NULL;
  http = NULL;
  websocket = NULL;
  port[0] = 0;
  port[1] = 0;

  if (argc < 2) {
    fprintf (stderr, "Specify a station sysname!\n");
    return 1;
  }

  if (argc < 3) {
    fprintf (stderr, "Specify a HTTP port!\n");
    return 1;
  } else {
    port[0] = atoi(argv[2]);
    if (!((port[0] >= 0) && (port[0] <= 65535))) {
      fprintf (stderr, "Invalid HTTP Port %d\n", port[0]);
      return 1;
    }
  }
  
  if (argc < 4) {
    fprintf (stderr, "Specify a Websocket port!\n");
    return 1;
  } else {
    port[1] = atoi(argv[3]);
    if (!((port[1] >= 0) && (port[1] <= 65535))) {
      fprintf (stderr, "Invalid Websocket Port %d\n", port[1]);
      return 1;
    }
  }
  
  if (port[0] == port[1]) {
    fprintf (stderr, "Must be different ports...");
    return 1;
  }
  
  if (krad_valid_host_and_port (argv[1])) {
    sysname = argv[1];
  } else {
    if (!krad_valid_sysname(argv[1])) {
      fprintf (stderr, "Invalid station sysname!\n");
      return 1;
    } else {
      sysname = argv[1];
    }
  }

  client = kr_client_create ("krad simple client");

  if (client == NULL) {
    fprintf (stderr, "Could create client\n");
    return 1;
  }

  if (!kr_connect (client, sysname)) {
    fprintf (stderr, "Could not connect to %s krad radio daemon\n", sysname);
    kr_client_destroy (&client);
    return 1;
  }

  printf ("Connected to %s!\n", sysname);

  http = krad_http_server_create ( port[0], port[1], NULL, NULL, NULL );
  websocket = krad_websocket_server_create ( sysname, port[1] );

  if ((http != NULL) && (websocket != NULL)) {
    printf ("HTTP Listening on port %d\n", port[0]);
    printf ("Websockets Listening on port %d\n", port[1]);
    printf ("Press enter key to exit...\n");
    if (read (STDIN_FILENO, &buf, 1) != 1) {
      fprintf (stderr, "Hrm weird..\n");
    }
  }
  
  if (http != NULL) {
    printf ("Destroying HTTP..\n");
    krad_http_server_destroy (http);
  }
  if (websocket != NULL) {
    printf ("Destroying Websocket..\n");
    krad_websocket_server_destroy (websocket);
  }
  
  printf ("Disconnecting from %s..\n", sysname);
  kr_disconnect (client);
  printf ("Disconnected from %s.\n", sysname);
  printf ("Destroying client..\n");
  kr_client_destroy (&client);
  printf ("Client Destroyed.\n");
  
  return 0;

}
