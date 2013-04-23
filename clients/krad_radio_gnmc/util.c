#include "util.h"

int sockeys[2];

void signal_recv (int sig) {

  if (sig == SIGWINCH) {
    write (sockeys[0], "R", 1);
  }

  signal (SIGWINCH, signal_recv);
}

void curses_init () {

  setlocale(LC_ALL,"");
  setlocale(LC_CTYPE, "UTF-8");

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

  nodelay (stdscr,TRUE);
  noecho ();
  cbreak ();
  set_escdelay (100);

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

int istop (screen_t *top, const char *name) {

  if (top != NULL) {

    if (!strcmp (top->name,name)) {
      return 1;
    }

  }

  return 0;
}

int kr_gnmc_poll (kr_client_t *client, int timeout) {


  /* poll stuff */
  struct pollfd pollfds[3];
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

  pollfds[2].fd = sockeys[1];
  pollfds[2].events = POLLIN;
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
        if (pollfds[n].revents & (POLLERR | POLLHUP | POLLNVAL) ) {
          // error on this pollfd, like disconnected suddently
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


          if (pollfds[n].fd == sockeys[1]) {
            return 3;

          }
          
        }
      }
    }
  }

  return 0;
}


void show_win_label (WINDOW *win, const char *name) {

  int i;
  int len;


  len = strlen (name);


  mvwprintw (win,1,1,name);

  mvwaddch (win,2,0,ACS_LTEE);

  for (i=1;i<=len;i++) {

    mvwaddch (win,2,i,ACS_HLINE);

  }

  mvwaddch (win,2,len+1,ACS_LRCORNER);
  mvwaddch (win,1,len+1,ACS_VLINE);
  mvwaddch (win,0,len+1,ACS_TTEE);


  return;
}


void s_to_hhmmss (char *timestr, int sec)
{

  int h,m,s;

  h = sec / 3600;
  m = (sec - h * 3600) / 60;
  s = (sec - h * 3600 - m * 60);

  sprintf (timestr, "%d hrs , %d min and %d sec",h,m,s);

  return;
}

int check_win_space (WINDOW *win, int minrows, int mincols) {

  int rows,cols;

  getmaxyx (win,rows,cols);

  if ( (rows < minrows) || (cols < mincols) ) {
    return 0;
  }
  else {
    return 1;
  }

}


void curses_nl (WINDOW *win, int x) {

  int y = getcury (win);

  wmove (win,++y,x);

  return;
}


void curses_win_focus (WINDOW *win, const char *name) {

  int y,x;

  y = getcury (win);
  x = getcurx (win);

  wattron (win,A_BOLD);
  box (win,0,0);
  show_win_label (win,name);
  wattroff (win,A_BOLD);

  wmove (win,y,x);

  return;
}

void curses_win_unfocus (WINDOW *win, const char *name) {

  int y,x;

  y = getcury (win);
  x = getcurx (win);
  
  wattroff (win,A_BOLD);
  box (win,0,0);
  show_win_label (win,name);

  wmove (win,y,x);

  return;
}

void curses_txt_focus (WINDOW *win) {

  
  wattron (win,A_BOLD);
  redrawwin (win);
  wattroff (win,A_BOLD);

  return;
}


void curses_delpan (PANEL *pan) {

  WINDOW *win;

  win = panel_window (pan);
  del_panel (pan);
  delwin (win);

  return;
}