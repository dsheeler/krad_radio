#include "util.h"

void shell_nl (kr_shell_t *kr_shell) {

  int y;
  y = getcury (stdscr);
  if (kr_shell->scroll) {
    scroll (stdscr);
    wmove (stdscr,y,0);
  }
  else {
  wmove (stdscr, y + 1, 0);
  }

  printw ("%s",kr_shell->sh_head);
  refresh ();

}

void shell_delete (kr_shell_t *kr_shell) {

  int x,y;
  getyx (stdscr, y, x);
  int maxx;


  if (kr_shell->loffset >= 0) {
    /* We don't want our prompt erased */
    if (x > (strlen (kr_shell->sh_head))) {
      kr_shell->pos--;
      strdelch (kr_shell->cmdbuffer,kr_shell->pos);
      mvdelch (y,x-1);
    }
  }
  else {

    if (x != 0) {
      kr_shell->pos--;
      strdelch (kr_shell->cmdbuffer,kr_shell->pos);
      mvdelch (y,x-1);
    } 
    else {
      maxx = getmaxx (stdscr);
      kr_shell->pos--;
      strdelch (kr_shell->cmdbuffer,kr_shell->pos);
      wmove (stdscr,y-1,maxx-1);
      mvdelch (y-1,maxx-1);
      kr_shell->loffset++;
    }

  }


}

void shell_left (kr_shell_t *kr_shell) {
  int x,y;
  getyx(stdscr, y, x);
  if (kr_shell->loffset >= 0) {
    if (x>strlen (kr_shell->sh_head)) {
      wmove (stdscr, y, x-1);
      kr_shell->pos--;        
    }
  }

  //TODO else case
}

void shell_right (kr_shell_t *kr_shell) {
  int x,y;
  getyx (stdscr, y, x);
  
  if (kr_shell->loffset >= 0) {
    if (x<(strlen (kr_shell->sh_head)+strlen (kr_shell->cmdbuffer))) {
      wmove (stdscr, y, x+1);
      kr_shell->pos++;
    }
  }

  //TODO else case
}

void shell_print (char *str)
{

  int y,max_y;
  y = getcury (stdscr);
  max_y = getmaxy (stdscr);

  if (y >= max_y-1) {
    scroll (stdscr);
    wmove (stdscr,y,0);
  }
  else {
    wmove (stdscr, y + 1, 0);
  }

  printw ("%s",str);

  refresh ();
}


char** split (char *str,int *n) {

  char *rest; 
  char *token;
  char *ptr = strdup (str);
  char **res = malloc (sizeof (char*)*(strlen (str)+1));
  int i = 0;

  while ((token = strtok_r (ptr, " ", &rest))) {
    res[i] = strndup (token,strlen (token)+1);
    ptr = rest;  
    i++;
  }

  *n = i;

  return res;

}

void strdelch (char *str, int p) {

memmove (&str[p],&str[p+1],strlen(str)-p);

}

void straddch (char *str, char c, int p) {

memmove (&str[p+1], &str[p], strlen (str)+1-p);
memmove (&str[p], &c, 1);

}

void resize_handler () {


  endwin ();
  doupdate ();
  resized = 1;

  //printf("\x1b[8;%d;%dt", nRows, nColumns);

  //kr_shell->has_resized = 1;

}

void colors_init () {

  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  init_pair(4, COLOR_BLUE, COLOR_BLACK);
  init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(6, COLOR_CYAN, COLOR_BLACK);
  init_pair(7, COLOR_WHITE, COLOR_BLACK);

}

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


int kr_shell_poll (kr_shell_t *kr_shell, int timeout) {

  void resize_handler (kr_shell_t *kr_shell);

  /* poll stuff */
  struct pollfd pollfds[2];
  int ret;
  int n;
  int nfds;
  int krad_fd;

  nfds = 0;

  krad_fd = kr_client_get_fd (kr_shell->client);

  if (krad_fd < 0) {
    // something is wrong
    return -1;
  }

  /*sigset_t mask;
  sigemptyset (&mask);
  sigaddset (&mask, SIGWINCH);

  if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
    perror ("sigprocmask error");
    exit (1);
  }

  int sfd = signalfd (-1, &mask, 0);

  if (sfd < 0) {
    perror ("signalfd error");
    exit (1);
  }
  */

  pollfds[0].fd = STDIN_FILENO;
  pollfds[0].events = POLLIN;
  nfds++;
  
  pollfds[1].fd = krad_fd;
  pollfds[1].events = POLLIN;
  nfds++;

  /*
  pollfds[2].fd = kr_shell->sfd;
  pollfds[2].events = POLLIN;
  nfds++;

  */

  ret = poll (pollfds, nfds, timeout);


  if (ret < 0) {
    // error
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

          /* resize! 
          if (pollfds[n].fd == sfd) {
            return 3;

          }*/
          
        }
      }
    }
  }

  return 0;
}

kr_client_t *kr_connect_launch (char *sysname) {
 
  kr_client_t *client;
  client = NULL;
  int ret;
  int timeout = 50;
 
  
  krad_radio_launch (sysname);

  client = kr_client_create ("Krad $hell Client");

  if (client == NULL) {
    fprintf (stderr, "Could create client\n");
    exit (1);
  }


  while (((ret = kr_connect (client, sysname)) != 1) && (timeout > 0)) {

    usleep(10000);

    timeout--;
  }

  
  if (ret!=1) {
    fprintf (stderr, "Could not connect to client\n");
    exit (1);
  }

  return client;
}

kr_shell_t *kr_shell_create (char *sysname) {
 
  kr_shell_t *kr_shell = calloc (1, sizeof (kr_shell_t));
 
  kr_shell->client = kr_connect_launch (sysname);
 
  if (kr_shell->client == NULL) {
    char errstr1[] = "Could not connect to ";
    char errstr2[] = " krad radio daemon";
    char *errstr = calloc (strlen (errstr1)+strlen (errstr2)+strlen (sysname) + 1,sizeof (char));

    sprintf (errstr,"%s%s%s",errstr1,sysname,errstr2);

    shell_print (errstr);
    free (errstr);
    free (kr_shell);
    return NULL;
  }
 
  kr_shell->sysname = strdup (sysname);

  kr_shell->has_resized = 0;
 
  return kr_shell;
}
 
void kr_shell_destroy (kr_shell_t *kr_shell) {
  kr_disconnect (kr_shell->client);
  free (kr_shell->sysname);
  free (kr_shell->history);
  free (kr_shell->batches);
  free (kr_shell);
}


void krad_shell_cmds () {

  attron (A_BOLD);
  shell_print ("Commands:");
  attroff (A_BOLD);
  shell_print ("");
  shell_print ("launch ls destroy uptime info tag tags stag remoteon remoteoff webon weboff oscon oscoff setrate getrate mix");
  shell_print ("");
  shell_print ("setdir lm ll lc tone input output rmport plug unplug map mixmap xmms2 noxmms2 receiver_on receiver_off link");
  shell_print ("");
  shell_print ("transmitter_on transmitter_off closedisplay display lstext rmtext addtest lssprites addsprite rmsprite");
  shell_print ("");
  shell_print ("setsprite comp setres setfps snap jsnap setport update play record capture");
  shell_print ("");
  shell_print ("addfx rmfx setfx");
  shell_print ("");
  attron (A_BOLD);
  shell_print ("If you need specific help, run help followed by a command name i.e. help ls");
  attroff (A_BOLD);
  shell_print ("");

}



int iscmd (char *cmd, kr_shell_t *kr_shell) {

  char **cmds = kr_shell->cmdlist;

  size_t csize = cmds_n;
  int i;

  for (i=0;i<csize;i++) {

    if (!strncmp (cmd,cmds[i],max (strlen (cmd)+1,strlen (cmds[i])+1))) {
      return 1;
    }

  }

  return 0;

}


void shell_clear (void) {
  clear ();
}

void krad_shell_help () {

  /* This gonna be an help one day */
  printf ("Usage: \n kr_shell stationname\n");

}

void krad_shell_help_cmd(char *cmd, kr_shell_t *kr_shell) {

  int pos;
  size_t csize = cmds_n;
  char **cmds = kr_shell->cmdlist;
  char **cmdhelp = kr_shell->cmdhelp;

  for (pos=0;pos<csize;pos++) {
    if (!strcmp (cmd,cmds[pos])) {
      break;
    }
  }

  shell_print (cmdhelp[pos]);

}

void cpu_prompt (int *number, kr_shell_t *kr_shell) {

  int x,y;
  getyx (stdscr, y, x);
  int offset = kr_shell->loffset;

  wmove (stdscr, y+offset, 0);
  printw ("%s",kr_shell->sh_head);

  char shhead[] = "$ ";
  
  if (*number < 10) {
    sprintf (kr_shell->sh_head,"[%s :: %d%% ]%s",kr_shell->sysname,*number,shhead);
  }

  else if (*number < 100) {
    sprintf (kr_shell->sh_head,"[%s :: %d%%]%s",kr_shell->sysname,*number,shhead);
  }

  else {
    sprintf (kr_shell->sh_head,"[%s ::%d%%]%s",kr_shell->sysname,*number,shhead);
  }

  wmove (stdscr,y,x);

  refresh ();
  return;

}

void check_scroll (kr_shell_t *kr_shell) {

  /* Do we need to scroll? */
  int y,max_y;
  y = getcury (stdscr);
  max_y = getmaxy (stdscr);

  if (y >= max_y-1) {
    kr_shell->scroll = 1;
  }
  else {
    kr_shell->scroll = 0;
  }

  return;
}

void cmd_not_valid_print (char *cmd) {

  char *errstr1 = " is not a valid command";
  char *errstr = calloc (strlen (errstr1)+strlen (cmd) + 1,sizeof (char));
  sprintf (errstr,"%s%s",cmd,errstr1);
  shell_print (errstr);
  free (errstr);
  return;
}