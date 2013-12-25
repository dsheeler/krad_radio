#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "krad_player.h"
#include "krad_wayland.h"

typedef struct kr_player_wl_St kr_player_wl_t;

struct kr_player_wl_St {
  kr_wayland *wayland;
  kr_wayland_window *output_window;
  kr_wayland_window *zoom_window;
  kr_wayland_window *control_window;
  kr_player_t *player;
  struct termios ttystate;
};

static int destroy = 0;

void signal_recv (int sig) {
  destroy = 1;
}

int display_cb(void *user, kr_wayland_event *event) {

  int ret;

  kr_player_wl_t *kwp;

  kwp = (kr_player_wl_t *)user;

/*

  if (kwp->frames > kwp->dframes) {
    pos = (kwp->dframes % kwp->framebufsize);
    memcpy (event->frame_event.buffer,
            kwp->rgba + (pos * kwp->frame_size),
            kwp->frame_size);
    kwp->dframes++;
*/


  ret = krad_player_get_frame(kwp->player, event->frame_event.buffer);


  return ret;
}

int window_cb(void *user, kr_wayland_event *event) {
  switch (event->type) {
    case KR_WL_FRAME:
      return display_cb(user, event);
    case KR_WL_POINTER:
      break;
    case KR_WL_KEY:
      break;
  }
  return 0;
}

kr_player_wl_t *kr_player_wl_create (char *input) {

  kr_player_wl_t *kwp;
  kr_wayland_window_params window_params;

  kwp = calloc (1, sizeof (kr_player_wl_t));

  krad_system_init ();
  
  //char logfile[265];
  //sprintf (logfile, "%s/kr_player_%"PRIu64".log",
  //         getenv ("HOME"), krad_unixtime ());  
  //krad_system_log_on (logfile);

  kwp->wayland = kr_wayland_create();


  kwp->player = kr_player_create_custom_cb(input);
  if (kwp->player == NULL) {
    fprintf (stderr, "Could not create player :/\n");
    free (kwp);
    return NULL;
  }

  window_params.width = 1280;
  window_params.height = 720;
  window_params.callback = window_cb;
  window_params.user = kwp;

  kwp->output_window = kr_wayland_window_create(kwp->wayland, &window_params);

  signal (SIGINT, signal_recv);
  signal (SIGTERM, signal_recv);

  tcgetattr (STDIN_FILENO, &kwp->ttystate);
  kwp->ttystate.c_lflag &= ~(ICANON | ECHO);
  kwp->ttystate.c_cc[VMIN] = 1;
  tcsetattr (STDIN_FILENO, TCSANOW, &kwp->ttystate);

  return kwp;
}

void kr_player_wl_destroy (kr_player_wl_t *kwp) {
  kr_wayland_window_destroy(&kwp->output_window);
  kr_wayland_destroy(&kwp->wayland);
  kwp->ttystate.c_lflag |= ICANON | ECHO;
  tcsetattr (STDIN_FILENO, TCSANOW, &kwp->ttystate);
  kr_player_destroy (&kwp->player);
  free (kwp);
}

void kr_player_wl_handle_input (kr_player_wl_t *kwp) {

  int ret;
  char buf[1];
  
  ret = read (STDIN_FILENO, buf, 1);

  if (ret != 1) {
    printf ("\nError read: %d\n", ret);
  }
  
  if (buf[0] == 'p') {
    kr_player_play (kwp->player);
  }
  if (buf[0] == 's') {
    kr_player_stop (kwp->player);
  }
  if (buf[0] == 'r') {
    kr_player_direction_set (kwp->player, REVERSE);
  }   
  if (buf[0] == 'f') {
    kr_player_direction_set (kwp->player, FORWARD);
  }
  if (buf[0] == 't') {
    kr_player_seek (kwp->player, 666);
  }
  if (buf[0] == 'a') {
    kr_player_speed_set (kwp->player, kr_player_speed_get (kwp->player) - 0.1f);
  }
  if (buf[0] == 'd') {
    kr_player_speed_set (kwp->player, kr_player_speed_get (kwp->player) + 0.1f);
  }  
  if (buf[0] == 'b') {
    kr_player_pause (kwp->player);
  }
  if (buf[0] == 'q') {
    destroy = 1;
  }
}

void kr_player_wl_status (kr_player_wl_t *kwp) {
  printf ("\r%s %s ",
          kr_player_playback_state_to_string (kr_player_playback_state_get (kwp->player)),
          kr_direction_to_string (kr_player_direction_get (kwp->player)));
  printf ("%0.3f%% ::", kr_player_speed_get (kwp->player));
  printf (" %"PRIi64" ", kr_player_position_get (kwp->player));
  fflush (stdout);
}

void kr_player_wl_run (kr_player_wl_t *kwp) {

  struct pollfd pollfds[2];
  int n;

  while (!destroy) {

    pollfds[0].fd = STDIN_FILENO;
    pollfds[0].events = POLLIN;
    pollfds[1].fd = kr_wayland_get_fd(kwp->wayland);
    pollfds[1].events = POLLIN;

    n = poll (pollfds, 2, 10);

    if (n > 0) {
      if (pollfds[0].revents) {
        kr_player_wl_status (kwp);
        kr_player_wl_handle_input (kwp);
      }
      if (pollfds[1].revents) {
        kr_wayland_process(kwp->wayland);   
      }
    }
    kr_player_wl_status (kwp);
  }
  printf ("\n");
}

int main (int argc, char **argv) {


  kr_player_wl_t *kwp;

  if (argc < 2) {
    fprintf (stderr, "krplayer [url]\n");
    return 1;
  }

  kwp = kr_player_wl_create (argv[1]);
  if (kwp == NULL) {
    fprintf (stderr, "Could not create player\n");
    return 1;
  }

  kr_player_wl_run (kwp);
  
  kr_player_wl_destroy (kwp);

  printf ("kr_player test completed.\n");

  return 0;
}
