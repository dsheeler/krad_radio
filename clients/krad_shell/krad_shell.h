#ifndef KRS_KRAD_SHELL_H
#define KRS_KRAD_SHELL_H

typedef struct kr_shell_St kr_shell_t;
typedef struct kr_def_St kr_cdef_t;
typedef struct kr_batch_St kr_batch_t;

/* our command defaults struct */
struct kr_def_St {

  /* command name */
  char *name;
  /* command default */
  char *def;

};

#define CMD_SIZE 256 // Command buffer size
#define H_SIZE 64 // History array size 

#include "kr_client.h"
#include <locale.h>
#include <ncurses.h>
#include <menu.h>
#include <panel.h>
#include <form.h>
#include "history.h"
#include "tcomp.h"
#include "util.h"
#include "mixer.h"
#include "cmdmenu.h"
#include "batch.h"
#include "globals.h"



struct kr_shell_St {


  /* Krad station name */
  char *sysname;

  /* Our shell will have some modes , for the moment being 0 will be our "Shell Mode" */
  int mode;

  /* krad client */
  kr_client_t *client;

  /* krad response */
  kr_response_t *kr_response;

  /* number of cmds issued in our shell */
  int cmds;

  /* Our shell header */
  char *sh_head;

  /* Our cmd line buffer */
  char cmdbuffer[CMD_SIZE];

  /* Our position writing on cmdbuffer */
  int pos;

  /* history struct */
  history_t *history;

  /* this will be our array of batches */
  kr_batch_t **batches;

  /* has terminal been resized? */
  int has_resized;

  /* command help array */
  char **cmdhelp;

  /* command names array */
  char **cmdlist;

  /* array of command defaults */
  kr_cdef_t *cmd_defs;

  /* number of saved batches */
  unsigned int nbatches;

  /* scroll flag */
  unsigned int scroll;

  /* used to know when we are writing a long command which takes us down one or more lines in the shell */
  int loffset;

  /* resize signal file descriptor */
  int sfd;

  
};

struct kr_batch_St {

  /* our batch name */
  char *name;
  /* batch array of commands */
  char **cmds;
  /* number of commands*/
  int ncmds;

};


void shell_print (char *str);
 
kr_shell_t *kr_shell_create (char *sysname);
void kr_shell_destroy (kr_shell_t *kr_shell);

kr_client_t *kr_connect_launch (char *sysname);

int krad_shell_cmd (kr_shell_t *kr_shell , char *sysname ,int argc , char *argv[]);

void handle_response (kr_client_t *client);

void shell_nl_ (kr_shell_t *kr_shell);

void print_in_middle (WINDOW *win, int starty, int startx, int width, char *string, chtype color);

int my_poll (kr_shell_t *kr_shell, int timeout);

#endif

