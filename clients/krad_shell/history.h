#include "krad_shell.h"
#ifndef KRS_HISTORY_H
#define KRS_HISTORY_H



typedef struct kr_shell_history_St history_t;

/* Our History Struct */

struct kr_shell_history_St {

  /* history array of cmd strings */
  char **his;
  /* history array current size */
  int size;
  /* history browsing cursor */
  int cur;
  /* last element flag */
  int last;

};



void history_print (kr_shell_t *kr_shell,int k);
void history_init (kr_shell_t *kr_shell);
void history_add (kr_shell_t *kr_shell);
void history_reset_cur (kr_shell_t *kr_shell);
void history_up (kr_shell_t *kr_shell);
void history_down (kr_shell_t *kr_shell);

#endif