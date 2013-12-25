#include "krad_compositor_from_ebml.h"

int kr_compositor_path_type_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_compositor_path_type *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_compositor_path_type *)st;

  res += kr_ebml2_unpack_element_int32(ebml, NULL, actual);

  return res;
}

int kr_compositor_subunit_type_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_compositor_subunit_type *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_compositor_subunit_type *)st;

  res += kr_ebml2_unpack_element_int32(ebml, NULL, actual);

  return res;
}

int kr_vector_type_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_vector_type *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_vector_type *)st;

  res += kr_ebml2_unpack_element_int32(ebml, NULL, actual);

  return res;
}

int kr_compositor_control_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  kr_compositor_control *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (kr_compositor_control *)st;

  res += kr_ebml2_unpack_element_int32(ebml, NULL, actual);

  return res;
}

int kr_compositor_controls_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_compositor_controls *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_compositor_controls *)st;

  res += kr_ebml2_unpack_element_int32(ebml, NULL, &actual->x);
  res += kr_ebml2_unpack_element_int32(ebml, NULL, &actual->y);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->z);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->w);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->h);
  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->rotation);
  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->opacity);

  return res;
}

int kr_sprite_info_fr_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  struct kr_sprite_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_sprite_info *)st;

  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->filename , sizeof(actual->filename));
  res += kr_ebml2_unpack_element_int32(ebml, NULL, &actual->rate);
  uber.actual = &(actual->controls);
  uber.type = DEBML_KR_COMPOSITOR_CONTROLS;
  res += info_unpack_fr_ebml(&ebml[res],&uber);

  return res;
}

int kr_text_info_fr_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  struct kr_text_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_text_info *)st;

  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->string , sizeof(actual->string));
  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->font , sizeof(actual->font));
  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->red);
  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->green);
  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->blue);
  uber.actual = &(actual->controls);
  uber.type = DEBML_KR_COMPOSITOR_CONTROLS;
  res += info_unpack_fr_ebml(&ebml[res],&uber);

  return res;
}

int kr_vector_info_fr_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  struct kr_vector_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_vector_info *)st;

  uber.actual = &(actual->type);
  uber.type = DEBML_KR_VECTOR_TYPE;
  res += info_unpack_fr_ebml(&ebml[res],&uber);
  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->red);
  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->green);
  res += kr_ebml2_unpack_element_float(ebml, NULL, &actual->blue);
  uber.actual = &(actual->controls);
  uber.type = DEBML_KR_COMPOSITOR_CONTROLS;
  res += info_unpack_fr_ebml(&ebml[res],&uber);

  return res;
}

int kr_compositor_path_info_fr_ebml(kr_ebml *ebml, void *st) {
  uber_St uber;
  int res;
  struct kr_compositor_path_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_compositor_path_info *)st;

  res += kr_ebml2_unpack_element_string(ebml, NULL, actual->name , sizeof(actual->name));
  uber.actual = &(actual->type);
  uber.type = DEBML_KR_COMPOSITOR_PATH_TYPE;
  res += info_unpack_fr_ebml(&ebml[res],&uber);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->width);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->height);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->crop_x);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->crop_y);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->crop_width);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->crop_height);
  uber.actual = &(actual->view);
  uber.type = DEBML_KR_PERSPECTIVE_VIEW;
  res += info_unpack_fr_ebml(&ebml[res],&uber);
  uber.actual = &(actual->controls);
  uber.type = DEBML_KR_COMPOSITOR_CONTROLS;
  res += info_unpack_fr_ebml(&ebml[res],&uber);

  return res;
}

int kr_compositor_info_fr_ebml(kr_ebml *ebml, void *st) {
  int res;
  struct kr_compositor_info *actual;

  res = 0;

  if ((ebml == NULL) || (st == NULL)) {
    return -1;
  }

  actual = (struct kr_compositor_info *)st;

  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->width);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->height);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->fps_numerator);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->fps_denominator);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->sprites);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->vectors);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->texts);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->inputs);
  res += kr_ebml2_unpack_element_uint32(ebml, NULL, &actual->outputs);
  res += kr_ebml2_unpack_element_uint64(ebml, NULL, &actual->frames);
  res += kr_ebml2_unpack_element_uint64(ebml, NULL, &actual->timecode);

  return res;
}

