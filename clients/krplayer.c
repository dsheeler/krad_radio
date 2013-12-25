#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "krad_player.h"

typedef struct kr_player_cli_St kr_player_cli_t;

struct kr_player_cli_St {
  /* krad_wayland_t *wayland; */
  kr_player_t *player;
  struct termios ttystate;
};

static int destroy = 0;

void signal_recv (int sig) {
  destroy = 1;
}

kr_player_cli_t *kr_player_cli_create (char *station, char *input) {

  kr_player_cli_t *kcp;

  kcp = calloc (1, sizeof (kr_player_cli_t));

  krad_system_init ();

  //char logfile[265];
  //sprintf (logfile, "%s/kr_player_%"PRIu64".log",
  //         getenv ("HOME"), krad_unixtime ());
  //krad_system_log_on (logfile);

  kcp->player = kr_player_create (station, input);
  if (kcp->player == NULL) {
    fprintf (stderr, "Could not create player :/\n");
    free (kcp);
    return NULL;
  }

  signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);

  tcgetattr (STDIN_FILENO, &kcp->ttystate);
  kcp->ttystate.c_lflag &= ~(ICANON | ECHO);
  kcp->ttystate.c_cc[VMIN] = 1;
  tcsetattr (STDIN_FILENO, TCSANOW, &kcp->ttystate);

  return kcp;
}

void kr_player_cli_destroy (kr_player_cli_t *kcp) {
  kcp->ttystate.c_lflag |= ICANON | ECHO;
  tcsetattr (STDIN_FILENO, TCSANOW, &kcp->ttystate);
  kr_player_destroy (&kcp->player);
  free (kcp);
}

void kr_player_cli_handle_input (kr_player_cli_t *kcp) {

  int ret;
  char buf[1];

  ret = read (STDIN_FILENO, buf, 1);

  if (ret != 1) {
    printf ("\nError read: %d\n", ret);
  }

  if (buf[0] == 'p') {
    kr_player_play (kcp->player);
  }
  if (buf[0] == 's') {
    kr_player_stop (kcp->player);
  }
  if (buf[0] == 'r') {
    kr_player_direction_set (kcp->player, REVERSE);
  }
  if (buf[0] == 'f') {
    kr_player_direction_set (kcp->player, FORWARD);
  }
  if (buf[0] == 't') {
    kr_player_seek (kcp->player, 666);
  }
  if (buf[0] == 'a') {
    kr_player_speed_set (kcp->player, kr_player_speed_get (kcp->player) - 0.1f);
  }
  if (buf[0] == 'd') {
    kr_player_speed_set (kcp->player, kr_player_speed_get (kcp->player) + 0.1f);
  }
  if (buf[0] == 'b') {
    kr_player_pause (kcp->player);
  }
  if (buf[0] == 'q') {
    destroy = 1;
  }
}

void kr_player_cli_check_input (kr_player_cli_t *kcp) {

  struct pollfd pollfds[1];
  int n;

  pollfds[0].fd = STDIN_FILENO;
  pollfds[0].events = POLLIN;

  n = poll (pollfds, 1, 0);

  if (n > 0) {
    kr_player_cli_handle_input (kcp);
    kr_player_cli_check_input (kcp);
  }
}

void kr_player_cli_status (kr_player_cli_t *kcp) {
  printf ("\r%s %s ",
          kr_player_playback_state_to_string (kr_player_playback_state_get (kcp->player)),
          kr_direction_to_string (kr_player_direction_get (kcp->player)));
  printf ("%0.3f%% ::", kr_player_speed_get (kcp->player));
  printf (" %"PRIi64" ", kr_player_position_get (kcp->player));
  fflush (stdout);
}

void kr_player_cli_run (kr_player_cli_t *kcp) {
  while (!destroy) {
    usleep (100000);
    kr_player_cli_status (kcp);
    kr_player_cli_check_input (kcp);
  }
  printf ("\n");
}

int main (int argc, char **argv) {


  kr_player_cli_t *kcp;

  if (argc < 3) {
    fprintf (stderr, "krplayer [station] [url]\n");
    return 1;
  }

  kcp = kr_player_cli_create (argv[1], argv[2]);
  if (kcp == NULL) {
    fprintf (stderr, "Could not create player\n");
    return 1;
  }

  kr_player_cli_run (kcp);

  kr_player_cli_destroy (kcp);

  printf ("kr_player test completed.\n");

  return 0;
}
