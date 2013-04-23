#ifndef KR_GNMC_SCR_H
#define KR_GNMC_SCR_H
#include "gnmc.h"
screen_t *screen_alloc (const char *name);
void screen_destroy (screen_t *scr);
screen_t *screen_create (const char *name, int32_t ro, 
int32_t co, int32_t y, int32_t x);
void screen_focus_curr (screen_t **main_scrs, int32_t n);
void screen_set_focus_next (screen_t **main_scrs, int32_t n);
void screen_reset_cur (screen_t *screen);
void screen_clear (screen_t *screen);
void screen_redraw (scr_t *screen, int32_t ro, 
int32_t co, int32_t y, int32_t x);
void screen_repaint (screen_t *screen);
box_list_t *screen_boxes (screen_t *screen);
screen_t *screen_focused (screen_t **main_scrs, int32_t n);
void screen_focus_next_box (screen_t *screen);
void screen_focus_prev_box (screen_t *screen);
#endif