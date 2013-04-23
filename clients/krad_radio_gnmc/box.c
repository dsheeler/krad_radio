#include "box.h"

box_t *box_alloc (void) {

	box_t *box;

	box = calloc (1, sizeof (box_t));
	box->pan = NULL;
	box->info = NULL;
	box->maxy = 0;
	box->focus = 0;

	return box;
}


void box_destroy (box_t *bx) {

	box_id_t id = bx->id;

	if (id.type == NAME) {
		free (id.val.name);
	}

	if (bx->info != NULL) {
		free (bx->info);
	}

	curses_delpan (bx->pan);
	free (bx);	

	return;
}


void box_push (box_list_t **boxes, box_t *bx) {

	box_list_t *new;
	box_list_t *next;

	new = calloc (1, sizeof (box_list_t));
	new->box = bx;
	new->next = *boxes;
	new->prev = NULL;
	next = *boxes;
	
	if (next != NULL) {
		next->prev = new;
	}

	*boxes = new;


	return;
}

void box_rmall (box_list_t **boxes) {

	box_list_t *head,*curr;
	box_t *box;


	head = *boxes;

	if (head == NULL) {
		return;
	}

	curr = head;

	while (curr != NULL) {

		box = curr->box;

		box_destroy (box);

		curr = curr->next;

		free (head);

		head = curr;

	}

	*boxes = NULL;

	return;
}


void box_rm (box_list_t **boxes, box_t *bx) {

	box_list_t *curr;
	box_t *box;
	box_list_t *head;
	box_list_t *prev;


	head = *boxes;

	if (head == NULL) {
		return;
	}

	box = head->box;

	if (head->next == NULL) {

		if (box_cmp(box->id,bx->id)) {
			box_destroy (box);
			free (head);
			*boxes = NULL;
			return;
		}

	}
	else {

		curr = head->next;
		prev = curr->prev;

		while (curr != NULL) {

			box = curr->box;

			if (box_cmp(box->id,bx->id)) {
				box_destroy (box);
				prev->next = curr->next;
				curr->next->prev = prev;
				free (curr);
				break;
			}

			curr = curr->next;
			prev = curr->prev;

		}
	}


	return;
}

void box_print (box_t *box, const char *format , ... ) {

	va_list arglist;
	WINDOW *win;	
	int32_t size;

	win = panel_window (box->pan);

	if (getmaxy (win) == 1) {

		if (!box->ro) { 
			if (box->info != NULL) {
				free (box->info);
			}

			va_start (arglist,format);
			size = vsnprintf (box->info,0,format,arglist);
			box->info = calloc (size+1,sizeof (char));
			va_end (arglist);
		}

		va_start (arglist,format);
		vsnprintf (box->info,size+1,format,arglist);
		va_end (arglist);

		if (box->focus) {
			box_focus (box);
			return;
		}
	}

	va_start (arglist,format);
	vw_printw (win,format,arglist);
	va_end (arglist);

  return;
}


void box_focus (box_t *box) {

	WINDOW *win;

	win = panel_window (box->pan);

	if (box->info != NULL) {
		wattron (win,A_BOLD | COLOR_PAIR (3));
		werase (win);
		wmove (win,0,0);
		wprintw (win,"%s",box->info);
		wattroff (win,A_BOLD | COLOR_PAIR (3));
	}

	return;
}

void box_unfocus (box_t *box) {

	WINDOW *win;

	win = panel_window (box->pan);

	if (box->info != NULL) {
		werase (win);
		wmove (win,0,0);
		wprintw (win,"%s",box->info);
	}

	return;
}


box_list_t *box_bottom_pos (box_list_t *boxes) {

	box_list_t *curr;

	if (boxes == NULL) {
		return NULL;
	}

	curr = boxes;

	while (curr != NULL) {

		if (curr->next == NULL) {
			return curr;
		}

		curr = curr->next;

	}

	return NULL;

}

void box_clear (box_t *box) {

	WINDOW *win;

	win = panel_window (box->pan);

	werase (win);
	wmove (win,0,0);

	return;
}

void box_clearln (box_t *box) {

	WINDOW *win;
	int32_t y;

	win = panel_window (box->pan);
	y = getcury (win);
	wmove (win,y,0);
	wclrtoeol (win);

	return;
}

void box_nl (box_t *box, int32_t x) {

	curses_nl (panel_window (box->pan), x);
	return;
}


box_t *box_get (box_list_t *boxes, box_id_t id) {

	box_list_t *next;
	box_t *box;

	if (boxes == NULL || boxes->box == NULL) { 
		return NULL;
	}

	next = boxes;

	while (next != NULL) {

		box = next->box;

		if (box_cmp (box->id,id)) {
			return box;
		}

		next = next->next;

	}

	return NULL;
}


void box_set_info (box_t *box, const char *info) {

	int32_t len;

	len = strlen (info) + 1;
	if (box->info != NULL) {
		free (box->info);
	}
	box->info = calloc (len,sizeof (char));
	snprintf (box->info,len,"%s",info);

	return;
}

int32_t box_check_space (box_t *box, int32_t minrows, int32_t mincols) {
	return check_win_space (panel_window (box->pan),minrows,mincols);
}


box_t *box_get_by_name (box_list_t *boxes, char *name) {

	box_t *box;
	box_id_t id;

	id.type = NAME;
	id.val.name = name;

	box = box_get (boxes,id);

	return box;
}

box_t *box_get_by_id (box_list_t *boxes, int id_val) {

	box_t *box;
	box_id_t id;

	id.type = ID;
	id.val.id = id_val;

	box = box_get (boxes,id);

	return box;
}


box_t *box_create (screen_t *screen, box_id_type_t type, int lines) {

	box_t *box;
	box_t *topbox;
	WINDOW *win;
	box_list_t **bxs;
	box_list_t *boxes;
	int32_t offset;

	offset = 4;

	bxs = screen->boxes;
	boxes = *bxs;

	win = panel_window (screen->mainp);
  box = box_alloc ();
  box->id.type = type;
  box->ro = 0;

  topbox = NULL;

  if (boxes != NULL) {
  	topbox = boxes->box;
	}

	if (topbox != NULL) {
		offset = topbox->maxy;
	}

	box->maxy = offset + lines;

	box->pan = new_panel (newwin (lines,getmaxx (win) - 4,
	getbegy (win) + offset,getbegx (win) + 2));
	box_push (screen->boxes,box);


	return box;
}

void box_set_name (box_t *box, const char *name) {

	box->id.val.name = calloc (strlen (name) + 1,sizeof (char));
	snprintf (box->id.val.name,strlen (name) + 1,"%s",name);

	return;
}

void box_set_id (box_t *box, int id) {

	box->id.val.id = id;

	return;
}


int box_cmp (box_id_t id1, box_id_t id2) {

  switch (id1.type) {
    case NAME:
      if (id2.type == NAME) {
        if (!strcmp (id1.val.name,id2.val.name)) {
          return 1;
        }
      }
      break;
    case ID:
      if (id2.type == ID) {
        if (id1.val.id == id2.val.id) {
          return 1;
        }
        
      }
      break;
  }

  return 0;

}

void box_reset_cur (box_t *box) {

  WINDOW *win;

  win = panel_window (box->pan);

  wmove (win,0,0);

  return;
}

void box_redraw_all (box_list_t *boxes, screen_t *screen) {

  box_list_t *curr;
  box_t *box;
  WINDOW *win,*swin;
  int32_t x;

  curr = boxes;
  swin = panel_window (screen->mainp);
  x = getmaxx (swin);

  while (curr != NULL) {
    box = curr->box;
    win = panel_window (box->pan);
    wresize (win,getmaxy(win),x-4);
    move_panel (box->pan,getbegy(swin)+box->maxy-getmaxy(win),getbegx(swin)+2);
    top_panel (box->pan);
    curr = curr->next;
  }


};