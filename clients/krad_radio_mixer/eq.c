#include "eq.h"

static int digits (int n) {

	if (n >= 0 && n < 10) {
		return 1;
	}
	else if (n >= 10 && n < 100 ) {
		return 2;
	}
	else {
		return 0;
	}

}

static void itoa (int i,char *a,int n) {

  if (n == 2) {
    snprintf (a,n+1,"0%d",i);
  }
  else {
    snprintf (a,n,"%d",i);
  }
  
  return;

}

static void show_n_panels (PANEL **panels,int n) {
  int i;
  for (i=0;i<n;i++) {
    show_panel (panels[i]);
  }

  return;
}


static void db_show_value (float val, db_show_t *db , int idx) {

  float h;
  int pos,minpos,maxpos;
  float delta;
  float dist;
  float minval,maxval;


  PANEL *sliderp = db->slider_p[idx];
  WINDOW *bar_win = panel_window (db->bar_p[idx]);
  WINDOW *sliderw = panel_window (sliderp);

  minval = -12.00;
  maxval = 24.00;

  minpos = 2;
  maxpos = getmaxy (bar_win) - 1;


  h = getmaxy (bar_win) - 1;

  if (val <= minval) {
    pos = minpos;
  }
  else if (val >= maxval) {
    pos = maxpos;
  }
  else {
    dist = fabsf (val - minval); 

    delta = fabsf (maxval - minval);
    pos = floor ((h/delta) * dist);
  }


  if (pos <= minpos) {
      pos = minpos;
  }

  if (pos >= maxpos) {
      pos = maxpos;
  }

  wattron (sliderw,COLOR_PAIR (8));
  

  if (val>=0) {

    mvwprintw (sliderw,0,0,"+");

  }
  else {

    mvwprintw (sliderw,0,0,"-");

  }

  mvwprintw (sliderw,0,1,"%4.1fdB",fabsf(val));
  wattroff (sliderw,COLOR_PAIR (8));

 
  move_panel (sliderp,getbegy (bar_win) + getmaxy (bar_win) - pos,getbegx (sliderw));
  
  update_panels ();
  doupdate ();

  return;
}

static void hz_show_value (float val, hz_show_t *hz, int idx) {

  float maxval,minval;

  WINDOW *hz_cell_w = panel_window (hz->cell_p[idx]);

  maxval = 19000;
  minval = 30;

  if (val >= minval && val <= maxval) {
    mvwprintw (hz_cell_w,1,1,"%5.0fHz",val);
  }

  update_panels ();
  doupdate ();

  return;
}


static void bw_show_value (float val, bw_show_t *bw, int idx) {

  float w;
  int pos;
  float delta;
  float minval,maxval;

  WINDOW *bw_bar_w = panel_window (bw->bar_p[idx]);
  WINDOW *bw_cell_w = panel_window (bw->cell_p[idx]);
  PANEL *bw_slider_p = bw->slider_p[idx];

  minval = 0.01;
  maxval = 5.00;

  delta = maxval - minval;

  w = getmaxx (bw_bar_w) - 1;

  pos = (w/delta) * val;

  if (val >= minval && val <=maxval) {

    move_panel (bw_slider_p,getbegy (panel_window (bw_slider_p)),getbegx (bw_bar_w) + pos );

    
  }

  mvwprintw (bw_cell_w,0,0,"%f",val);

  update_panels ();
  doupdate ();

  return;
}

static void eq_init_values (eq_port_t *eqport, kr_address_t *addr, kr_mixer_portgroup_t *portg) {

  eq_saved_val_t *vals;
  ctrls_show_t *ctrls;

  vals = eqport->vals;
  ctrls = eqport->ctrls;

  db_show_t *db;
  hz_show_t *hz;
  bw_show_t *bw;

  db = ctrls->db;
  hz = ctrls->hz;
  bw = ctrls->bw;

  int band = eqport->band;

  if (!strcmp (addr->id.name,eqport->name)) {

    vals->db = portg->eq.band[band].db;
    vals->hz = portg->eq.band[band].hz;
    vals->bw = portg->eq.band[band].bandwidth;

    db_show_value (vals->db,db,0);
    hz_show_value (vals->hz,hz,0);
    bw_show_value (vals->bw,bw,0);

  
  }

  return;
}


static void eq_value_update (eq_port_t *eqport, kr_address_t *addr, float val) {

  int band;
  char *type;
  char *control;
  int curr_band;
  eq_saved_val_t *vals;
  ctrls_show_t *ctrls;

  
  vals = eqport->vals;
  ctrls = eqport->ctrls;

  db_show_t *db;
  hz_show_t *hz;
  bw_show_t *bw;

  db = ctrls->db;
  hz = ctrls->hz;
  bw = ctrls->bw;


  curr_band = eqport->band;
  control = effect_control_to_string(addr->control.effect_control);
  type = effect_type_to_string (addr->sub_id + 1);
  band = addr->sub_id2;


  if (!strcmp (addr->id.name,eqport->name)) {

    if (curr_band == band) {

      if (!strncmp (type,"eq",2)) {

        if (!strncmp (control,"db",2)) {

          vals->db = val;
          db_show_value (vals->db,db,0);

        }
        else if (!strncmp (control,"hz",2)) {

          vals->hz = val;
          hz_show_value (vals->hz,hz,0);

        }
        else if (!strncmp (control,"bw",2)) {

          vals->bw = val;
          bw_show_value (vals->bw,bw,0);

        }

      }
    }

  }

  return;
}


static int eq_delivery_handler (kr_client_t *client, eq_port_t *eqport) {


  kr_crate_t *crate;

  crate = NULL;


  while ((kr_delivery_get (client, &crate) > 0) && (crate != NULL)) {

    if ((crate->addr->path.unit == KR_MIXER)) {


        
        if (crate->notice == EBML_ID_KRAD_SUBUNIT_INFO && crate->addr->path.subunit.mixer_subunit == KR_PORTGROUP) {
      
            if (kr_crate_loaded (crate)) {

              eq_init_values (eqport,crate->addr,crate->inside.portgroup);
      
            }

        }

        if (crate->addr->path.subunit.mixer_subunit == KR_EFFECT) {
          if (kr_crate_contains_float (crate)) {
            eq_value_update (eqport,crate->addr,crate->real);
          }
        }
        
        kr_crate_recycle (&crate);
        continue;
    }

    if ((kr_crate_notice (crate) == EBML_ID_KRAD_RADIO_UNIT_DESTROYED) &&
        (crate->addr->path.unit == KR_MIXER) && (crate->addr->path.subunit.mixer_subunit == KR_PORTGROUP)) {
        /* TODO : handle */
        kr_crate_recycle (&crate);
        continue;
    }

    kr_crate_recycle (&crate);
  }
  
  return 0;
}


static void show_all_panels (ctrls_show_t *ctrls, int showed_bands) {

  db_show_t *db;
  hz_show_t *hz;
  bw_show_t *bw;

  db = ctrls->db;
  hz = ctrls->hz;
  bw = ctrls->bw;

  show_panel (db->pan);
  show_n_panels (db->bar_p,showed_bands);
  show_n_panels (db->slider_p,showed_bands);

  show_panel (hz->pan);
  show_n_panels (hz->cell_p,showed_bands);

  show_panel (bw->pan);
  show_n_panels (bw->bar_p,showed_bands);
  show_n_panels (bw->slider_p,showed_bands);
  show_n_panels (bw->cell_p,showed_bands);

  return;
}

static ctrls_show_t* eq_ctrls_gen (WINDOW *eq_win, int nbands) {


	/* Power*/
  WINDOW *db_win;
  /* Frequency */
  WINDOW *hz_win;
  /* Bandwidth */
  WINDOW *bw_win;

  /* Structs */
  ctrls_show_t *ctrls;

  db_show_t *db;
  hz_show_t *hz;
  bw_show_t *bw;

  
	int i;
  int max_ro,max_co,maxy,h_gap,v_gap;
  int v_bar_h,h_bar_h;
  int nctrls = 3;
	int h_offs = 0;
  int v_offs = 0;
  

  ctrls = calloc (1,sizeof (ctrls_show_t));

  ctrls->db = calloc (1,sizeof (db_show_t));
  ctrls->hz = calloc (1,sizeof (hz_show_t));
  ctrls->bw = calloc (1,sizeof (bw_show_t));

  db = ctrls->db;
  hz = ctrls->hz;
  bw = ctrls->bw;
  

  db->slider_p = calloc (nbands,sizeof (PANEL*));
  db->bar_p = calloc (nbands,sizeof (PANEL*));

  db->bar_w = calloc (nbands,sizeof (WINDOW*));
  db->slider_w = calloc (nbands,sizeof (WINDOW*));

  hz->cell_w = calloc (nbands,sizeof (WINDOW*));
  hz->cell_p = calloc (nbands,sizeof (PANEL*));

  bw->bar_w = calloc (nbands,sizeof (WINDOW*));
  bw->slider_w = calloc (nbands,sizeof (WINDOW*));
  bw->cell_w = calloc (nbands,sizeof (WINDOW*));

  bw->bar_p = calloc (nbands,sizeof (PANEL*));
  bw->slider_p = calloc (nbands,sizeof (PANEL*));
  bw->cell_p = calloc (nbands,sizeof (PANEL*));


	getmaxyx (eq_win,max_ro,max_co);

  max_co = floor (max_co / nctrls);

	h_gap = floor (max_co / nbands);
  v_gap = floor (max_ro*0.7 / nbands);

  /* Start Drawing */

  maxy = getmaxy (eq_win);

	db_win = newwin (max_ro*0.7,max_co,maxy * 0.20,max_co*0);
  db->pan = new_panel (db_win);
  //box (db_win,0,0);

  hz_win =  newwin (max_ro*0.7,max_co,maxy * 0.20,max_co*1);
  hz->pan = new_panel (hz_win);
  //box (hz_win,0,0);

  bw_win = newwin (max_ro*0.7,max_co,maxy * 0.20,max_co*2);
  bw->pan = new_panel (bw_win);
  //box (bw_win,0,0);

  v_bar_h = floor (getmaxy(db_win)*0.80) + 2;
  h_bar_h = floor (getmaxx(bw_win)*0.80) + 2;

  for (i=0;i<nbands;i++) {

    /* Power */

    db->bar_w[i] = newwin (v_bar_h,3,getbegy (db_win) + getmaxy (db_win) - (v_bar_h) - 2 , ( getbegx (db_win) + ((getmaxx (db_win)/nbands) / 2) ) + h_offs - 2);

    db->bar_p[i] = new_panel (db->bar_w[i]);

    wattron (db->bar_w[i],A_BOLD | COLOR_PAIR (4));
    box (db->bar_w[i],ACS_VLINE,ACS_HLINE);

    db->slider_w[i] = newwin (1,8,getbegy (db->bar_w[i])+ (v_bar_h/2),getbegx (db->bar_w[i]) - 2 );

    wattron (db->slider_w[i],A_BOLD |COLOR_PAIR (8));
    wprintw (db->slider_w[i],"     dB");

    db->slider_p[i] = new_panel (db->slider_w[i]);


    /* Frequency */

    hz->cell_w[i] = newwin (1+2,7+2,( getbegy (hz_win) + ((getmaxy (hz_win)/nbands) / 2) ) + v_offs - 1, getbegx (hz_win) + (getmaxx (hz_win) / 2) - 5);
    hz->cell_p[i] = new_panel (hz->cell_w[i]);
    wattron (hz->cell_w[i], A_BOLD | COLOR_PAIR (4));
    box (hz->cell_w[i],0,0);

    wattron (hz->cell_w[i],COLOR_PAIR (8));
    mvwprintw (hz->cell_w[i],1,1,"     Hz");

    /* Bandwidth */

    bw->bar_w[i] = newwin (2,h_bar_h,( getbegy (bw_win) + ((getmaxy (bw_win)/nbands) / 2) ) + v_offs, getbegx (bw_win) + (getmaxx (bw_win) / 2) - (h_bar_h/2));
    bw->bar_p[i] = new_panel (bw->bar_w[i]);
    wattron (bw->bar_w[i], A_BOLD | COLOR_PAIR (4));
    box (bw->bar_w[i],0,0);

    bw->slider_w[i] = newwin (2,1,getbegy (bw->bar_w[i]) ,getbegx (bw->bar_w[i]) + (h_bar_h/2) -1 );
    bw->slider_p[i] = new_panel (bw->slider_w[i]);

    wattron (bw->slider_w[i],COLOR_PAIR (1));
    waddch (bw->slider_w[i],' '|A_REVERSE);
    waddch (bw->slider_w[i],' '|A_REVERSE);

    bw->cell_w[i] = newwin (1,4,getbegy (bw->bar_w[i]) + 3,getbegx (bw->bar_w[i]) + (h_bar_h/2) - 2);
    bw->cell_p[i] = new_panel (bw->cell_w[i]);
    wattron (bw->cell_w[i],A_BOLD | COLOR_PAIR (8));
    wprintw (bw->cell_w[i],"    ");


    h_offs += h_gap;
    v_offs += v_gap;
  }


	update_panels ();
	doupdate ();


	return ctrls;
}

int eq_run (kr_client_t *client, char *sysname) {

	WINDOW *eq_win,*tmpw;
	PANEL  *eq_mainpan,*eq_subpan,*ctrl_subp;
	FORM *eq_form;
	FIELD **field;
	FIELD **ctrl_field;
	FORM *ctrl_form;


  /* Panels Structs */
  ctrls_show_t *ctrls;
  eq_port_t *eqport;

  db_show_t *db;
  hz_show_t *hz;
  bw_show_t *bw;

  eq_saved_val_t *vals;

	int i;
	int max_co,subx,suby,gap;
	int offs = 0;
	int field_idx;
	int in_ctrl = 0;
  int nbands = KRAD_EQ_MAX_BANDS;
  int showed_bands = 1;
  int idx;
  char num[3];

	int ch;
	int res;
  int bandn;

	clear ();

	char *fieldname[3] = {"Power","Frequency","Bandwidth"}; 
  int nctrls = 3;

	eq_win = newwin (0,0,0,0);

	field = calloc (nbands+1,sizeof (FIELD*));

  ctrl_field = calloc (nctrls+1,sizeof (FIELD*));

	max_co = getmaxx (eq_win);

	gap = floor ((max_co) / nbands);

  vals = NULL;
  bandn = 0;
  ch = 0;


  for (i=0;i<nbands;i++) {

    field[i] = new_field (1,2,1, offs, 0, 0);
    set_field_just (field[i], JUSTIFY_CENTER);
    itoa (i,num,digits (i) + 1 );
    set_field_buffer (field[i], 0, num);
    field_opts_off (field[i], O_AUTOSKIP | O_EDIT); 
    offs += gap;

  }

  field[nbands] = NULL;

  offs = 0;
  gap = floor ((max_co ) / nctrls);

  for (i=0;i<nctrls;i++) {

    ctrl_field[i] = new_field (1,strlen (fieldname[i]),1, offs, 0, 0);
    set_field_just (ctrl_field[i], JUSTIFY_CENTER);
    set_field_buffer (ctrl_field[i], 0, fieldname[i]);
    field_opts_off (ctrl_field[i], O_AUTOSKIP | O_EDIT); 
    offs += gap;

  }


  /* Controls generation */
  ctrls = eq_ctrls_gen (eq_win,showed_bands);

  vals = calloc (1,sizeof (eq_saved_val_t));

  eqport = calloc (1,sizeof (eq_port_t));

  db = ctrls->db;
  hz = ctrls->hz;
  bw = ctrls->bw;

  eqport->name = sysname;
  eqport->vals = vals;
  eqport->ctrls = ctrls;


  /* Eq form init */
  eq_form = new_form (field);

  wattron (eq_win,COLOR_PAIR (2) | A_BOLD);
  mvwprintw (eq_win,getmaxy (eq_win) * 0.05,floor ((max_co/2)-((strlen (sysname)+14)/2)),"Equalizer for %s",sysname); 
  wattroff (eq_win,COLOR_PAIR (2) | A_BOLD);

  set_form_win (eq_form, eq_win);
  scale_form (eq_form, &suby, &subx);
  set_form_sub (eq_form, derwin (eq_win,suby, subx , getmaxy (eq_win) * 0.10 , (max_co - subx) / 2 ));

  eq_mainpan = new_panel (eq_win);
  eq_subpan = new_panel (form_sub (eq_form));

  top_panel (eq_mainpan);
  top_panel (eq_subpan);

  post_form (eq_form);

  keypad (eq_win, TRUE);

  set_current_field (eq_form,field[0]);
  set_field_fore (current_field (eq_form),A_BOLD | COLOR_PAIR (3)); 


  /* Eq ctrls form init */

  ctrl_form = new_form (ctrl_field);

  set_form_win (ctrl_form, eq_win);
  scale_form (ctrl_form, &suby, &subx);
  set_form_sub (ctrl_form, derwin (eq_win,suby, subx , getmaxy (eq_win) * 0.90 , (max_co - subx) / 2 ));

  ctrl_subp = new_panel (form_sub (ctrl_form));

  top_panel (ctrl_subp);

  post_form (ctrl_form);


  field_idx = field_index (current_field (eq_form));


  /* Let's show controls! */
  kr_mixer_portgroups (client);
  show_all_panels (ctrls,showed_bands);


	update_panels ();
	doupdate ();

	while (ch != 'q') { 


    res = krm_poll (client,-1); 

    switch (res) {
      case 0:
        continue;
      case 1:
        ch = getch ();
        break;
      case 2:
        eqport->band = bandn;
        kr_delivery_recv (client);
        eq_delivery_handler (client,eqport);
        continue;
      case -1:
        continue;
    }

    switch (ch) { 

      case KEY_LEFT:

      	if (in_ctrl) {
          set_field_fore (current_field (ctrl_form),A_NORMAL); 
          form_driver (ctrl_form, REQ_PREV_FIELD);
          set_field_fore (current_field (ctrl_form),A_BOLD | COLOR_PAIR (1)); 
          idx = field_index (current_field (ctrl_form));
      	}
      	else {
      		set_field_fore (current_field (eq_form),A_NORMAL); 
      		form_driver (eq_form, REQ_PREV_FIELD);
      		set_field_fore (current_field (eq_form),A_BOLD | COLOR_PAIR (3)); 
          bandn = field_index (current_field (eq_form));
          kr_mixer_portgroups (client); 
      	}
        break;
      case KEY_RIGHT:
      	if (in_ctrl) {
          set_field_fore (current_field (ctrl_form),A_NORMAL); 
          form_driver (ctrl_form, REQ_NEXT_FIELD);
          set_field_fore (current_field (ctrl_form),A_BOLD | COLOR_PAIR (1)); 
          idx = field_index (current_field (ctrl_form));
          
        }
      	else {
          set_field_fore (current_field (eq_form),A_NORMAL); 
          form_driver (eq_form, REQ_NEXT_FIELD);
          set_field_fore (current_field (eq_form),A_BOLD | COLOR_PAIR (3));
          bandn = field_index (current_field (eq_form));
          kr_mixer_portgroups (client); 
      	}
        break;
      case KEY_UP:
        if (in_ctrl) {
          switch (idx) {
            case 0:
              vals->db += 0.50;
              kr_mixer_set_effect_control (client, sysname, 0, bandn, "db", vals->db, 0, 0);
              break;
            case 1:
              vals->hz += 5;
              kr_mixer_set_effect_control (client, sysname, 0, bandn, "hz", vals->hz, 0, 0);
              break;
            case 2:
              vals->bw += 0.10;
              kr_mixer_set_effect_control (client, sysname, 0, bandn, "bw", vals->bw, 0, 0);
              break;
          }

        }
        break;
      case KEY_DOWN:
        if (in_ctrl) {
          switch (idx) {
            case 0:
              vals->db -= 0.5;
              kr_mixer_set_effect_control (client, sysname, 0, bandn, "db", vals->db, 0, 0);
              break;
            case 1:
              vals->hz -= 5;
              kr_mixer_set_effect_control (client, sysname, 0, bandn, "hz", vals->hz, 0, 0);
              break;
            case 2:
              vals->bw -= 0.10;
              kr_mixer_set_effect_control (client, sysname, 0, bandn, "bw", vals->bw, 0, 0);
              break;

          }
   
        }
        break;
      case 'w':
        if (in_ctrl) {
          switch (idx) {
            case 0:
              vals->db += 0.10;
              kr_mixer_set_effect_control (client, sysname, 0, bandn, "db", vals->db, 0, 0);
              break;
            case 1:
              vals->hz += 1;
              kr_mixer_set_effect_control (client, sysname, 0, bandn, "hz", vals->hz, 0, 0);
              break;
            case 2:
              break;

          }

        }
        break;
      case 's':
        if (in_ctrl) {
          switch (idx) {
            case 0:
              vals->db -= 0.10;
              kr_mixer_set_effect_control (client, sysname, 0, bandn, "db", vals->db, 0, 0);
              break;
            case 1:
              vals->hz -= 1;
              kr_mixer_set_effect_control (client, sysname, 0, bandn, "hz", vals->hz, 0, 0);
              break;
            case 2:
              break;

          }
   
        }
        break;
      case 9:
        if (in_ctrl) {
          idx = field_index (current_field (ctrl_form));
          set_field_fore (ctrl_field[idx],A_NORMAL);
          /* ncurses bug I suppose */
          form_driver (ctrl_form, REQ_NEXT_FIELD);
          form_driver (ctrl_form, REQ_PREV_FIELD);
          set_current_field (eq_form,field[field_idx]); 
          in_ctrl = 0;
        }
        else {
          field_idx = field_index (current_field (eq_form));
          set_current_field (ctrl_form,ctrl_field[0]); 
          set_field_fore (ctrl_field[0],A_BOLD | COLOR_PAIR (1));  
          /* ncurses bug I suppose */
          form_driver (ctrl_form, REQ_NEXT_FIELD);
          form_driver (ctrl_form, REQ_PREV_FIELD);
          idx = 0;
          in_ctrl = 1;
        }
        break;
      default:
        break;
    }

    update_panels ();
    doupdate ();
  }
 	

  /* CLEANUP */


  for (i=0;i<showed_bands;i++) {

    tmpw = panel_window (db->slider_p[i]);
    del_panel (db->slider_p[i]);
    delwin (tmpw);

    tmpw = panel_window (db->bar_p[i]);
    del_panel (db->bar_p[i]);
    delwin (tmpw);    

    tmpw = panel_window (hz->cell_p[i]);
    del_panel (hz->cell_p[i]);
    delwin (tmpw);

    tmpw = panel_window (bw->bar_p[i]);
    del_panel (bw->bar_p[i]);
    delwin (tmpw);

    tmpw = panel_window (bw->slider_p[i]);
    del_panel (bw->slider_p[i]);
    delwin (tmpw);

    tmpw = panel_window (bw->cell_p[i]);
    del_panel (bw->cell_p[i]);
    delwin (tmpw);

  }


  free (db->bar_w);
  free (db->slider_w);
  free (hz->cell_w);
  free (bw->bar_w);
  free (bw->slider_w);
  free (bw->cell_w);
  free (db->bar_p);
  free (db->slider_p);
  free (hz->cell_p);
  free (bw->bar_p);
  free (bw->slider_p);
  free (bw->cell_p);

  tmpw = panel_window (db->pan);
  del_panel (db->pan);
  delwin (tmpw);

  tmpw = panel_window (hz->pan);
  del_panel (hz->pan);
  delwin (tmpw);

  tmpw = panel_window (bw->pan);
  del_panel (bw->pan);
  delwin (tmpw);

  free (ctrls);
  free (db);
  free (hz);
  free (bw);
  free (vals);
  free (eqport);


  tmpw = form_sub (ctrl_form);

  unpost_form (ctrl_form);
  free_form (ctrl_form);

  for (i=0;i<nctrls;i++) {
    free_field (ctrl_field[i]);
  }

  free (ctrl_field);

  del_panel (ctrl_subp);
  delwin (tmpw);

  tmpw = form_sub (eq_form);

  unpost_form (eq_form);
  free_form (eq_form);

  
  for (i=0;i<nbands;i++) {
    free_field (field[i]);
  }

  free (field);


  del_panel (eq_subpan);
  del_panel (eq_mainpan);

  delwin (tmpw);
  delwin (eq_win);




	return 0;
}