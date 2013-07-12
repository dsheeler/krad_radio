#include "mixer.h"
 
static void mix_mute (kr_client_t *client, mix_port_t *port) {

  if (!port->mute) {
    port->mute = 1;
    kr_mixer_set_control (client,port->name,"volume",0, 0);
  }
  else {
    port->mute = 0;
    kr_mixer_set_control (client,port->name,"volume",port->vol, 0);
  }


  return;

}

static void mix_txt_set (mix_item_t *item) {

  mix_widget_t *widget;
  mix_port_t *port;
  WINDOW *bar_win;



  if (item == NULL) {
    return;
  }


  widget = item->widget;
  port = item->port;

  bar_win = panel_window (widget->bar_p);
  
  wattron (bar_win,A_BOLD | COLOR_PAIR (3));
  mvwprintw (bar_win,getmaxy (bar_win) - 2,1,"  ");

  if (!port->mute) {
    if (port->vol < 10) {

      mvwprintw (bar_win,getmaxy (bar_win) - 2,1,"0%d",port->vol);

    }
    else if (port->vol > 99) {

      mvwprintw (bar_win,getmaxy (bar_win) - 2,1,"MX");

    }
    else {

      mvwprintw (bar_win,getmaxy (bar_win) - 2,1,"%d",port->vol);

    }
  }
  else {
    mvwprintw (bar_win,getmaxy (bar_win) - 2,1,"MM",port->vol);
  }
  wattroff (bar_win,A_BOLD | COLOR_PAIR (3));
      
  
  return;
}


static void mix_vol_update (int val, mix_item_t *item)  {

    int i;
    int vmax = 100;
    int vmin = 0;
    PANEL **bar_cell_p;


    if (item == NULL) {
      return;
    }

    /* total number of blocks */
    int tblocks = item->widget->bar_cell_n ;

    /* how many blocks do we need to show */
    int addsub = floor ((tblocks * val) / vmax);

    addsub = tblocks - addsub;

    bar_cell_p = item->widget->bar_cell_p;


    if ((val <= vmax) && (val >= vmin)) {


      item->port->vol = val;


      for (i=0;i<tblocks;i++) {
        show_panel (bar_cell_p[i]);
      }

      for (i=0;i<addsub;i++) { 
        hide_panel (bar_cell_p[i]);
      }

    }

    else if (val < 0) {


      for (i=0;i<tblocks;i++) {
        hide_panel (bar_cell_p[i]);
      }
      item->port->vol = 0;
    }

    else if (val > 100) {

      for (i=0;i<tblocks;i++) {
        show_panel (bar_cell_p[i]);
      }
      item->port->vol = 100;
    }


}


static void mix_form_create (mixer_t *mix) {


  mix->form = new_form (mix->fields);

  set_form_win (mix->form,panel_window (mix->main_p));

  set_form_sub (mix->form,panel_window (mix->sub_p));

  post_form (mix->form);

  set_current_field (mix->form,mix->fields[0]);
  set_field_fore (current_field (mix->form),A_BOLD | COLOR_PAIR (4)); 

  wattron (panel_window (mix->main_p),A_BOLD | COLOR_PAIR (8));
  mvwprintw (panel_window (mix->main_p), 2 ,getmaxx (panel_window (mix->main_p))/2 - 8,"Krad Radio Mixer");
  wattroff (panel_window (mix->main_p),A_BOLD | COLOR_PAIR (8));

  return;

}

static void mix_form_destroy (mixer_t *mix) {

  set_field_fore (current_field (mix->form),A_NORMAL); 
  unpost_form (mix->form);
  free_form (mix->form);
  mix->form = NULL;

  return;
}

static void mix_widgets_arrange (mixer_t *mix) {

  int i,k;
  int rows,cols;
  int width,height;
  int bar_h,bar_w;
  int relx,rely;
  int bar_relx,bar_rely;
  mix_item_t *item;
  mix_widget_t *widget;
  int gap,left_space;
  int needed_space;
  int offs;

  needed_space = 0;
  offs = 0;
  getmaxyx (panel_window (mix->main_p),rows,cols);

  for (k=0;k<mix->nitems;k++) {

    item = mix->items[k];
    widget = item->widget;

    needed_space += getmaxx (panel_window (widget->main_p) );

  }

  left_space = cols - needed_space;

  if (left_space < 0) {
    //TODO: HANDLE THIS CASE!!!
    fprintf (stderr, "Space error %d %d\n",cols,needed_space);
    exit (1);
  }

  gap = left_space / (mix->nitems + 1);

  
  for (k=0;k<mix->nitems;k++) {

    item = mix->items[k];
    widget = item->widget;

    /* Gathering info */

    getmaxyx (panel_window (widget->main_p),height,width);
    getmaxyx (panel_window (widget->bar_p),bar_h,bar_w);
    

    /* Positioning main Panel */

    move_panel (widget->main_p,rows/2 - height/2,gap + k*gap + offs);
    getbegyx (panel_window (widget->main_p),rely,relx);

    /* Centering bar Panel */

    move_panel (widget->bar_p, rely + height/2 - bar_h/2 , relx + width/2 - bar_w/2);
    getbegyx (panel_window (widget->bar_p),bar_rely,bar_relx);

    /* Placing bar cells */

    for (i=0;i<bar_h-4;i++) {
      
      move_panel (widget->bar_cell_p[i],bar_rely + 1 + i,bar_relx + 1);

    }

    /* Centering form field */

    move_field (widget->field,rely + height + 1,gap + k*gap + offs + 1);

    offs += width;
  }

  return;
}

static void mix_items_realloc (mixer_t *mix, int freeflag) {
    
    mix_item_t **items_tmp;
    FIELD **fields_tmp;
    int i,j;

    if (!freeflag) {

      if (mix->nitems == 1) {

        mix->items = calloc (1,sizeof (mix_item_t*));
        mix->fields = calloc (2,sizeof (FIELD*));

      }

      else {


        mix_form_destroy (mix);

        items_tmp = calloc (mix->nitems,sizeof (mix_item_t*));
        fields_tmp = calloc (mix->nitems+1,sizeof (FIELD*));

        for (i=j=0;i<mix->nitems - 1;i++) {

          if (mix->items[i] != NULL) {
            items_tmp[j] = mix->items[i];
            fields_tmp[j] = mix->fields[i];
            j++;
          }


        }

        free (mix->items);
        free (mix->fields);

        mix->items = items_tmp;
        mix->fields = fields_tmp;

      }

    }

    else {

      if (mix->nitems == 0) {

        free (mix->items);
        free (mix->fields);

        mix->items = NULL;
        mix->fields = NULL;

      }
      else {

        items_tmp = calloc (mix->nitems,sizeof (mix_item_t*));
        fields_tmp = calloc (mix->nitems+1,sizeof (FIELD*));

        for (i=j=0;i<mix->nitems + 1;i++) {

          if (mix->items[i] != NULL) {
            items_tmp[j] = mix->items[i];
            fields_tmp[j] = mix->fields[i];
            j++;
          }


        }

        free (mix->items);
        free (mix->fields);

        mix->items = items_tmp;
        mix->fields = fields_tmp;
      }

    }

    return;
}

static void mix_item_free (mix_item_t *item) {

  mix_widget_t *widg;
  widg = item->widget;
  int i;
  WINDOW *pw;

  /* Widget Free */

  for (i=0;i<widg->bar_cell_n;i++) {
    pw = panel_window (widg->bar_cell_p[i]);
    del_panel (widg->bar_cell_p[i]);
    delwin (pw);
  }

  free (widg->bar_cell_p);

  pw = panel_window (widg->bar_p);
  del_panel (widg->bar_p);
  delwin (pw);
  

  pw = panel_window (widg->main_p);
  del_panel (widg->main_p);
  delwin (pw);


  free_field (widg->field);
  free (item->widget);


  /* Port Free */

  free (item->port->name);
  free (item->port);
  

  free (item);

  return; 
}

static void mix_cleanup (mixer_t *mix) {

  int i;
  mix_item_t *item;
  WINDOW *pw;

  mix_form_destroy (mix);

  for (i=0;i<mix->nitems;i++) {
    item = mix->items[i];
    mix_item_free (item);
  }

  free (mix->items);
  free (mix->fields);

  pw = panel_window (mix->sub_p);
  del_panel (mix->sub_p);
  delwin (pw);

  pw = panel_window (mix->main_p);
  del_panel (mix->main_p);
  delwin (pw);  

  free (mix);  

  return;
}

static void mix_resize_handler (mixer_t *mix) {

  int rows,cols;
  int i,j;
  WINDOW *main_w,*sub_w,*bar_w,*pw,*bar_cell_w;
  mix_item_t *item;
  int bar_h,width;

  mix_form_destroy (mix);

  getmaxyx (stdscr,rows,cols);

  sub_w = panel_window (mix->sub_p);
  main_w = panel_window (mix->main_p);


  wclear (sub_w);
  touchwin (sub_w);
  wclear (main_w);
  touchwin (main_w);

  wresize (main_w,rows,cols);
  wresize (sub_w,3,cols);

  box (main_w,0,0);


  wclear (stdscr);
  touchwin (stdscr);


  bar_h = floor (rows*0.75) - 2;

  for (i=0;i<mix->nitems;i++) {

    item = mix->items[i];

    width = max ((strlen (item->port->name) + (strlen (item->port->name) % 2)),BAR_WIDTH);

    for (j=0;j<item->widget->bar_cell_n;j++) {
      pw = panel_window (item->widget->bar_cell_p[j]);
      del_panel (item->widget->bar_cell_p[j]);
      delwin (pw);
    }

    free (item->widget->bar_cell_p);

    bar_w = panel_window (item->widget->bar_p);
    main_w = panel_window (item->widget->main_p);

    wclear (bar_w);
    touchwin (bar_w);
    wclear (main_w);
    touchwin (main_w);

    wresize (bar_w,bar_h+2,BAR_WIDTH);
    wresize (main_w,bar_h + 2,width+2);

    wattron (bar_w,A_BOLD);
    box (bar_w,0,0);


    mvwaddch (bar_w,getmaxy (bar_w) - 3,0,ACS_LTEE);
    mvwaddch (bar_w,getmaxy (bar_w) - 3,1,ACS_HLINE);
    mvwaddch (bar_w,getmaxy (bar_w) - 3,2,ACS_HLINE);
    mvwaddch (bar_w,getmaxy (bar_w) - 3,3,ACS_RTEE);

    item->widget->bar_cell_n = bar_h-2;

    item->widget->bar_cell_p = calloc (item->widget->bar_cell_n,sizeof(PANEL*));

    for (j=0;j<item->widget->bar_cell_n;j++) {

      bar_cell_w = newwin (1,2,getbegy (bar_w)+1+j,1);
      wattron (bar_cell_w,COLOR_PAIR (2));
      waddch (bar_cell_w,' '|A_REVERSE);
      waddch (bar_cell_w,' '|A_REVERSE);
      item->widget->bar_cell_p[j] = new_panel (bar_cell_w);
      bar_cell_w = NULL;

    }

    mix_vol_update (item->port->vol,item);
    mix_txt_set (item);
  
  }

  mix_widgets_arrange (mix);

  mix_form_create (mix);

  return;
}

static void mix_item_destroy (mixer_t *mix, char *name) {

  int i;
  mix_item_t *item;

  for (i=0;i<mix->nitems;i++) {

    item = mix->items[i];

    if (!strcmp (name,item->port->name)) {
      mix_form_destroy (mix);
      mix_item_free (item);
      mix->items[i] = NULL;
      mix->fields[i] = NULL;

      break;
    }

  }

  mix->nitems--;
  mix_items_realloc (mix,1);
  mix->fields[mix->nitems] = NULL;
  mix_widgets_arrange (mix);
  mix_form_create (mix);

  return;
}

static mix_widget_t* mix_widget_create (mixer_t *mix, mix_port_t *port) {

  mix_widget_t *widget;
  WINDOW *mix_w,*widget_w,*bar_w,*bar_cell_w;
  int rows;
  int width;
  int bar_h;
  int i;


  mix_w = panel_window (mix->main_p);

  rows = getmaxy (mix_w);

  /* we want this to be even for centering things */
  width = max ((strlen (port->name) + (strlen (port->name) % 2)),BAR_WIDTH);

  widget = calloc (1,sizeof (mix_widget_t));

  widget->field = new_field (2,width,0,0,0,0);

  set_field_just (widget->field, JUSTIFY_CENTER);
  set_field_buffer (widget->field, 0, port->name);
  field_opts_off (widget->field, O_AUTOSKIP | O_EDIT); 


  bar_h = floor (rows*0.75) - 2;

  widget_w = newwin (bar_h + 2,width+2,0,0);

  //box (widget_w,0,0);

  widget->main_p = new_panel (widget_w);

  widget->bar_cell_n = bar_h-2;

  bar_w = newwin (bar_h+2,4,0,0);

  wattron (bar_w,A_BOLD);

  box (bar_w,0,0);


  mvwaddch (bar_w,getmaxy (bar_w) - 3,0,ACS_LTEE);
  mvwaddch (bar_w,getmaxy (bar_w) - 3,1,ACS_HLINE);
  mvwaddch (bar_w,getmaxy (bar_w) - 3,2,ACS_HLINE);
  mvwaddch (bar_w,getmaxy (bar_w) - 3,3,ACS_RTEE);

  widget->bar_p = new_panel (bar_w);


  widget->bar_cell_p = calloc (widget->bar_cell_n,sizeof(PANEL*));

  bar_cell_w = NULL;

  for (i=0;i<widget->bar_cell_n;i++) {

    bar_cell_w = newwin (1,2,getbegy (bar_w)+1+i,1);
    wattron (bar_cell_w,COLOR_PAIR (2));
    waddch (bar_cell_w,' '|A_REVERSE);
    waddch (bar_cell_w,' '|A_REVERSE);
    widget->bar_cell_p[i] = new_panel (bar_cell_w);
    bar_cell_w = NULL;

  }

  return widget;
}

static void mix_item_create (mixer_t *mix , kr_portgroup_t *krport) {

  mix_widget_t *widget;
  mix_port_t *port;
  mix_item_t *item;
  int i;

  if ((krport->direction == OUTPUT) && (krport->output_type == DIRECT)) {
    return;
  }

  /* check that we don't already have this item */

  for (i=0;i<mix->nitems;i++) {

    item = mix->items[i];

    if (item != NULL) {
      if (!strcmp (krport->name,item->port->name)) {
        return;
      }
    }

  }

  mix->nitems++;

  mix_items_realloc (mix,0);

  port = calloc (1,sizeof (mix_port_t));

  port->name = strdup (krport->name);
  port->vol = krport->volume[0];
  port->mute = 0;

  widget = mix_widget_create (mix,port);

  item = calloc (1,sizeof (mix_item_t));

  item->port = port;
  item->widget = widget;

  mix->items[mix->nitems-1] = item;

  mix->fields[mix->nitems-1] = widget->field;
  mix->fields[mix->nitems] = NULL;

  mix_widgets_arrange (mix);

  mix_form_create (mix);

  mix_txt_set (item);

  mix_vol_update (item->port->vol,item);

  return;  
} 

static mix_item_t* mix_item_from_addr (mixer_t *mix, kr_address_t *addr) {

  int i;
  mix_item_t *item;

  for (i=0;i<mix->nitems;i++) {

    item = mix->items[i];

    if (!strcmp (item->port->name,addr->id.name)) {
      return item;
    }

  }

  return NULL;

}


static int mix_delivery_handler (kr_client_t *client, mixer_t *mix) {

  kr_crate_t *crate;
  mix_item_t *item;
  crate = NULL;
  item = NULL;

  
  while ((kr_delivery_get (client, &crate) > 0) && (crate != NULL)) {


    if ((kr_crate_notice (crate) == EBML_ID_KRAD_RADIO_UNIT_DESTROYED) &&
        (crate->addr->path.unit == KR_MIXER) && (crate->addr->path.subunit.mixer_subunit == KR_PORTGROUP)) {

        mix_item_destroy (mix,crate->addr->id.name);
        kr_crate_recycle (&crate);

        continue;
    }

    if ((kr_crate_notice (crate) == EBML_ID_KRAD_SUBUNIT_CONTROL) && (crate->addr->path.unit == KR_MIXER)) {

        if (crate->addr->path.subunit.mixer_subunit == KR_PORTGROUP) {

            if (kr_crate_contains_float (crate)) {

              if (crate->addr->control.portgroup_control == KR_PEAK) {

                // handle peaks todo!

              } else {
                item = mix_item_from_addr (mix,crate->addr);
                mix_vol_update (crate->real,item);
                mix_txt_set (item);
              }

              continue;
              
            }
      

        }

        kr_crate_recycle (&crate);
        continue;
    }

    if (kr_crate_loaded (crate)) {

      if (crate->contains == KR_PORTGROUP) {

        mix_item_create (mix,crate->inside.portgroup);
      }

      kr_crate_recycle (&crate);
      continue;
    }



    kr_crate_recycle (&crate);
  }

  return 0;
}


int mixer_run (kr_client_t *client) {


  mixer_t *mix;
  WINDOW *main_w,*sub_w;

  int rows, cols;
  int ch = 0;
  int pollres;
  mix_port_t *cport;
  int fieldidx;


  mix = calloc (1,sizeof (mixer_t));

  mix->items = NULL;
  mix->fields = NULL;
  mix->nitems = 0;


  getmaxyx (stdscr,rows,cols);


  main_w = newwin (rows,cols,0,0); 
  sub_w = subwin (main_w,3,cols,rows * 0.95,1);

  box (main_w,0,0);

  keypad (main_w, TRUE);

  mix->main_p = new_panel (main_w);
  mix->sub_p = new_panel (sub_w);



  nodelay (main_w,TRUE);



  kr_mixer_portgroups (client);
  kr_subscribe_all (client);


  while (ch != 27) { 

    update_panels ();
    doupdate ();


    pollres = krm_poll (client,-1); 

    update_panels ();
    doupdate ();

    switch (pollres) {
      case 0:
        continue;
      case 1:
        ch = getch ();
        break;
      case 2:
        kr_delivery_recv (client);
        mix_delivery_handler (client,mix);
        continue;
      case -1:
        // error
        continue;
      case -2:
        mix_resize_handler (mix);
        continue;

    }


    switch (ch) { 


      case ERR:
        continue;
      case KEY_RESIZE:
        continue;
      case 'r':
        mix_resize_handler (mix);
        break;
      case KEY_LEFT:
        set_field_fore (current_field (mix->form),A_NORMAL); 
        form_driver (mix->form, REQ_PREV_FIELD);
        set_field_fore (current_field (mix->form),A_BOLD | COLOR_PAIR (4)); 
        break;
      case KEY_RIGHT:
        set_field_fore (current_field (mix->form),A_NORMAL); 
        form_driver (mix->form, REQ_NEXT_FIELD);
        set_field_fore (current_field (mix->form),A_BOLD | COLOR_PAIR (4)); 
        break;
      case KEY_UP:
        fieldidx = field_index (current_field (mix->form));
        cport =  mix->items[fieldidx]->port;
        if (!cport->mute) {
          kr_mixer_set_control (client,cport->name,"volume",cport->vol + 3, 0);
          mix_vol_update (cport->vol + 3,mix->items[fieldidx]);
          mix_txt_set (mix->items[fieldidx]);
        }
        else {
          mix_vol_update (cport->vol + 3,mix->items[fieldidx]);
          mix_txt_set (mix->items[fieldidx]);   
        }
        break;
      case KEY_DOWN:
        fieldidx = field_index (current_field (mix->form));
        cport =  mix->items[fieldidx]->port;
        if (!cport->mute) {
          kr_mixer_set_control (client,cport->name,"volume",cport->vol - 3, 0);
          mix_vol_update (cport->vol - 3,mix->items[fieldidx]);
          mix_txt_set (mix->items[fieldidx]);
        }
        else {
          mix_vol_update (cport->vol - 3,mix->items[fieldidx]);
          mix_txt_set (mix->items[fieldidx]);   
        }
        break;
      case 'm':
        fieldidx = field_index (current_field (mix->form));
        cport =  mix->items[fieldidx]->port;
        mix_mute (client,cport);
        mix_txt_set (mix->items[fieldidx]);  
        break;
      case 'e':
        eq_run (client,mix->items[field_index (current_field (mix->form))]->port->name);
        touchwin (panel_window (mix->main_p));
        break;
      default:
        break;
    }

  }

  mix_cleanup (mix);

  return 0;

}
