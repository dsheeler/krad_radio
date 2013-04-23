#include "krad_shell.h"


static kr_shell_t* shell_init (kr_shell_t *kr_shell, char *sysname);

int main (int argc, char *argv[]) {

  /* Shell Struct */
  kr_shell_t *kr_shell = NULL;

  /* Krad Vars */
  char *sysname;


  /* command vars */
  int c;
  char *cmdbuffer;
  char **cmdsplit;
  int n;  // Our command's argc
  int res;


  /* We need only a station name, nothing more, nothing less, for now at least */
  if (argc != 2) {
    krad_shell_help ();
    return (1);
  }

  /* let's check that sysname is acceptable */

  if (krad_valid_host_and_port (argv[1])) {
    sysname = argv[1];
  } else {
    if (!krad_valid_sysname (argv[1])) {
      fprintf (stderr, "Invalid station sysname!\n");
      return 1;
    } else {
      sysname = argv[1];
    }
  }

  /*  shell */
  kr_shell = shell_init (kr_shell,sysname);

  if (kr_shell == NULL) {
      fprintf (stderr, "Could not connect init shell\n");
      endwin ();
      return (1);
  }

  /* We want shell mode atm*/
  kr_shell->mode = 0;

  if (kr_shell->mode == 0) {

    cmdbuffer = kr_shell->cmdbuffer;

    /* Main loop to catch input */
    while (1) {

      refresh ();

      int number;

      int polres = kr_shell_poll (kr_shell,-1);


      switch (polres) {

        case 0:
          //shell_print ("timeout");
          continue;
        case -1:
          //shell_print ("Daemon is not running anymore!");
          usleep (1000*100);
          continue;
        case 1:
          c = getch ();
          break;
        case 2:
          kr_delivery_recv (kr_shell->client);
          kr_crate_t *crate;
          crate = NULL;
          kr_delivery_get (kr_shell->client, &crate);
          if (crate != NULL) {
            if (kr_uncrate_int (crate, &number)) {
              cpu_prompt (&number,kr_shell);
            }
            kr_crate_recycle (&crate);
          }
          continue;
        default:
          break;

      }
    


      /* let's check that it's not too long for our buffer*/
        if (strlen (cmdbuffer) > (CMD_SIZE - 1)) {
          endwin ();
          fprintf (stderr,"Buffer is full , cmd is too long , max 255 chars\n");
          exit (1);
        }


      /* This is crucial, Enter key has been pressed , we need to issue the command*/
      if (c == '\n') {

        /* resetting cmd buffer pos */
        kr_shell->pos = 0;
   
        /* let's check if we need scrolling */
        check_scroll (kr_shell);

        if (!(cmdbuffer[0] == '\0')){


          /* command string handling */
          cmdsplit = split (cmdbuffer,&n);

          /* add current cmdbuffer line to history */
          history_add (kr_shell);
          
          /* increment command number */
          kr_shell->cmds++;


        /* do we have at least one arg? */
        if (n > 0) { 

          /* Is this a valid command? */

          if (!iscmd (cmdsplit[0],kr_shell)) {
            cmd_not_valid_print (cmdsplit[0]);
          }

          /* let's handle clear command */
          if ((strncmp (cmdsplit[0], "clear", 5) == 0)) {
            wclear (stdscr);
          }

          else if ((strncmp (cmdsplit[0], "exit", 4) == 0) || (strncmp (cmdsplit[0], "quit", 4) == 0)) {
            break;
          }

          else if ((strncmp (cmdsplit[0], "launch", 6) == 0) || (strncmp (cmdsplit[0], "load", 4) == 0)) {
             kr_shell->client = kr_connect_launch (sysname);
          }

          else if ((strncmp (cmdsplit[0], "destroy", 7) == 0) || (strncmp (cmdsplit[0], "kill", 4) == 0)) {
            //kr_disconnect(&kr_shell->client);

            int ret = krad_radio_destroy (sysname);
            
            if (ret == 0) {
              shell_print ("Daemon shutdown");
            }
            if (ret == 1) {
              shell_print ("Daemon was killed");
            }
            if (ret == -1) {
              shell_print ("Daemon was not running");
            }

          }  

          /* command replay function (as requested :P) */
          else if ((strncmp (cmdsplit[0], "replay", 6) == 0) && (n == 2)) {

            krs_replay (kr_shell,cmdsplit);

          }

          /* command batch save function  */
          else if ((strncmp (cmdsplit[0], "save", 4) == 0) && (n == 3)) {

            krs_save (kr_shell,cmdsplit);
          }

           /* command batch run function  */
          else if ((strncmp (cmdsplit[0], "run", 3) == 0) && (n==2)) {

            krs_run (kr_shell,cmdsplit);

          }

          else if ((strncmp (cmdsplit[0], "sleep", 5) == 0) && (n==2)) {

            useconds_t ms = atoi (cmdsplit[1]) * 1000;
            usleep (ms);

          }

          /* help handling */
          else if ((strncmp (cmdsplit[0], "help", 4) == 0)) {
           
           if (n == 2) {

            if (!iscmd (cmdsplit[n-1],kr_shell)) {

              /* cmd not valid */
              cmd_not_valid_print (cmdsplit[n-1]);

            }
            /* cmd specific help */
            else {
              krad_shell_help_cmd (cmdsplit[n-1],kr_shell);
            }

          }
          else {
            krad_shell_cmds ();
          }

          }

          else {
            res = krad_shell_cmd (kr_shell,sysname,n,cmdsplit);
            if (res==1) {
              char errstr1[] = "Could not connect to ";
              char errstr2[] = " krad radio daemon";
              char *errstr = calloc (strlen (errstr1)+strlen (errstr2)+strlen (kr_shell->sysname) + 1,sizeof (char));
              sprintf (errstr,"%s%s%s",errstr1,kr_shell->sysname,errstr2);
              shell_print (errstr);
            }

          }

          free (cmdsplit);
        }



      } 
      else {
        history_reset_cur (kr_shell);
      }
   
        /* resetting and cleaning */
        n = 0;
        memset (cmdbuffer,'\0',CMD_SIZE);
        shell_nl (kr_shell);
   
        
        continue;
      }
   
      /* Delete Case */
      if ((c == 127) || (c == KEY_BACKSPACE))  {
        shell_delete (kr_shell);
        continue;
      }
   
     /* History */
   
      if (c == KEY_UP) {
        history_up (kr_shell);
        continue;
      }
   
      if (c == KEY_DOWN) {
        history_down (kr_shell);
        continue;
      }


      if (c == KEY_LEFT) {

        shell_left (kr_shell);
        continue;
      }

      if(c == KEY_RIGHT) {

        shell_right (kr_shell);
        continue;
      }

      /* It's time for some Tab Completion */
      if (c == 9) {
        
        tab_comp (kr_shell);

        continue;
      }

      /* CTRL+C */
      if (c == 3) {
        memset(cmdbuffer,'\0',CMD_SIZE);
        kr_shell->pos = 0;
        shell_nl (kr_shell);
        continue;
      }

      /* we catch all the ALT+char sequences */
      if (c==27) {

        int cc;
        cc = getch ();


        switch ( cc ) {

          /* command menu */
          case 'm':
            cmd_menu_gen (kr_shell);
            touchwin (stdscr);
            wrefresh (stdscr);
            shell_nl (kr_shell);
            printw (cmdbuffer);
            continue;
          /* exit shell */
          case 113:
            break;
          default:
            continue;
        }

        break;
      } 


      /* getting some infos */
      int x,y;
      getyx (stdscr, y, x);

      //if (c == ERR) {
      //  printw ("OK");
      //}

      //if (c == KEY_RESIZE) {
      //  printw ("OKr");
      //}

      if (c != ERR && c != KEY_RESIZE) {

        int curx,cury;
        curx = getcurx (stdscr);
        cury = getcury (stdscr);
        int maxx = getmaxx (stdscr);

        if ( curx >= maxx - 1) {
          kr_shell->loffset--;
          move (cury+1,0);
        }

        /* We write chars on our buffer at cursor position, like we see on screen*/
        straddch (cmdbuffer,c,kr_shell->pos);

  
        /* We update the screen accordingly */
        clrtoeol ();
        mvaddstr (y, x+1, &cmdbuffer[kr_shell->pos+1]);
        mvaddch (y,x,c);
        move (y,x+1);
        refresh ();
        kr_shell->pos++;

      }
      
    }

    /* End curses mode */
    endwin ();  
 

  }

  /* destroy shell structure and free memory */
  kr_shell_destroy (kr_shell);
  return 0;
}


static kr_shell_t* shell_init (kr_shell_t *kr_shell, char *sysname) {

  /* let's init the shell and connect to station! */
  kr_shell = kr_shell_create (sysname);

  if (kr_shell == NULL) {
    return NULL;
  }

  /* let's get broadcasts */
  kr_subscribe_all (kr_shell->client);

  /* Curses Init */

  setlocale(LC_ALL,"");

  (void) signal(SIGWINCH, resize_handler);

  /* Start curses mode      */
  initscr ();  
  start_color ();      
  colors_init ();
  noecho ();
  raw ();
  nodelay (stdscr,TRUE);

  /* We want scrolling */
  scrollok (stdscr, TRUE);

  /* We want arrows and stuff */
  keypad (stdscr, TRUE);

  attron (A_BOLD | COLOR_PAIR (4));

  printw ("KradRadio $hell");

  attroff (A_BOLD | COLOR_PAIR (4));

  /* resize sig 

  sigset_t mask;
  sigemptyset (&mask);
  sigaddset (&mask, SIGWINCH);

  if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
    perror ("sigprocmask error");
    exit (1);
  }

  kr_shell->sfd = signalfd (-1, &mask, 0);

  if (kr_shell->sfd < 0) {
    perror ("signalfd error");
    exit (1);
  }
*/
  /* Our shell header */
  char shhead[] = "$ ";
  char *cpu = calloc (7,sizeof (char));
  sprintf (cpu," 0%%");
  kr_shell->sh_head = calloc (strlen (shhead) + strlen (sysname) +  strlen (cpu) + 9,sizeof (char));
  sprintf (kr_shell->sh_head,"[%s ::%s ]%s",sysname,cpu,shhead);

  kr_shell->pos = 0;
  kr_shell->cmds = 0;
  
  memset (kr_shell->cmdbuffer,'\0',CMD_SIZE);

  kr_shell->cmdhelp = cmdhelp;
  kr_shell->cmdlist = cmds;
  kr_shell->cmd_defs = krad_shell_cmd_def;
  kr_shell->nbatches = 0;
  kr_shell->scroll = 0;
  kr_shell->loffset = 0;

  /* history init */
  history_init (kr_shell);

  shell_nl (kr_shell);

  return kr_shell;

}