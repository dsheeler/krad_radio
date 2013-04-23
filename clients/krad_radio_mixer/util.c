#include "util.h"

void print_in_middle (WINDOW *win, int starty, int startx, int width, char *string, chtype color) {

  int length, x, y;
  float temp;

  if (win == NULL) {
    win = stdscr;
  }

  getyx (win, y, x);

  if (startx != 0) {
    x = startx;
  }

  if(starty != 0) {
    y = starty;
  }

  if(width == 0) {
    width = 80;
  }

  length = strlen (string);
  temp = (width - length)/ 2;
  x = startx + (int)temp;
  wattron (win, color | A_BOLD);
 
  mvwprintw (win, y, x, "%s", string);
  wattroff (win, color | A_BOLD);
  
  refresh ();
}

int krm_poll (kr_client_t *client, int timeout) {


  /* poll stuff */
  struct pollfd pollfds[2];
  int ret;
  int n;
  int nfds;
  int krad_fd;

  nfds = 0;

  krad_fd = kr_client_get_fd (client);

  if (krad_fd < 0) {
    // something is wrong
    return -1;
  }

  pollfds[0].fd = STDIN_FILENO;
  pollfds[0].events = POLLIN;
  nfds++;
  
  pollfds[1].fd = krad_fd;
  pollfds[1].events = POLLIN;
  nfds++;

  ret = poll (pollfds, nfds, timeout);


  if (ret < 0) {
    // error
    if (errno == EINTR) {
      return -2;
    }
    return -1;
  }
  
  if (ret == 0) {
    // timeout
    return 0;
  }
  
  if (ret > 0) {
    for (n = 0; n < nfds; n++) {
      if (pollfds[n].revents) {
        if (pollfds[n].revents & POLLERR) {
          // error on this pollfd, like disconnected suddently
          return -1;
        }
        
        if (pollfds[n].revents & POLLHUP) {
          // disconnected / hangup
          return -1;
        }
 
        if (pollfds[n].revents & POLLIN) {
          // we have input, deal with it
          
          if (pollfds[n].fd == STDIN_FILENO)  {
            return 1;
          }
          
          if (pollfds[n].fd == krad_fd) {
            return 2;

          }

          
        }
      }
    }
  }

  return 0;
}