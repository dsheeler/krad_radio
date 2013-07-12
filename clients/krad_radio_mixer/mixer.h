#ifndef KRM_MIXER_H
#define KRM_MIXER_H
#include "kr_mixer.h"
#include "eq.h"

#define BAR_WIDTH 4

typedef struct mix_widget_St mix_widget_t;
typedef struct mix_item_St mix_item_t;
typedef struct mixer_St mixer_t;
typedef struct mix_port_St mix_port_t;

struct mix_port_St {

  int vol;
  int mute;
  char *name;

};

struct mix_widget_St {

	PANEL *main_p;
	PANEL *bar_p;
	PANEL **bar_cell_p;
	FIELD *field;
	int bar_cell_n;

};

struct mix_item_St {
	mix_widget_t *widget;
	mix_port_t *port;
};

struct mixer_St {
	PANEL *main_p;
	PANEL *sub_p;
	FORM *form;
	FIELD **fields;
	mix_item_t **items;
	int nitems;
};

int mixer_run (kr_client_t *client);
#endif
