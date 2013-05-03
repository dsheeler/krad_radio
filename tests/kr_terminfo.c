#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>

/* gcc -Wall kr_terminfo.c -o kr_terminfo */

static void sig_winch_handler (int sig) {
  printf ("sig_winch_handler!\n");
}

int main (int argc, char **argv) {

  struct winsize sz;

  signal (SIGWINCH, sig_winch_handler);

  while (1) {

    ioctl (STDIN_FILENO, TIOCGWINSZ, &sz);
    printf ("Screen width: %i  Screen height: %i\n",
            sz.ws_col, sz.ws_row);

    usleep (50000);
  }
  return 0;
}
