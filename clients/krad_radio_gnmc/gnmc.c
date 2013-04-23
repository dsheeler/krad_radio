#include "gnmc.h"

static void gnmc_cleanup (gnmc_t *mon) {

	screen_t **main_screens;
	int32_t i;

	main_screens = mon->main_scrs;                                         
	
	for (i=0;i<mon->nscrs;i++) {
		screen_destroy (main_screens[i]);
	}

	free (main_screens);

	if (mon->top_screen != NULL) {
		screen_destroy (mon->top_screen);
	}

	free (mon->name);
  free (mon->info);
	free (mon);

	close (sockeys[0]);
	close (sockeys[1]);

	return;
}


static void gnmc_resize_handler (gnmc_t *mon) {

	screen_t *screen;
	int32_t rows,cols,i;
	box_list_t **boxes;


	struct winsize size;

		if (ioctl(fileno(stdout), TIOCGWINSZ, &size) == 0) {
			resize_term(size.ws_row, size.ws_col);
			wrefresh(curscr);
		}

	if (mon->top_screen != NULL) {
		screen_destroy (mon->top_screen);
	}

	getmaxyx (stdscr,rows,cols);

	for (i=0;i<mon->nscrs;i++) {

		screen = mon->main_scrs[i];
		boxes = screen->boxes;
	
		if (i == STAT_SCR) {
			screen_redraw (screen,rows/2,cols/2,0,0);
			box_redraw_all (*boxes,screen);
		}
		else if (i == MIX_SCR) {
			screen_redraw (screen,rows/2,cols/2,0,cols/2);
			box_redraw_all (*boxes,screen);
		}
		else if (i == COMP_SCR) {
			screen_redraw (screen,rows/2,cols/2,rows/2,0);
			box_redraw_all (*boxes,screen);
		}
		else if (i == TRANS_SCR) {
			screen_redraw (screen,rows/2,cols/2,rows/2,cols/2);
			box_redraw_all (*boxes,screen);
		}
	}

	wclear (stdscr);
  touchwin (stdscr);

  update_panels ();
	doupdate ();

	return;
}


static int32_t gnmc_sysinfo_show (screen_t *scr, 
  kr_radio_t *radio, info_t *info) {

	char *str;
	char *tmpstr;
	char *token;
	char uptime[32];

	box_t *sysinfo_box,*moninfo_box;
	box_list_t **bxs;
	box_list_t *boxes;

	s_to_hhmmss (uptime, radio->uptime);

	bxs = scr->boxes;
	boxes = *bxs;

	sysinfo_box = box_get_by_name (boxes,"sysinfo");
  moninfo_box = box_get_by_name (boxes,"moninfo");

	box_clear (sysinfo_box);

	box_reset_cur (sysinfo_box);

	box_print (sysinfo_box,"Uptime: %s",uptime);

	box_nl (sysinfo_box,0);

	box_print (sysinfo_box,"CPU: %u%%",radio->cpu_usage);

	box_nl (sysinfo_box,0);

	str = strdup (radio->sysinfo);
	tmpstr = str;
	while ( (token = strsep (&str,"\n")) != NULL ) {
		box_print (sysinfo_box,"%s",token);
		box_nl (sysinfo_box,0);
	}
	free (tmpstr);

	if (radio->logname[0] != '\0') {
		box_print (sysinfo_box,"Log: %s",radio->logname);
	}

  box_clear (moninfo_box);
  box_print (moninfo_box,"Deliveries: %d",info->deliveries);
  box_nl (moninfo_box,0);
  box_print (moninfo_box,"Crates: %dS %dM %dC %dT",
  info->crates[0],info->crates[1],info->crates[2],info->crates[3]);

	return 0;
}

static int32_t gnmc_krstation_handler (gnmc_t *mon, kr_crate_t *crate) {

	screen_t *screen;
	box_t *rbox;
	kr_subunit_t subunit;
	kr_radio_t *radio;
	kr_remote_t *remote;
  screen_t **main_scrs;

	subunit = crate->addr->path.subunit;
	radio = crate->inside.radio;
	remote = crate->inside.remote;

  main_scrs = mon->main_scrs;
  screen = main_scrs[STAT_SCR];


	if (subunit.zero == KR_TAGS) {
      // printf ("Station Tags");
	}


	switch (subunit.station_subunit) {

		case KR_CPU:
				// if (kr_crate_has_int (crate)) {
				// 	mvwprintw (win,5,5,"CPU %d%%",crate->integer);
				// 	update_panels ();
				// 	doupdate ();
				// }
			break;
		case KR_REMOTE:
			
			rbox = box_get_by_id (screen_boxes (screen),remote->port);

			if (rbox == NULL) {
				rbox = box_create (screen,ID,1);
				box_set_id (rbox,remote->port);
			}

			box_clear (rbox);
			box_reset_cur (rbox);
			box_print (rbox,"Remote: %s %d",remote->interface,remote->port);

			break;
		default:
			if (gnmc_sysinfo_show (screen,radio,mon->info) < 0) {
				return -1;
			}
			break;
	}
    
	return 0;
}


static int32_t gnmc_portgroup_show (port_t *port, screen_t *scr) {

	box_t *port_box;

	port_box = box_create (scr,NAME,1);
	box_set_name (port_box,port->sysname);

	switch (port->direction) {
		case INPUT:
			box_print (port_box,"Input %s , Vol: %0.2f%%",
      port->sysname,port->volume[0]);
			box_set_info (port_box,"Input");
      port_box->ro = 1;
			break;
		case OUTPUT:
			if (port->output_type == DIRECT) {
				box_print (port_box,"Output %s , Vol: %0.2f%%",
        port->sysname,port->volume[0]);
				box_set_info (port_box,"Output");
        port_box->ro = 1;
			}
			else {
				box_print (port_box,"Aux Output %s , Vol: %0.2f%%",
        port->sysname,port->volume[0]);
				box_set_info (port_box,"Aux Output");
        port_box->ro = 1;
			}
			break;
		case MIX:
			box_print (port_box,"Bus %s , Vol: %0.2f%%",
      port->sysname,port->volume[0]);
			box_set_info (port_box,"Bus");
      port_box->ro = 1;
			break;
	}

	return 0;
}

static void gnmc_krmixer_updatevol (screen_t *screen, char *name, float vol) {

	box_t *box;
	box_list_t **boxes;
	box_list_t *bxs;

	boxes = screen->boxes;
	bxs = *boxes;

	box = box_get_by_name (bxs, name);

	if (box != NULL) {
		box_clear (box);
		box_print (box,"%s %s , Vol: %0.2f%%",box->info,name,vol);
	}

	return;
}

static int gnmc_krmixer_handler (screen_t **main_scrs, kr_crate_t *crate) {

	screen_t *screen;
	kr_subunit_t subunit;

	subunit = crate->addr->path.subunit;

	screen = main_scrs[MIX_SCR];

  kr_portgroup_t *port = crate->inside.portgroup;

	if (subunit.mixer_subunit == KR_PORTGROUP) {

		if (kr_crate_notice (crate) == EBML_ID_KRAD_RADIO_UNIT_DESTROYED)  {
			box_rmall (screen->boxes);
			return 1;
		}

		if (kr_crate_contains_float (crate)) {
			if (crate->addr->control.portgroup_control == KR_VOLUME) {
				gnmc_krmixer_updatevol (screen,crate->addr->id.name,crate->real);
        return 0;
      }
		}

		if (kr_crate_loaded (crate)) {
			if (crate->contains == KR_PORTGROUP) {
				if (gnmc_portgroup_show (port,screen) < 0) {
					return -1;
				}
			}
		}

	}

	return 0;
}

static int32_t gnmc_krvideoport_show (comp_t *comp, screen_t *scr) {


	box_t *box;

	box = box_get_by_name (screen_boxes (scr), "res");
	box_clear (box);
	box_reset_cur (box);

	box_print (box,"Res: %dx%d",comp->width,comp->height);

	box = box_get_by_name (screen_boxes (scr), "fps");
	box_clear (box);
	box_reset_cur (box);

	box_print (box,"Fps: %d",comp->fps_numerator / comp->fps_denominator);

	box = box_get_by_name (screen_boxes (scr),"texts");
	box_clear (box);
	box_reset_cur (box);

	box_print (box,"Texts: %d",comp->texts);

	box = box_get_by_name (screen_boxes (scr), "sprites");
	box_clear (box);
	box_reset_cur (box);

	box_print (box,"Sprites %d",comp->sprites);

	box = box_get_by_name (screen_boxes (scr), "vectors");
	box_clear (box);
	box_reset_cur (box);

	box_print (box,"Vectors: %d ",comp->vectors);

	box = box_get_by_name (screen_boxes (scr), "frames");
	box_clear (box);
	box_reset_cur (box);

	box_print (box,"Frames: %d ",comp->frames);


	return 0;
}


static void gnmc_krcomp_boxes_init (screen_t *screen) {

	box_set_name (box_create (screen,NAME,1),"res");
	box_set_name (box_create (screen,NAME,1),"fps");
	box_set_name (box_create (screen,NAME,1),"texts");
	box_set_name (box_create (screen,NAME,1),"sprites");
	box_set_name (box_create (screen,NAME,1),"vectors");
	box_set_name (box_create (screen,NAME,1),"frames");

	return;
}

static void gnmc_texts_screen_populate (scr_t *scr, kr_text_t *text, uint32_t id) {

	box_t *box;

  kr_comp_controls_t ctls = text->controls;

	box = box_create (scr,ID,1);
  box_set_id (box,id);
	box_print (box,"ID: %d X: %d Y: %d Text: %s",id,ctls.x,ctls.y,text->text);

	return;
}

static void gnmc_sprites_screen_populate (scr_t *scr, sprite_t *spr, uint32_t id) {

	box_t *box;

	box = box_create (scr,ID,1);
  box_set_id (box,id);
	box_print (box,"ID: %d Filename: %s",id,spr->filename);

	return;
}

static void gnmc_comp_cache_update (scr_t *scr, kr_crate_t *crate, 
  kr_compositor_subunit_t sub) {

  kr_compositor_control_t control;
  uint32_t id;
  kr_crate_t *cached;
  cr_cache_t *cache;

  control = crate->addr->control.compositor_control;
  id = crate->addr->id.number;
  cache = scr->cache;

  cached = cache->read (cache,id);

  if (cached != NULL) {
    switch (control) {
      case KR_NO:
        switch (sub) {
          case KR_TEXT:
            break;
          case KR_SPRITE:
            break;
          case KR_VECTOR:
            break;
          case KR_VIDEOPORT:
            break;
        }
      break;
      case KR_X:
        switch (sub) {
          case KR_TEXT:
            cached->rep.text.controls.x = crate->integer;
          break;
            case KR_SPRITE:
          break;
            case KR_VECTOR:
          break;
            case KR_VIDEOPORT:
        break;
      }
      break;
      case KR_Y:
        switch (sub) {
          case KR_TEXT:
            cached->rep.text.controls.y = crate->integer;
          break;
            case KR_SPRITE:
          break;
            case KR_VECTOR:
          break;
            case KR_VIDEOPORT:
        break;
      }
      break;
      case KR_Z:
        switch (sub) {
          case KR_TEXT:
            cached->rep.text.controls.z = crate->integer;
            break;
          case KR_SPRITE:
            break;
          case KR_VECTOR:
            break;
          case KR_VIDEOPORT:
            break;
        }
      break;
      case KR_WIDTH:
        switch (sub) {
          case KR_TEXT:
            break;
          case KR_SPRITE:
            break;
          case KR_VECTOR:
            break;
          case KR_VIDEOPORT:
            break;
        }
      break;
      case KR_HEIGHT:
        switch (sub) {
          case KR_TEXT:
            break;
          case KR_SPRITE:
            break;
          case KR_VECTOR:
            break;
          case KR_VIDEOPORT:
            break;
        }
      break;
      case KR_ROTATION:
        switch (sub) {
          case KR_TEXT:
            break;
          case KR_SPRITE:
            break;
          case KR_VECTOR:
            break;
          case KR_VIDEOPORT:
            break;
        }
      break;
      case KR_OPACITY:
        switch (sub) {
          case KR_TEXT:
            break;
          case KR_SPRITE:
            break;
          case KR_VECTOR:
            break;
          case KR_VIDEOPORT:
            break;
        }
      break;
      case KR_XSCALE:
        switch (sub) {
          case KR_TEXT:
            break;
          case KR_SPRITE:
            break;
          case KR_VECTOR:
            break;
          case KR_VIDEOPORT:
            break;
        }
      break;
      case KR_YSCALE:
        switch (sub) {
          case KR_TEXT:
            break;
          case KR_SPRITE:
            break;
          case KR_VECTOR:
            break;
          case KR_VIDEOPORT:
            break;
        }
      break;
      case KR_RED:
        switch (sub) {
          case KR_TEXT:
            break;
          case KR_SPRITE:
            break;
          case KR_VECTOR:
            break;
          case KR_VIDEOPORT:
            break;
        }
      break;
      case KR_GREEN:
        switch (sub) {
          case KR_TEXT:
            break;
          case KR_SPRITE:
            break;
          case KR_VECTOR:
            break;
          case KR_VIDEOPORT:
            break;
        }
      break;
      case KR_BLUE:
        switch (sub) {
          case KR_TEXT:
            break;
          case KR_SPRITE:
            break;
          case KR_VECTOR:
            break;
          case KR_VIDEOPORT:
            break;
        }
      break;
      case KR_ALPHA:
        switch (sub) {
          case KR_TEXT:
            break;
          case KR_SPRITE:
            break;
          case KR_VECTOR:
            break;
          case KR_VIDEOPORT:
            break;
        }
      break;
      case KR_TICKRATE:
        switch (sub) {
          case KR_TEXT:
            break;
          case KR_SPRITE:
            break;
          case KR_VECTOR:
            break;
          case KR_VIDEOPORT:
            break;
        }
      break;
    }

  }

  return;
}

static void gnmc_texts_screen_update (screen_t *screen, uint32_t id) {

  cr_cache_t *cache;
  kr_crate_t *crate;
  box_t *box;
  kr_comp_controls_t ctls;
  kr_text_t *text;
  box_list_t **bxs;

  cache = screen->cache;
  crate = cache->read (cache,id);
  bxs = screen->boxes;

  if (crate != NULL) {
    text = &crate->rep.text;
    ctls = text->controls;
    box = box_get_by_id (*bxs,id);
    if (box != NULL) {
      box_clear (box);
      box_print (box,"ID: %d X: %d Y: %d Text: %s",id,ctls.x,ctls.y,text->text);
    }
  }

  return;
}

static int32_t gnmc_krcomp_handler (gnmc_t *mon, kr_crate_t *crate) {

	screen_t *screen;
	kr_subunit_t subunit;
	screen_t **main_scrs;
  cr_cache_t *cache;

	kr_compositor_t *comp;

	main_scrs = mon->main_scrs;

	subunit = crate->addr->path.subunit;

	screen = main_scrs[COMP_SCR];

	switch (subunit.compositor_subunit) {
		case KR_VIDEOPORT:
			break;
		case KR_SPRITE:

      if (kr_crate_has_int (crate) || kr_crate_has_float (crate)) {
        if (istop (mon->top_screen,"Sprites")) {
          gnmc_comp_cache_update (mon->top_screen,crate,KR_SPRITE);
        }
      }


      if (kr_crate_loaded (crate)) {
        if (istop (mon->top_screen,"Sprites")) {
          gnmc_sprites_screen_populate (mon->top_screen,crate->inside.sprite,
            crate->addr->id.number);
          cache = mon->top_screen->cache;
          cache->write (cache,crate,crate->addr->id.number);
        }
      }

			break;
		case KR_TEXT:

      if (kr_crate_has_int (crate) || kr_crate_has_float (crate)) {
        if (istop (mon->top_screen,"Texts")) {
          gnmc_comp_cache_update (mon->top_screen,crate,KR_TEXT);
          gnmc_texts_screen_update (mon->top_screen,crate->addr->id.number);
        }
      }

      if (kr_crate_loaded (crate)) {
			 if (istop (mon->top_screen,"Texts")) {
          if (strlen (crate->inside.text->text) > 0) {
				    gnmc_texts_screen_populate (mon->top_screen,crate->inside.text,
              crate->addr->id.number);
            cache = mon->top_screen->cache;
            cache->write (cache,crate,crate->addr->id.number);
          }
			 }
      }

			break;
		case KR_VECTOR:
      if (kr_crate_loaded (crate)) {

      }
			break;
		default:
      if (kr_crate_loaded (crate)) {
			 comp = crate->inside.compositor;
			 if (gnmc_krvideoport_show (comp,screen) < 0) {
				 return -1;
			 }
      }
			break;
	}

	return 0;
}

static int32_t gnmc_krtrans_handler (screen_t **main_scrs, kr_crate_t *crate) {


	screen_t *screen;
	WINDOW *win;
	kr_subunit_t subunit;
	kr_unit_id_t id;

	subunit = crate->addr->path.subunit;
	id = crate->addr->id;

	screen = main_scrs[TRANS_SCR];

	win = panel_window (screen->mainp);

	wprintw (win,"Ouhhh! %d",subunit.transponder_subunit);

	switch (subunit.transponder_subunit) {
		case KR_ADAPTER:
			screen_reset_cur (screen);
			wprintw (win,"Adapter %d",id.number);
			break;
		case KR_TRANSMITTER:
			break;
		case KR_RECEIVER:
			break;
		case KR_RAWIN:
			screen_reset_cur (screen);
			wprintw (win,"RawIn!");
			break;
		case KR_RAWOUT:
			break;
		case KR_DEMUXER:
			break;
		case KR_MUXER:
			break;
		case KR_ENCODER:
			break;
		case KR_DECODER:
			break;
	}

	return 0;
}


static int32_t gnmc_delivery_handler (kr_client_t *client, gnmc_t *mon) {

  kr_crate_t *crate;
  crate = NULL;
  int32_t res;
  uint32_t *crates;

  crates = mon->info->crates;

  while ((kr_delivery_get (client, &crate) > 0) && (crate != NULL)) {

  		switch (crate->addr->path.unit) {
  			case KR_STATION:
          crates[0]++;
  				if (kr_crate_loaded (crate)) {
  					if (gnmc_krstation_handler (mon,crate) < 0) {
  						kr_crate_recycle (&crate);
  						return -1;
  					}
  				}
  				break;
  			case KR_MIXER:
          crates[1]++;
  				res = gnmc_krmixer_handler (mon->main_scrs,crate);
    				if (res < 0) {
  						kr_crate_recycle (&crate);
  						return -1;
  					}
  					else if (res == 1) {
  						kr_mixer_portgroups (client);
  					}
  				break;
  			case KR_COMPOSITOR:
          crates[2]++;
    			if (gnmc_krcomp_handler (mon,crate) < 0) {
  					kr_crate_recycle (&crate);
  					return -1;
  				}
  				break;
  			case KR_TRANSPONDER:
          crates[3]++;
  				if (kr_crate_loaded (crate)) {
            if (gnmc_krtrans_handler (mon->main_scrs,crate) < 0) {
  						kr_crate_recycle (&crate);
  						return -1;
  					}
  				}
  				break;
  			default:
  				break;

  	}

  	kr_crate_recycle (&crate);

	}

	return 0;
}

static int32_t gnmc_disconnect_handler (kr_client_t *client, 
  char *sysname, gnmc_t *mon) {

	int32_t rows,cols;
	WINDOW *win;
	PANEL *pan;
	int32_t ch;
	int32_t c;
  struct timeval disc_time,curr_time,gap_time;
  double usec1,usec2;

  disc_time = mon->info->disc_time;
  gettimeofday (&disc_time,NULL);

	getmaxyx (stdscr,rows,cols);

	c = 44+strlen (sysname);

	win = newwin (9,c,rows/2 - 5, cols/2 - c/2);
	box (win,0,0);
	pan = new_panel (win);
	wattron (win,A_BOLD);

	mvwprintw (win,2,2,"Connection lost to ");

	wattron (win,A_BOLD | A_UNDERLINE);

	wprintw (win,"%s",sysname);

	wattroff (win,A_UNDERLINE);

	wprintw (win," krad radio daemon");

  mvwprintw (win,4,2,"Disconnected since %s",asctime (localtime (&disc_time.tv_sec)));

	while (!kr_connect (client, sysname)) {
		ch = getch ();
		if (ch == 27) {
			return 1;
		}

    wmove (win,6,0);
    wclrtoeol (win);
    box (win,0,0);

    gettimeofday (&curr_time,NULL);

    usec1 = curr_time.tv_sec * 1000000 + curr_time.tv_usec;
    usec2 = disc_time.tv_sec * 1000000 + disc_time.tv_usec;

    gap_time.tv_sec = floor ((usec1 - usec2) / 1000000 );
    gap_time.tv_usec = (usec1 - usec2) - (gap_time.tv_sec * 1000000);

    mvwprintw (win,6,2,"Disconnected for %d.%0d sec",gap_time.tv_sec,gap_time.tv_usec / 100000);

    update_panels ();
    doupdate ();
    
    usleep (1000*100);
  }

  curses_delpan (pan);

  kr_subscribe_all (client);

  touchwin (stdscr);
  update_panels ();
  doupdate ();

	return 0;
}

static void gnmc_interactive_box_handler (gnmc_t *mon, kr_client_t *client) {

	box_list_t *boxpos;
	box_t *box;
	screen_t *screen;
	int32_t rows,cols;

	getmaxyx (stdscr,rows,cols);

	screen = screen_focused (mon->main_scrs,mon->nscrs);

	boxpos = screen->fbox;
	
	if (boxpos == NULL) {
		return;
	}

	box = boxpos->box;

	if (!strncmp (box->id.val.name,"texts",5)) {
		kr_compositor_subunit_list (client);
		mon->top_screen = screen_create ("Texts",rows,cols,0,0);
    mon->top_screen->cache = crcache_init ();
		curses_win_focus (panel_window (mon->top_screen->mainp),
      mon->top_screen->name);
	}
	else if (!strncmp (box->id.val.name,"sprites",5)) {
		kr_compositor_subunit_list (client);
		mon->top_screen = screen_create ("Sprites",rows,cols,0,0);
		curses_win_focus (panel_window (mon->top_screen->mainp),
      mon->top_screen->name);
	}


	return;
}

static void gnmc_top_screen_destroy (gnmc_t *mon) {

  screen_t *top;
  cr_cache_t *cache;

  top = mon->top_screen;
  cache = top->cache;
  if (cache != NULL) {
    cache->free (cache);
  }
  screen_destroy (top);
  mon->top_screen = NULL;

  return;
}

static gnmc_t *gnmc_init (char *sysname) {

	gnmc_t *monitor;
	box_t *sysinfo_box,*moninfo_box;
	int32_t rows,cols,i;
  screen_t **scrs;
  struct timeval conn_time;

	getmaxyx (stdscr,rows,cols);

	monitor = calloc (1, sizeof (gnmc_t));
	monitor->name = strdup (sysname);

	monitor->nscrs = 4;
	monitor->main_scrs = calloc (monitor->nscrs,sizeof (screen_t*));
	monitor->top_screen = NULL; 

  monitor->info = calloc (1, sizeof (info_t));
  monitor->info->deliveries = 0;
  
  for (i=0;i<4;i++) {
    monitor->info->crates[i] = 0;
  }
  
  conn_time = monitor->info->conn_time;

  gettimeofday (&conn_time,NULL);

  scrs = monitor->main_scrs;

  scrs[STAT_SCR] = screen_create ("Station",rows/2,cols/2,0,0);

  sysinfo_box = box_create (scrs[STAT_SCR], NAME, 5);
  box_set_name (sysinfo_box,"sysinfo");

  moninfo_box = box_create (scrs[STAT_SCR], NAME, 2);
  box_set_name (moninfo_box,"moninfo");

	scrs[MIX_SCR] = screen_create ("Mixer",rows/2,cols/2,0,cols/2);
	scrs[COMP_SCR] = screen_create ("Compositor",rows/2,cols/2,rows/2,0);
	scrs[TRANS_SCR] = screen_create ("Transponder",rows/2,cols/2,rows/2,cols/2);

	scrs[STAT_SCR]->focus = 1;

	gnmc_krcomp_boxes_init (scrs[COMP_SCR]);

	screen_focus_curr (scrs,monitor->nscrs);

	return monitor;
}


int gnmc_run (kr_client_t *client , char *sysname) {

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockeys)) { 
		return 0;
	}

	signal (SIGWINCH, signal_recv);

	gnmc_t *monitor;
	int32_t res,ch;
	struct timeval tv1,tv2;
	double timevar;
	double past;
	uint8_t dummy[1];
  screen_t **scrs;

	ch = 0;
	past = 0;
	timevar = 0;

	monitor = gnmc_init (sysname);

  scrs = monitor->main_scrs;

	gettimeofday(&tv1, NULL);

	kr_system_info (client);
	kr_remote_list (client);
	kr_mixer_portgroups (client);
	kr_compositor_info (client);

	kr_subscribe_all (client);

	while (1) {

		res = kr_gnmc_poll (client,POLL_INTERVAL);

		gettimeofday(&tv2, NULL);

		timevar = floor ( ((tv2.tv_usec / 1000) + (tv2.tv_sec * 1000)) - 
      ((tv1.tv_usec / 1000) + (tv1.tv_sec * 1000))  + past);

		if (timevar >= UPDATE_INTERVAL) { 
			kr_system_info (client);
			kr_remote_list (client);
			kr_compositor_info (client);
			past = 0;
		}
		else {
			past += timevar;
		}

		gettimeofday(&tv1, NULL);

    switch (res) {
      case 0:
      	continue;
      case 1: 
        ch = getch ();
        break;
      case 2:
        kr_delivery_recv (client);
        monitor->info->deliveries++;
        if (gnmc_delivery_handler (client,monitor) < 0) {
        	gnmc_cleanup (monitor);
        	return -1;
        }
        update_panels ();
        doupdate ();
        continue;
      case -1:
        // error
      	if (gnmc_disconnect_handler (client,sysname,monitor)) {
      		gnmc_cleanup (monitor);
      		return 0;
      	}
        continue;
      case -2:
        continue;
      case 3:
      	read (sockeys[1],dummy,1);
      	gnmc_resize_handler (monitor);
      	continue;
    }


    switch (ch) {
    	case 'r':
    		gnmc_resize_handler (monitor);
    		break;
   		case 27:
   		  if (monitor->top_screen != NULL) {
          gnmc_top_screen_destroy (monitor);
    		}
    		else  {
   		  	gnmc_cleanup (monitor);
   		  	return 0;
   		  }
    		break;
   		case 9:
   		  screen_set_focus_next (scrs,monitor->nscrs);
   		  screen_focus_curr (scrs,monitor->nscrs);
    		break;
    	case KEY_UP:
    		screen_focus_prev_box (screen_focused (scrs,monitor->nscrs));
    		break;
    	case KEY_DOWN:
    		screen_focus_next_box (screen_focused (scrs,monitor->nscrs));
    		break;
    	case 10:
    		gnmc_interactive_box_handler (monitor,client);
    		break;
   		default:
   			break;
    }

    update_panels ();
    doupdate ();

	}

	return 0;
}