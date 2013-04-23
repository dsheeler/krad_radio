#include "tcomp.h"

/* Completion Implementation  , a simple linear search */
static int* completion (kr_shell_t *kr_shell,int *n) {

  char **cmds = kr_shell->cmdlist;
  char *cmdbuffer = kr_shell->cmdbuffer;

  size_t csize = cmds_n;

  int i=0;
  int j=0;
  int *res = malloc (sizeof (int));
  int *temp;
  int length = strlen (cmdbuffer);

  for (i=0;i<csize;i++) {
    if ((!strncmp (cmdbuffer,cmds[i],length)) && (length<=strlen (cmds[i]))) {
      
      if (j == 0) {
        res[j] = i;
      }
        
      else {
        temp = realloc (res,sizeof(int)*(j+1));

        if (temp == NULL) {
          fprintf (stderr, "Realloc failure\n");
          exit (0);
        }
        else {
          res = temp;
          res[j] = i;
        }
      }
      j++;
    } 
  }

  *n = j;
  return res;
}


void tab_comp (kr_shell_t *kr_shell) {

  char *cmdbuffer = kr_shell->cmdbuffer;
  char **cmds = kr_shell->cmdlist;

  /* Our array of possible cmds matches */
  int *matches;
  int n;
  int i;
  if (cmdbuffer[0] != '\0') {

    matches = completion (kr_shell,&n);

    if (n == 1) {

      int y;
      strncpy (cmdbuffer,cmds[matches[0]],strlen (cmds[matches[0]])+1);
      y = getcury (stdscr);
      wmove (stdscr, y, 0);
      clrtoeol ();
      wmove (stdscr, y, 0);
      printw ("%s", kr_shell->sh_head);
      printw ("%s", cmdbuffer);
      kr_shell->pos = strlen (cmds[matches[0]]);


    }
    else {

      for (i=0;i<n;i++) {
        shell_print (cmds[matches[i]]);
      }
      shell_nl (kr_shell);
      printw ("%s", cmdbuffer);

    }

    free (matches); 
  }

  return;

}