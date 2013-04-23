#include "cmdmenu.h"

void cmd_menu_gen (kr_shell_t *kr_shell) {

  ITEM **items;
  int c;        
  MENU *menu;
  int n_choices, i;
  ITEM *cur_item;
  WINDOW *menu_win;

  char *cmdbuffer = kr_shell->cmdbuffer;

  char **cmds = kr_shell->cmdlist;
  kr_cdef_t *krad_shell_cmd_def = kr_shell->cmd_defs;

  int ro,co;
  getmaxyx (stdscr, ro, co);
  int max_men_y;
  int max_men_x;
  
  n_choices = cmds_n;
  items = (ITEM **)calloc (n_choices + 1, sizeof (ITEM *));

  /* adding command items to menu */
  for (i = 0; i < n_choices; ++i) {
    items[i] = new_item (cmds[i], cmds[i]);
  }

  items[n_choices] = (ITEM *)NULL;

  /* menu init */
  menu = new_menu ((ITEM **)items);

  /* menu main window init */
  menu_win = newwin (14, 60, 4, 4);

  /* enabling keys on menu window */
  keypad (menu_win, TRUE);
  
  /* setup menu win and subwin */
  set_menu_win (menu, menu_win);
  set_menu_sub (menu, derwin (menu_win, 10, 50, 3, 3));

  /* we want 2 menu columns with 10 items each */
  set_menu_format (menu, 10, 3);

  /* Set menu option not to show the description */
  menu_opts_off (menu, O_SHOWDESC);

  /* Set menu mark to the string "> " */
  set_menu_mark (menu, "> ");

  /* Print a border around the main window and print a title */
  box (menu_win, 0, 0);
  
  /* print the menu title, centered , bold and blue */
  print_in_middle (menu_win, 1, 0, 60, "Krad Shell Commands Menu", COLOR_PAIR (4));
  
  /* adding menu header */
  mvwaddch (menu_win, 2, 0, ACS_LTEE);
  mvwhline (menu_win, 2, 1, ACS_HLINE, 58);
  mvwaddch (menu_win, 2, 59, ACS_RTEE);
  refresh ();
  
  /* Post the menu */
  post_menu (menu);

  /* centering stuff */
  getmaxyx (menu_win, max_men_y, max_men_x);
  mvwin (menu_win,(ro/2)-(max_men_y/2),co/2-max_men_x/2);
  wrefresh (menu_win);
  refresh ();

  while ((c = wgetch (menu_win)) != KEY_F(1)) {
    switch (c) { 

      case KEY_DOWN:
        menu_driver (menu, REQ_DOWN_ITEM);
        break;
      case KEY_UP:
        menu_driver (menu, REQ_UP_ITEM);
        break;
      case KEY_LEFT:
        menu_driver (menu, REQ_LEFT_ITEM);
        break;
      case KEY_RIGHT:
        menu_driver (menu, REQ_RIGHT_ITEM);
        break;
      case KEY_NPAGE:
        menu_driver (menu, REQ_SCR_DPAGE);
        break;
      case KEY_PPAGE:
        menu_driver (menu, REQ_SCR_UPAGE);
        break;
    }

    if (c == 27) {
      break;
    }

    if (c == 10) {

      cur_item = current_item (menu);
      memset (cmdbuffer,'\0',CMD_SIZE);
      int defsize = cmd_defs_n;

      for (i=0;i<defsize;i++) { 
        if (!strcmp (cur_item->name.str,krad_shell_cmd_def[i].name)) {
          sprintf (cmdbuffer,"%s %s",krad_shell_cmd_def[i].name,krad_shell_cmd_def[i].def);
          kr_shell->pos = strlen (cmdbuffer);
          break;
        }
      }

      /* if command default is not defined we simply print the command name without args*/
      if (cmdbuffer[0] == '\0') {
        sprintf (cmdbuffer,"%s",cur_item->name.str);
        kr_shell->pos = strlen (cmdbuffer);
      }

      break;
        
    }

    wrefresh (menu_win);       
  } 

  /* Unpost and free all the memory taken up */

  unpost_menu (menu);
  free_menu (menu);

  for (i = 0; i < n_choices; ++i) {
    free (items[i]);
  }

  werase (menu_win);
  werase (menu_sub (menu));
  wrefresh (menu_win);
  wrefresh (menu_sub (menu));
  delwin (menu_win);
  delwin (menu_sub (menu));
}