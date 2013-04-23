#include "kr_gnmc.h"

int main (int argc, char **argv) {

	char *sysname;
	kr_client_t *client;
  int res;

	if (argc != 2) {
		fprintf (stderr,"Need 1 arg, station name \n");
		return 1;
	}

  if (krad_valid_host_and_port (argv[1])) {
    sysname = argv[1];
  } else {
    if (!krad_valid_sysname(argv[1])) {
      fprintf (stderr, "Invalid station sysname! \n");
      return 1;
    } else {
      sysname = argv[1];
    }
  }

	client = kr_client_create ("krad command line client");

  if (client == NULL) {
    fprintf (stderr, "Could create client \n");
    return 1;
  }

  if (!kr_connect (client, sysname)) {
    fprintf (stderr, "Could not connect to %s krad radio daemon \n", sysname);
    kr_client_destroy (&client);
    return 1;
  }

  curses_init ();
  
  if ( ( res = gnmc_run (client,sysname) ) < 0 ) {
  	curses_exit ();
    fprintf (stderr,
      "Something went wrong, probably not enough space to draw data \n");
  }
  else {
  	curses_exit ();
  }

  kr_disconnect (client);
  kr_client_destroy (&client);


	return 0;
}