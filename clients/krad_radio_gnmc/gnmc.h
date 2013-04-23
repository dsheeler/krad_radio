typedef struct gnmc_St gnmc_t;
typedef struct box_St box_t;
typedef struct box_list_St box_list_t;
typedef enum box_id_type_En box_id_type_t;
typedef struct box_id_St box_id_t;
typedef struct screen_St screen_t;
typedef screen_t scr_t;
typedef struct info_St info_t;

#ifndef KR_GNMC_H
#define KR_GNMC_H
#include "kr_gnmc.h"
#include "util.h"
#include "crcache.h"
#include "screen.h"
#include "box.h"

#define UPDATE_INTERVAL 33
#define POLL_INTERVAL 15

#define STAT_SCR 0
#define MIX_SCR 1
#define COMP_SCR 2
#define TRANS_SCR 3

struct gnmc_St {
	char *name;
	screen_t **main_scrs;
	screen_t *top_screen;
	int32_t nscrs;
	info_t *info;
};

struct screen_St {
	char *name;
	PANEL *mainp;
	uint8_t focus;
	box_list_t **boxes;
	box_list_t *fbox;
  cr_cache_t *cache;
};

struct box_list_St {
	box_t *box;
	box_list_t *prev;
	box_list_t *next;
};

enum box_id_type_En {
	NAME = 0,
	ID = 1
};

union box_id_Un {
	char *name;
	int32_t id;
};

struct box_id_St {
  box_id_type_t type;
  union box_id_Un val;
};

struct box_St {
	box_id_t id;
	PANEL *pan;
	char *info;
	uint32_t maxy;
	uint8_t focus;
  uint8_t ro;
};

struct info_St {
  uint32_t deliveries;
  uint32_t crates[4];
  struct timeval conn_time;
  struct timeval disc_time;
};

int32_t gnmc_run (kr_client_t *client , char *sysname) ;

#endif

typedef kr_portgroup_t port_t;
typedef kr_compositor_t comp_t;
typedef kr_sprite_t sprite_t;
