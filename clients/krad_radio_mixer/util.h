#ifndef KRM_UTIL_H
#define KRM_UTIL_H
#include "kr_mixer.h"
void print_in_middle (WINDOW *win, int starty, int startx, int width, char *string, chtype color);
int krm_poll (kr_client_t *client, int timeout);
#endif