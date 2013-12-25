#include "krad_compositor_to_ebml.h"

int kr_compositor_path_type_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_compositor_path_type *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_compositor_path_type *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_compositor_subunit_type_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_compositor_subunit_type *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_compositor_subunit_type *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_vector_type_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_vector_type *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_vector_type *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_compositor_control_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_compositor_control *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_compositor_control *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, *actual);

  return res;
}

int kr_compositor_controls_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_compositor_controls *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_compositor_controls *)st;

  res += kr_ebml_pack_int32(ebml, 0xe1, actual->x);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->y);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->z);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->w);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->h);
  res += kr_ebml_pack_float(ebml, 0xe1, actual->rotation);
  res += kr_ebml_pack_float(ebml, 0xe1, actual->opacity);

  return res;
}

int kr_sprite_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  struct kr_sprite_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_sprite_info *)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->filename);
  res += kr_ebml_pack_int32(ebml, 0xe1, actual->rate);
  uber.actual = &(actual->controls);
  uber.type = EBML_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_text_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  struct kr_text_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_text_info *)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->string);
  res += kr_ebml_pack_string(ebml, 0xe1, actual->font);
  res += kr_ebml_pack_float(ebml, 0xe1, actual->red);
  res += kr_ebml_pack_float(ebml, 0xe1, actual->green);
  res += kr_ebml_pack_float(ebml, 0xe1, actual->blue);
  uber.actual = &(actual->controls);
  uber.type = EBML_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_vector_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  struct kr_vector_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_vector_info *)st;

  uber.actual = &(actual->type);
  uber.type = EBML_KR_VECTOR_TYPE;
  res += info_pack_to_ebml(&ebml[res],&uber);
  res += kr_ebml_pack_float(ebml, 0xe1, actual->red);
  res += kr_ebml_pack_float(ebml, 0xe1, actual->green);
  res += kr_ebml_pack_float(ebml, 0xe1, actual->blue);
  uber.actual = &(actual->controls);
  uber.type = EBML_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_compositor_path_info_to_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  struct kr_compositor_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_compositor_path_info *)st;

  res += kr_ebml_pack_string(ebml, 0xe1, actual->name);
  uber.actual = &(actual->type);
  uber.type = EBML_KR_COMPOSITOR_PATH_TYPE;
  res += info_pack_to_ebml(&ebml[res],&uber);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->width);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->height);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->crop_x);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->crop_y);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->crop_width);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->crop_height);
  uber.actual = &(actual->view);
  uber.type = EBML_KR_PERSPECTIVE_VIEW;
  res += info_pack_to_ebml(&ebml[res],&uber);
  uber.actual = &(actual->controls);
  uber.type = EBML_KR_COMPOSITOR_CONTROLS;
  res += info_pack_to_ebml(&ebml[res],&uber);

  return res;
}

int kr_compositor_info_to_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_compositor_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_compositor_info *)st;

  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->width);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->height);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->fps_numerator);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->fps_denominator);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->sprites);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->vectors);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->texts);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->inputs);
  res += kr_ebml_pack_uint32(ebml, 0xe1, actual->outputs);
  res += kr_ebml_pack_uint64(ebml, 0xe1, actual->frames);
  res += kr_ebml_pack_uint64(ebml, 0xe1, actual->timecode);

  return res;
}

