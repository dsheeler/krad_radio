/* Krad Radio ncurses Mixer Interface */
#include "kr_mixer.h"

void curses_init () {

	/* Start curses mode */
  initscr ();  
  start_color (); 

  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  init_pair(4, COLOR_BLUE, COLOR_BLACK);
  init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(6, COLOR_CYAN, COLOR_BLACK);
  init_pair(7, COLOR_WHITE, COLOR_BLACK);
  init_pair(8, COLOR_WHITE, COLOR_RED);


  noecho ();
  raw ();
  set_escdelay (100);
  nodelay (stdscr,TRUE);

  /* We want scrolling */
  scrollok (stdscr, TRUE);

  /* We want arrows and stuff */
  keypad (stdscr, TRUE);

  curs_set (0);

  return;
}

void curses_exit () {


  curs_set (1);
	endwin ();

}

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
  
  if ((res = mixer_run (client))  < 0) {
  	curses_exit ();
    fprintf (stderr,"Something went wrong with the mixer \n");
  }
  else {

  	curses_exit ();
  
  }

  kr_disconnect (client);
  kr_client_destroy (&client);

	return 0;
}

