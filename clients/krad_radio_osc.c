#include "kr_client.h"
#include "krad_osc.h"

int main (int argc, char *argv[]) {

  kr_client_t *client;
  krad_osc_t *osc;
  char *sysname;
  int port;

  sysname = NULL;
  client = NULL;
  port = 0;

  if (argc < 2) {
    fprintf (stderr, "Specify a station sysname!\n");
    return 1;
  }

  if (argc < 3) {
    fprintf (stderr, "Specify a OSC port!\n");
    return 1;
  } else {
    port = atoi(argv[2]);
    if (!((port >= 0) && (port <= 65535))) {
      fprintf (stderr, "Invalid OSC Port %d\n", port);
      return 1;
    }
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
  
  osc = krad_osc_create (sysname);
  
  if (osc && krad_osc_listen (osc, port)) {
    printf ("OSC Listening on port %d\n", port);
    while (1) {
      usleep (25000);
    }
  }

  printf ("Shutting down OSC..\n");
  krad_osc_destroy (osc);
  printf ("OSC Shutdown\n");
  printf ("Disconnecting from %s..\n", sysname);
  kr_disconnect (client);
  printf ("Disconnected from %s.\n", sysname);
  printf ("Destroying client..\n");
  kr_client_destroy (&client);
  printf ("Client Destroyed.\n");
  
  return 0;

}
