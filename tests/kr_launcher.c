#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* gcc -Wall kr_launcher.c -o kr_launcher */

typedef struct kr_launcher_St kr_launcher_t;

struct kr_launcher_St {
  int32_t sd;
};

int32_t kr_launcher_destroy (kr_launcher_t **launcher);


static void kr_launcher_run_cmd (char *cmd[]) {

  int32_t ret;
  int32_t err;
  char errstr[256];

  ret = execvp (cmd[0], cmd);
  
  if (ret == -1) {
    err = errno;
    strerror_r (err, errstr, sizeof(errstr));
    fprintf (stderr, "Launch error: %s\n", errstr);
  }
}

void kr_launcher_loop (kr_launcher_t *launcher) {

  int32_t ret;
  char input[1024];
  struct pollfd lsd[1];

  fprintf (stderr, "Launcher hello: %d\n", launcher->sd);

  memset (input, 0, sizeof(input));

  while (1) {

    lsd[0].events = POLLIN;
    lsd[0].fd = launcher->sd;

    ret = poll (lsd, 1, -1);

    if (ret != 1) {
      fprintf (stderr, "poll failure\n");
      break;
    }
    if (lsd[0].revents & POLLERR) {
      fprintf (stderr, "Got poll err\n");
      break;
    }
    if (lsd[0].revents & POLLHUP) {
      fprintf (stderr, "Got poll POLLHUP\n");
      fprintf (stderr, "Launcher happy goodbye\n");
      kr_launcher_destroy (&launcher);
      exit (0);
    }
    if (!(lsd[0].revents & POLLIN)) {
      fprintf (stderr, "Did NOT get POLLIN\n");
      break;
    }
    
    if (lsd[0].revents & POLLIN) {
      fprintf (stderr, "Got pollin!\n");
      ret = read (lsd[0].fd, input, sizeof(input));
      fprintf (stderr, "read %d!\n", ret);
      if (0) {
        kr_launcher_run_cmd (NULL);
      }
    }
  }

  fprintf (stderr, "Launcher sad goodbye\n");

  kr_launcher_destroy (&launcher);
}

int32_t kr_launcher_destroy (kr_launcher_t **launcher) {
  if ((launcher != NULL) && (*launcher != NULL)) {
    close ((*launcher)->sd);
    free (*launcher);
    *launcher = NULL;
    return 0;
  }
  return -1;
}

void kr_launcher_start (kr_launcher_t *controller, kr_launcher_t *launcher) {

  pid_t sid;
  pid_t pid;
  int32_t flags;
  /* FILE *refp; */

  pid = fork ();

  if (pid < 0) {
    fprintf (stderr, "fork() failure\n");
    exit (1);
  }

  if (pid > 0) {
    kr_launcher_destroy (&launcher);
    if (waitpid(pid, NULL, 0) != pid) {
      fprintf (stderr, "waitpid() failure\n");
      exit (1);
    }
    return;
  }
  
  kr_launcher_destroy (&controller);

  umask (0);
 
  sid = setsid ();
  
  if (sid < 0) {
    exit (1);
  }

  if ((chdir("/")) < 0) {
    exit (1);
  }

  pid = fork();

  if (pid < 0) {
    fprintf (stderr, "fork() failure\n");
    exit (1);
  }

  if (pid > 0) {
    exit (0);
  }
  
  /*
  refp = freopen ("/dev/null", "r", stdin);
  if (refp == NULL) {
    exit(5);
  }  
  refp = freopen ("/dev/null", "w", stdout);
  if (refp == NULL) {
    exit(6);
  }  
  refp = freopen ("/dev/null", "w", stderr);  
  if (refp == NULL) {
    exit(7);
  }
  */

  flags = fcntl (launcher->sd, F_GETFD);
  if (flags == -1) {
    fprintf (stderr, "fcntl() F_GETFD failure\n");
  }
  flags |= FD_CLOEXEC;
  if (fcntl(launcher->sd, F_SETFD, flags) == -1) {
    fprintf (stderr, "fcntl() F_SETFD failure\n");
  }
  
  kr_launcher_loop (launcher);
}

kr_launcher_t *kr_launcher_create () {

  kr_launcher_t *launcher;
  kr_launcher_t *launcher_controller;
  int sockets[2];

  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets)) {
    fprintf (stderr, "Krad System: can't socketpair errno: %d", errno);
    return NULL;
  }

  launcher = calloc (1, sizeof(kr_launcher_t));
  launcher_controller = calloc (1, sizeof(kr_launcher_t));
  
  launcher->sd = sockets[0];
  launcher_controller->sd = sockets[1];
  
  kr_launcher_start (launcher_controller, launcher);
  
  return launcher_controller;
}

void kr_launcher_launch (kr_launcher_t *launcher, char *cmd[]) {

  int i;
  int pos;
  char output[512];

  pos = 0;
  
  for (i = 0; cmd[i] != NULL; i++) {
    pos += snprintf (output + pos, (sizeof(output) - 2) - pos,
                     "%s,", cmd[i]);
    if (pos >= (sizeof(output) - 2)) {
      return;
    }
  }
  pos += snprintf (output + pos, sizeof(output) - pos,
                   "\n");

  send (launcher->sd, output, pos, 0);
}

int main (int argc, char **argv) {

  kr_launcher_t *launcher;

  launcher = kr_launcher_create ();

  fprintf (stderr, "Launcher controller hello: %d\n", launcher->sd);

  usleep (500000);
  
  char *cmd[] = { "ls", "-l", NULL };
  kr_launcher_launch (launcher, cmd);

  usleep (500000);

  kr_launcher_launch (launcher, cmd);

  usleep (5000000);

  kr_launcher_destroy (&launcher);

  usleep (500000);

  return 0;
}
