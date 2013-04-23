#ifndef KRS_UTIL_H
#define KRS_UTIL_H
#include "krad_shell.h"

char** split (char *str, int *n);
void strdelch (char *str, int p);
void straddch (char *str, char c, int p);
void resize_handler ();
void print_in_middle (WINDOW *win, int starty, int startx, int width, char *string, chtype color);
void colors_init ();
int kr_shell_poll (kr_shell_t *kr_shell, int timeout);
kr_client_t *kr_connect_launch (char *sysname);
kr_shell_t *kr_shell_create (char *sysname);
void kr_shell_destroy (kr_shell_t *kr_shell);
void krad_shell_cmds ();
int iscmd (char *cmd,kr_shell_t *kr_shell);
void shell_nl (kr_shell_t *kr_shell);
void shell_delete (kr_shell_t *kr_shell);
void shell_print (char *str);
void shell_clear (void);
void krad_shell_help ();
void krad_shell_help_cmd(char *cmd, kr_shell_t *kr_shell);
void cpu_prompt(int *number, kr_shell_t *kr_shell);
void check_scroll (kr_shell_t *kr_shell);
void cmd_not_valid_print (char *cmd);
void shell_left (kr_shell_t *kr_shell);
void shell_right (kr_shell_t *kr_shell);

#endif
