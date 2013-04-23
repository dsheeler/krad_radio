#include "screen.h"

screen_t *screen_alloc (const char *name) {

	screen_t *scr;

	scr = calloc (1, sizeof (screen_t));

	scr->name = strdup (name);
	scr->mainp = NULL;
	scr->focus = 0;
	scr->boxes = NULL;
	scr->fbox = NULL;

	return scr;
}


void screen_destroy (screen_t *scr) {

	if (scr->boxes != NULL) {
		box_rmall (scr->boxes);
		free (scr->boxes);
	}

	free (scr->name);
	curses_delpan (scr->mainp);
	free (scr);

	return;
}


screen_t *screen_create (const char *name, int32_t ro, 
  int32_t co, int32_t y, int32_t x) {

	screen_t *screen;
	WINDOW *win;

	screen = screen_alloc (name);
	
	win = newwin (ro,co,y,x);

	screen->mainp = new_panel (win);

	win = panel_window (screen->mainp);

	screen->boxes = calloc (1,sizeof (box_list_t*));
	screen->boxes[0] = NULL;
  screen->cache = NULL;

	box (win,0,0);

	show_win_label (win,name);

	wmove (win,4,2);

	return screen;

}

void screen_focus_curr (screen_t **main_scrs, int32_t n) {

	WINDOW *win;
  screen_t *scr;
  int32_t i;
  box_list_t *boxes;
  box_list_t *box;

	for (i=0;i<n;i++) {
		scr = main_scrs[i];
		if (scr->focus) {
			win = panel_window (scr->mainp);
			curses_win_focus (win,scr->name);

			if (scr == main_scrs[COMP_SCR]) {
				boxes = screen_boxes (scr);
				if (boxes != NULL) {
					box = box_bottom_pos (boxes);
					if (box != NULL) {
						box->box->focus = 1;
						box_focus (box->box);
						scr->fbox = box;
					}
				}
			}

			update_panels ();
			doupdate ();
			return;
		}
	}

	return;
}

screen_t *screen_focused (screen_t **main_scrs, int32_t n) {

	int32_t i;

	for (i=0;i<n;i++) {

		if (main_scrs[i]->focus) {
			return main_scrs[i];
		}

	}

	return NULL;
}


void screen_focus_next_box (screen_t *screen) {

	box_list_t *boxpos;
	box_t *box;

	if (screen->fbox != NULL) {

		boxpos = screen->fbox;
		box = boxpos->box;

		if (boxpos->prev != NULL) {
			screen->fbox = boxpos->prev;
			boxpos = screen->fbox;
			box->focus = 0;
			box_unfocus (box);
			box = boxpos->box;
			box->focus = 1;
			box_focus (box);
		}

	}

	return;
}

void screen_focus_prev_box (screen_t *screen) {

	box_list_t *boxpos;
	box_t *box;

	if (screen->fbox != NULL) {

		boxpos = screen->fbox;
		box = boxpos->box;

		if (boxpos->next != NULL) {
			screen->fbox = boxpos->next;
			boxpos = screen->fbox;
			box->focus = 0;
			box_unfocus (box);
			box = boxpos->box;
			box->focus = 1;
			box_focus (box);
		}

	}

	return;
}

void screen_set_focus_next (screen_t **main_scrs, int32_t n) {

	screen_t *screen;
	box_list_t *boxes;
	box_list_t *fbox;
	int32_t i;

	for (i=0;i<n;i++) {

		screen = main_scrs[i];

		if (screen->focus) {
			screen->focus = 0;
			curses_win_unfocus (panel_window (screen->mainp),screen->name);

			if (screen == main_scrs[COMP_SCR]) {
				boxes = screen_boxes (screen);
				if (boxes != NULL) {
					if (screen->fbox != NULL) {
						fbox = screen->fbox;
						fbox->box->focus = 0;
						box_unfocus (fbox->box);
						screen->fbox = NULL;
					}
				}
			}

			update_panels ();
			doupdate ();
			break;
		}

	}

	if (i < n-1) {
		screen = main_scrs[i+1];
		screen->focus = 1;
	} 
	else {

		screen = main_scrs[0];
		screen->focus = 1;

	}

	return;
}



void screen_reset_cur (screen_t *screen) {

  WINDOW *win;

  win = panel_window (screen->mainp);

  wmove (win,4,2);

  return;
}


void screen_clear (screen_t *screen) {
  
  int32_t y; 
  WINDOW *win;

  win = panel_window (screen->mainp);

  y = getcury (win);

  wclrtoeol(win);
  wmove (win,y+1,2);
  wclrtoeol(win);
  wmove (win,y,2);

  y = getcury (win);

  if (screen->focus) {
    wattron (win,A_BOLD);
  }

  box (win,0,0);

  wattroff (win,A_BOLD);

  show_win_label (win,screen->name);

  wmove (win,y,2);

  return;
}


void screen_redraw (scr_t *scr, int32_t ro, int32_t co, int32_t y, int32_t x) {

  WINDOW *win;

  win = panel_window (scr->mainp);
  del_panel (scr->mainp);
  delwin (win);

  win = newwin (ro,co,y,x);
  scr->mainp = new_panel (win);

  screen_repaint (scr);

  return;
}

void screen_repaint (screen_t *screen) {

  WINDOW *win;

  win = panel_window (screen->mainp);

  werase (win);

  if (screen->focus) {
    wattron (win,A_BOLD);
  }

  box (win,0,0);

  wattroff (win,A_BOLD);

  show_win_label (win,screen->name);

  wmove (win,4,2);


  return;
}


box_list_t *screen_boxes (screen_t *screen) {

	box_list_t **bxs;
	box_list_t *boxes;

	bxs = screen->boxes;
	boxes = *bxs;

	return boxes;
}
