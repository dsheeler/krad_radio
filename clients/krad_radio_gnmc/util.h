#ifndef KR_GNMC_UTIL_H
#define KR_GNMC_UTIL_H
#include "gnmc.h"
extern int sockeys[2];
void curses_init (void);
void curses_exit (void);
int kr_gnmc_poll (kr_client_t *client, int timeout);
void show_win_label (WINDOW *win, const char *name);
void s_to_hhmmss (char *timestr, int sec);
int check_win_space (WINDOW *win, int minrows, int mincols);
void curses_nl (WINDOW *win, int x);
void curses_win_focus (WINDOW *win, const char *name);
void curses_win_unfocus (WINDOW *win, const char *name);
void curses_txt_focus (WINDOW *win);
void curses_delpan (PANEL *pan);
void signal_recv (int sig);
int istop (screen_t *top, const char *name);
#endif