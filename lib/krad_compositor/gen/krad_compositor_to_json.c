#include "krad_compositor_to_json.h"

int kr_compositor_path_type_to_json(char *json, void *st, int32_t max) {
  int res;
  res = 0;
  kr_compositor_path_type *actual;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_compositor_path_type*)st;

  res += snprintf(&json[res],max,"\"%u\"",*actual);

  return res;
}

int kr_compositor_subunit_type_to_json(char *json, void *st, int32_t max) {
  int res;
  res = 0;
  kr_compositor_subunit_type *actual;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_compositor_subunit_type*)st;

  res += snprintf(&json[res],max,"\"%u\"",*actual);

  return res;
}

int kr_vector_type_to_json(char *json, void *st, int32_t max) {
  int res;
  res = 0;
  kr_vector_type *actual;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_vector_type*)st;

  res += snprintf(&json[res],max,"\"%u\"",*actual);

  return res;
}

int kr_compositor_control_to_json(char *json, void *st, int32_t max) {
  int res;
  res = 0;
  kr_compositor_control *actual;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (kr_compositor_control*)st;

  res += snprintf(&json[res],max,"\"%u\"",*actual);

  return res;
}

int kr_compositor_controls_to_json(char *json, void *st, int32_t max) {
  int res;
  res = 0;
  struct kr_compositor_controls *actual;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_compositor_controls*)st;

  res += snprintf(&json[res],max,"{");
  res += snprintf(&json[res],max,"\"x\" : %d,",actual->x);
  res += snprintf(&json[res],max,"\"y\" : %d,",actual->y);
  res += snprintf(&json[res],max,"\"z\" : %u,",actual->z);
  res += snprintf(&json[res],max,"\"w\" : %u,",actual->w);
  res += snprintf(&json[res],max,"\"h\" : %u,",actual->h);
  res += snprintf(&json[res],max,"\"rotation\" : %0.2f,",actual->rotation);
  res += snprintf(&json[res],max,"\"opacity\" : %0.2f",actual->opacity);
  res += snprintf(&json[res],max,"}");

  return res;
}

int kr_sprite_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int i;
  int res;
  res = 0;
  struct kr_sprite_info *actual;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_sprite_info*)st;

  res += snprintf(&json[res],max,"{");
  res += snprintf(&json[res],max,"\"filename\" : \"%s\",",actual->filename);
  res += snprintf(&json[res],max,"\"rate\" : %d,",actual->rate);
  res += snprintf(&json[res],max,"\"controls\": ");
  uber.actual = &(actual->controls);
  uber.type = JSON_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max,"}");

  return res;
}

int kr_text_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int i;
  int res;
  res = 0;
  struct kr_text_info *actual;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_text_info*)st;

  res += snprintf(&json[res],max,"{");
  res += snprintf(&json[res],max,"\"string\" : \"%s\",",actual->string);
  res += snprintf(&json[res],max,"\"font\" : \"%s\",",actual->font);
  res += snprintf(&json[res],max,"\"red\" : %0.2f,",actual->red);
  res += snprintf(&json[res],max,"\"green\" : %0.2f,",actual->green);
  res += snprintf(&json[res],max,"\"blue\" : %0.2f,",actual->blue);
  res += snprintf(&json[res],max,"\"controls\": ");
  uber.actual = &(actual->controls);
  uber.type = JSON_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max,"}");

  return res;
}

int kr_vector_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int res;
  res = 0;
  struct kr_vector_info *actual;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_vector_info*)st;

  res += snprintf(&json[res],max,"{");
  res += snprintf(&json[res],max,"\"type\": ");
  uber.actual = &(actual->type);
  uber.type = JSON_KR_VECTOR_TYPE;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max,",");
  res += snprintf(&json[res],max,"\"red\" : %0.2f,",actual->red);
  res += snprintf(&json[res],max,"\"green\" : %0.2f,",actual->green);
  res += snprintf(&json[res],max,"\"blue\" : %0.2f,",actual->blue);
  res += snprintf(&json[res],max,"\"controls\": ");
  uber.actual = &(actual->controls);
  uber.type = JSON_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max,"}");

  return res;
}

int kr_compositor_path_info_to_json(char *json, void *st, int32_t max) {
  uber_St uber;
  int i;
  int res;
  res = 0;
  struct kr_compositor_path_info *actual;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_compositor_path_info*)st;

  res += snprintf(&json[res],max,"{");
  res += snprintf(&json[res],max,"\"name\" : \"%s\",",actual->name);
  res += snprintf(&json[res],max,"\"type\": ");
  uber.actual = &(actual->type);
  uber.type = JSON_KR_COMPOSITOR_PATH_TYPE;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max,",");
  res += snprintf(&json[res],max,"\"width\" : %u,",actual->width);
  res += snprintf(&json[res],max,"\"height\" : %u,",actual->height);
  res += snprintf(&json[res],max,"\"crop_x\" : %u,",actual->crop_x);
  res += snprintf(&json[res],max,"\"crop_y\" : %u,",actual->crop_y);
  res += snprintf(&json[res],max,"\"crop_width\" : %u,",actual->crop_width);
  res += snprintf(&json[res],max,"\"crop_height\" : %u,",actual->crop_height);
  res += snprintf(&json[res],max,"\"view\": ");
  uber.actual = &(actual->view);
  uber.type = JSON_KR_PERSPECTIVE_VIEW;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max,",");
  res += snprintf(&json[res],max,"\"controls\": ");
  uber.actual = &(actual->controls);
  uber.type = JSON_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_json(&json[res],&uber,max-res);
  res += snprintf(&json[res],max,"}");

  return res;
}

int kr_compositor_info_to_json(char *json, void *st, int32_t max) {
  int res;
  res = 0;
  struct kr_compositor_info *actual;

  if ((json == NULL) || (st == NULL) || (max < 1)) {
    return -1;
  }

  actual = (struct kr_compositor_info*)st;

  res += snprintf(&json[res],max,"{");
  res += snprintf(&json[res],max,"\"width\" : %u,",actual->width);
  res += snprintf(&json[res],max,"\"height\" : %u,",actual->height);
  res += snprintf(&json[res],max,"\"fps_numerator\" : %u,",actual->fps_numerator);
  res += snprintf(&json[res],max,"\"fps_denominator\" : %u,",actual->fps_denominator);
  res += snprintf(&json[res],max,"\"sprites\" : %u,",actual->sprites);
  res += snprintf(&json[res],max,"\"vectors\" : %u,",actual->vectors);
  res += snprintf(&json[res],max,"\"texts\" : %u,",actual->texts);
  res += snprintf(&json[res],max,"\"inputs\" : %u,",actual->inputs);
  res += snprintf(&json[res],max,"\"outputs\" : %u,",actual->outputs);
  res += snprintf(&json[res],max,"\"frames\" : %ju,",actual->frames);
  res += snprintf(&json[res],max,"\"timecode\" : %ju",actual->timecode);
  res += snprintf(&json[res],max,"}");

  return res;
}

