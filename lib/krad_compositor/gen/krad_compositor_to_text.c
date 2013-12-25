#include "krad_compositor_to_text.h"

int kr_compositor_path_type_to_text(char *text, void *st, int32_t max) {
  int res;
  kr_compositor_path_type *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_compositor_path_type *)st;

  res += snprintf(&text[res],max-res,"kr_compositor_path_type : %u \n",*actual);

  return res;
}

int kr_compositor_subunit_type_to_text(char *text, void *st, int32_t max) {
  int res;
  kr_compositor_subunit_type *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_compositor_subunit_type *)st;

  res += snprintf(&text[res],max-res,"kr_compositor_subunit_type : %u \n",*actual);

  return res;
}

int kr_vector_type_to_text(char *text, void *st, int32_t max) {
  int res;
  kr_vector_type *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_vector_type *)st;

  res += snprintf(&text[res],max-res,"kr_vector_type : %u \n",*actual);

  return res;
}

int kr_compositor_control_to_text(char *text, void *st, int32_t max) {
  int res;
  kr_compositor_control *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_compositor_control *)st;

  res += snprintf(&text[res],max-res,"kr_compositor_control : %u \n",*actual);

  return res;
}

int kr_compositor_controls_to_text(char *text, void *st, int32_t max) {
  int res;
  struct kr_compositor_controls *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_compositor_controls *)st;

  res += snprintf(&text[res],max-res,"x : %d \n",actual->x);
  res += snprintf(&text[res],max-res,"y : %d \n",actual->y);
  res += snprintf(&text[res],max-res,"z : %u \n",actual->z);
  res += snprintf(&text[res],max-res,"w : %u \n",actual->w);
  res += snprintf(&text[res],max-res,"h : %u \n",actual->h);
  res += snprintf(&text[res],max-res,"rotation : %0.2f \n",actual->rotation);
  res += snprintf(&text[res],max-res,"opacity : %0.2f \n",actual->opacity);

  return res;
}

int kr_sprite_info_to_text(char *text, void *st, int32_t max) {
  uber_St uber;
  int res;
  struct kr_sprite_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_sprite_info *)st;

  res += snprintf(&text[res],max-res,"filename : %s \n",actual->filename);
  res += snprintf(&text[res],max-res,"rate : %d \n",actual->rate);
  uber.actual = &(actual->controls);
  uber.type = TEXT_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_text(&text[res],&uber,max-res);

  return res;
}

int kr_text_info_to_text(char *text, void *st, int32_t max) {
  uber_St uber;
  int res;
  struct kr_text_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_text_info *)st;

  res += snprintf(&text[res],max-res,"string : %s \n",actual->string);
  res += snprintf(&text[res],max-res,"font : %s \n",actual->font);
  res += snprintf(&text[res],max-res,"red : %0.2f \n",actual->red);
  res += snprintf(&text[res],max-res,"green : %0.2f \n",actual->green);
  res += snprintf(&text[res],max-res,"blue : %0.2f \n",actual->blue);
  uber.actual = &(actual->controls);
  uber.type = TEXT_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_text(&text[res],&uber,max-res);

  return res;
}

int kr_vector_info_to_text(char *text, void *st, int32_t max) {
  uber_St uber;
  int res;
  struct kr_vector_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_vector_info *)st;

  uber.actual = &(actual->type);
  uber.type = TEXT_KR_VECTOR_TYPE;
  res += info_pack_to_text(&text[res],&uber,max-res);
  res += snprintf(&text[res],max-res,"red : %0.2f \n",actual->red);
  res += snprintf(&text[res],max-res,"green : %0.2f \n",actual->green);
  res += snprintf(&text[res],max-res,"blue : %0.2f \n",actual->blue);
  uber.actual = &(actual->controls);
  uber.type = TEXT_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_text(&text[res],&uber,max-res);

  return res;
}

int kr_compositor_path_info_to_text(char *text, void *st, int32_t max) {
  uber_St uber;
  int res;
  struct kr_compositor_path_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_compositor_path_info *)st;

  res += snprintf(&text[res],max-res,"name : %s \n",actual->name);
  uber.actual = &(actual->type);
  uber.type = TEXT_KR_COMPOSITOR_PATH_TYPE;
  res += info_pack_to_text(&text[res],&uber,max-res);
  res += snprintf(&text[res],max-res,"width : %u \n",actual->width);
  res += snprintf(&text[res],max-res,"height : %u \n",actual->height);
  res += snprintf(&text[res],max-res,"crop_x : %u \n",actual->crop_x);
  res += snprintf(&text[res],max-res,"crop_y : %u \n",actual->crop_y);
  res += snprintf(&text[res],max-res,"crop_width : %u \n",actual->crop_width);
  res += snprintf(&text[res],max-res,"crop_height : %u \n",actual->crop_height);
  uber.actual = &(actual->view);
  uber.type = TEXT_KR_PERSPECTIVE_VIEW;
  res += info_pack_to_text(&text[res],&uber,max-res);
  uber.actual = &(actual->controls);
  uber.type = TEXT_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_text(&text[res],&uber,max-res);

  return res;
}

int kr_compositor_info_to_text(char *text, void *st, int32_t max) {
  int res;
  struct kr_compositor_info *actual;

  res = 0;

  if ((text == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_compositor_info *)st;

  res += snprintf(&text[res],max-res,"width : %u \n",actual->width);
  res += snprintf(&text[res],max-res,"height : %u \n",actual->height);
  res += snprintf(&text[res],max-res,"fps_numerator : %u \n",actual->fps_numerator);
  res += snprintf(&text[res],max-res,"fps_denominator : %u \n",actual->fps_denominator);
  res += snprintf(&text[res],max-res,"sprites : %u \n",actual->sprites);
  res += snprintf(&text[res],max-res,"vectors : %u \n",actual->vectors);
  res += snprintf(&text[res],max-res,"texts : %u \n",actual->texts);
  res += snprintf(&text[res],max-res,"inputs : %u \n",actual->inputs);
  res += snprintf(&text[res],max-res,"outputs : %u \n",actual->outputs);
  res += snprintf(&text[res],max-res,"frames : %ju \n",actual->frames);
  res += snprintf(&text[res],max-res,"timecode : %ju \n",actual->timecode);

  return res;
}

