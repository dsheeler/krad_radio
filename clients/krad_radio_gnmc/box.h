#ifndef KR_GNMC_BOX_H
#define KR_GNMC_BOX_H
#include "gnmc.h"
box_t *box_alloc (void);
void box_destroy (box_t *bx);
void box_push (box_list_t **boxes, box_t *bx);
void box_rmall (box_list_t **boxes);
void box_rm (box_list_t **boxes, box_t *bx);
box_t *box_get (box_list_t *boxes, box_id_t id);
box_t *box_create (screen_t *screen, box_id_type_t type, int32_t lines);
void box_set_name (box_t *box, const char *name);
int32_t box_cmp (box_id_t id1, box_id_t id2);
void box_reset_cur (box_t *box);
void box_redraw_all (box_list_t *boxes, screen_t *screen);
box_t *box_get_by_name (box_list_t *boxes, char *name);
void box_set_info (box_t *box, const char *info);
void box_print (box_t *box, const char *format , ... );
void box_clear (box_t *box);
void box_clearln (box_t *box);
void box_nl (box_t *box, int32_t x);
int32_t box_check_space (box_t *box, int32_t minrows, int32_t mincols);
void box_set_id (box_t *box, int32_t id);
box_t *box_get_by_id (box_list_t *boxes, int32_t id_val);
box_list_t *box_bottom_pos (box_list_t *boxes);
void box_focus (box_t *box);
void box_unfocus (box_t *box);
#endif