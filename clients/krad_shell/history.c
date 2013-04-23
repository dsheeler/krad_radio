#include "history.h"


void history_print (kr_shell_t *kr_shell,int k) {

  int y;
  char *cmdbuffer = kr_shell->cmdbuffer;
  history_t *history = kr_shell->history;
  char **his = history->his;
  y = getcury (stdscr);
  wmove (stdscr, y, strlen(kr_shell->sh_head));
  clrtoeol ();
  if (k != -1) {
    strncpy (cmdbuffer,his[k],strlen(his[k])+1);
    printw ("%s",his[k]);
  }
  else {
    memset (kr_shell->cmdbuffer,'\0',CMD_SIZE);
    kr_shell->pos = 0;
  }
  refresh ();

}


void history_init (kr_shell_t *kr_shell) {

/* History struct init */
history_t *history = calloc (1,sizeof (history_t));
kr_shell->history = history;

/* History array init */
history->his = calloc (H_SIZE,sizeof (char*));
history->size = H_SIZE;
history->cur = 0;
history->last = 0;

return;

}

void history_add (kr_shell_t *kr_shell) {

  char *cmdbuffer = kr_shell->cmdbuffer;
  history_t *history = kr_shell->history;
  int h = kr_shell->cmds; 

  /* if we history is full we are gonna allocate some more memory */
  if (h == history->size-1) {

    history->size = history->size * 2;
    history->his = realloc (history->his,sizeof (char*)*history->size);

  }

  history->his[h] = strndup (cmdbuffer,strlen (cmdbuffer)+1);

  history->cur = h;
  history->cur++;

  return;


}

void history_reset_cur (kr_shell_t *kr_shell) {

  history_t *history = kr_shell->history;
  int h = kr_shell->cmds; 

  history->cur = h;

  return;
}

void history_up (kr_shell_t *kr_shell) {


  history_t *history = kr_shell->history;
  char **his = history->his;
  int h = kr_shell->cmds;


  history->last = 0;
   
  if (history->cur > 0) {
    history->cur--;
  }      
   
  if (his[history->cur] == NULL) {
    return;
  }

  if (history->cur == h-1) {
    history->last++;
  }
   
  history_print (kr_shell,history->cur);
  kr_shell->pos = strlen (his[history->cur]);
  return;

}

void history_down (kr_shell_t *kr_shell) {

  history_t *history = kr_shell->history;
  char **his = history->his;
  int h = kr_shell->cmds;

  if (his[history->cur] == NULL) {
    return;
  }

  if (history->cur < h-1) {
    history->cur++;
  }

  /* if I'm at the and of the history and got nothing more to show I should clear the line */
  if ((history->cur == h-1)) {

    if (!history->last) {
      history->last++;
      history_print (kr_shell,history->cur);
      kr_shell->pos = strlen (his[history->cur]);
    }
    else {
      history_print (kr_shell,-1);
      history->cur++;
    }

  }
  else {
    history_print (kr_shell,history->cur);
    kr_shell->pos = strlen (his[history->cur]);
  }

  return;

}