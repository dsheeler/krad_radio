#ifndef KRM_EQ_H
#define KRM_EQ_H
#include "kr_mixer.h"

typedef struct ctrls_show_St ctrls_show_t;
typedef struct db_show_St db_show_t;
typedef struct hz_show_St hz_show_t;
typedef struct bw_show_St bw_show_t;
typedef struct eq_saved_val_St eq_saved_val_t;
typedef struct eq_port_St eq_port_t;

struct db_show_St {

	PANEL *pan;
	PANEL **bar_p;
	PANEL **slider_p;

	WINDOW **bar_w;
	WINDOW **slider_w;

};

struct hz_show_St {

	PANEL *pan;
	PANEL **cell_p;

	WINDOW **cell_w;

};

struct bw_show_St {

	PANEL *pan;
	PANEL **bar_p;
	PANEL **slider_p;
	PANEL **cell_p;

	WINDOW **bar_w;
	WINDOW **slider_w;
	WINDOW **cell_w;

};

struct ctrls_show_St {

	db_show_t *db;
	hz_show_t *hz;
	bw_show_t *bw;

};

struct eq_saved_val_St {

	float db;
	float hz;
	float bw;

};

struct eq_port_St {

	ctrls_show_t *ctrls;
	eq_saved_val_t *vals;
	char *name;
	int band;

};

int eq_run (kr_client_t *client, char *sysname);
#endif