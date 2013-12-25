#include "krad_compositor_to_json.h"

int kr_compositor_path_type_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_compositor_path_type *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_compositor_path_type *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_compositor_subunit_type_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_compositor_subunit_type *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_compositor_subunit_type *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_vector_type_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_vector_type *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_vector_type *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_compositor_control_to_json(char *json, void *st, int32_t max) {
  int res;
  kr_compositor_control *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_compositor_control *)st;

  res += snprintf(&json[res],max-res,"\"%u\"",*actual);

  return res;
}

int kr_compositor_controls_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_compositor_controls *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_compositor_controls *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"x\" : %d,",actual->x);
  res += snprintf(&json[res],max-res,"\"y\" : %d,",actual->y);
  res += snprintf(&json[res],max-res,"\"z\" : %u,",actual->z);
  res += snprintf(&json[res],max-res,"\"w\" : %u,",actual->w);
  res += snprintf(&json[res],max-res,"\"h\" : %u,",actual->h);
  res += snprintf(&json[res],max-res,"\"rotation\" : %0.2f,",actual->rotation);
  res += snprintf(&json[res],max-res,"\"opacity\" : %0.2f",actual->opacity);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_sprite_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int res;
  struct kr_sprite_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_sprite_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"filename\" : \"%s\",",actual->filename);
  res += snprintf(&json[res],max-res,"\"rate\" : %d,",actual->rate);
  res += snprintf(&json[res],max-res,"\"controls\": ");
  uber.actual = &(actual->controls);
  uber.type = JSON_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_text_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int res;
  struct kr_text_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_text_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"string\" : \"%s\",",actual->string);
  res += snprintf(&json[res],max-res,"\"font\" : \"%s\",",actual->font);
  res += snprintf(&json[res],max-res,"\"red\" : %0.2f,",actual->red);
  res += snprintf(&json[res],max-res,"\"green\" : %0.2f,",actual->green);
  res += snprintf(&json[res],max-res,"\"blue\" : %0.2f,",actual->blue);
  res += snprintf(&json[res],max-res,"\"controls\": ");
  uber.actual = &(actual->controls);
  uber.type = JSON_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_vector_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int res;
  struct kr_vector_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_vector_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"type\": ");
  uber.actual = &(actual->type);
  uber.type = JSON_KR_VECTOR_TYPE;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"red\" : %0.2f,",actual->red);
  res += snprintf(&json[res],max-res,"\"green\" : %0.2f,",actual->green);
  res += snprintf(&json[res],max-res,"\"blue\" : %0.2f,",actual->blue);
  res += snprintf(&json[res],max-res,"\"controls\": ");
  uber.actual = &(actual->controls);
  uber.type = JSON_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_compositor_path_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int res;
  struct kr_compositor_path_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_compositor_path_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"name\" : \"%s\",",actual->name);
  res += snprintf(&json[res],max-res,"\"type\": ");
  uber.actual = &(actual->type);
  uber.type = JSON_KR_COMPOSITOR_PATH_TYPE;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"width\" : %u,",actual->width);
  res += snprintf(&json[res],max-res,"\"height\" : %u,",actual->height);
  res += snprintf(&json[res],max-res,"\"crop_x\" : %u,",actual->crop_x);
  res += snprintf(&json[res],max-res,"\"crop_y\" : %u,",actual->crop_y);
  res += snprintf(&json[res],max-res,"\"crop_width\" : %u,",actual->crop_width);
  res += snprintf(&json[res],max-res,"\"crop_height\" : %u,",actual->crop_height);
  res += snprintf(&json[res],max-res,"\"view\": ");
  uber.actual = &(actual->view);
  uber.type = JSON_KR_PERSPECTIVE_VIEW;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,",");
  res += snprintf(&json[res],max-res,"\"controls\": ");
  uber.actual = &(actual->controls);
  uber.type = JSON_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

int kr_compositor_info_to_json(char *json, void *st, int32_t max) {
  int res;
  struct kr_compositor_info *actual;

  res = 0;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_compositor_info *)st;

  res += snprintf(&json[res],max-res,"{");
  res += snprintf(&json[res],max-res,"\"width\" : %u,",actual->width);
  res += snprintf(&json[res],max-res,"\"height\" : %u,",actual->height);
  res += snprintf(&json[res],max-res,"\"fps_numerator\" : %u,",actual->fps_numerator);
  res += snprintf(&json[res],max-res,"\"fps_denominator\" : %u,",actual->fps_denominator);
  res += snprintf(&json[res],max-res,"\"sprites\" : %u,",actual->sprites);
  res += snprintf(&json[res],max-res,"\"vectors\" : %u,",actual->vectors);
  res += snprintf(&json[res],max-res,"\"texts\" : %u,",actual->texts);
  res += snprintf(&json[res],max-res,"\"inputs\" : %u,",actual->inputs);
  res += snprintf(&json[res],max-res,"\"outputs\" : %u,",actual->outputs);
  res += snprintf(&json[res],max-res,"\"frames\" : %ju,",actual->frames);
  res += snprintf(&json[res],max-res,"\"timecode\" : %ju",actual->timecode);
  res += snprintf(&json[res],max-res,"}");

  return res;
}

