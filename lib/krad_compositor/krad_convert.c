#include "krad_convert.h"

void kr_image_convert_init(kr_convert *conv) {
  if (conv == NULL) return;
  memset(conv, 0, sizeof(kr_convert));
  conv->quality = SWS_BILINEAR;
}

void kr_image_convert_clear(kr_convert *conv) {
  if (conv == NULL) return;
  if (conv->sws != NULL) {
    sws_freeContext(conv->sws);
  }
  kr_image_convert_init(conv);
}

int kr_image_convert(kr_convert *conv, kr_image *dst, kr_image *src) {

  int ret;
  int src_w;
  int src_h;
  int src_bpp;
  uint8_t *src_ppx[4];
  int32_t src_strides[4];

  if (conv == NULL) return -2;
  if (src == NULL) return -3;
  if (dst == NULL) return -4;

  src_bpp = 2;

  src_w = src->w;
  src_h = src->h;
  src_ppx[0] = src->ppx[0];
  src_ppx[1] = src->ppx[1];
  src_ppx[2] = src->ppx[2];
  src_ppx[3] = src->ppx[3];
  src_strides[0] = src->pps[0];
  src_strides[0] = src_bpp * src_w;
  src_strides[1] = src->pps[1];
  src_strides[2] = src->pps[2];
  src_strides[3] = src->pps[3];

  conv->sws = sws_getCachedContext(conv->sws, src_w, src_h, src->fmt, dst->w,
   dst->h, dst->fmt, conv->quality, NULL, NULL, NULL);
  if (conv->sws == NULL) {
    printke("Krad Converter: could not sws_getCachedContext");
    printke("source: %dx%d--%d dest:  %dx%d--%d", src_w, src_h,
     src_strides[0], dst->w, dst->h, dst->pps[0]);
    return -5;
  }
  /*printk("source: %dx%d--%d-%d dest:  %dx%d--%d-%d", src_w, src_h,
   src_strides[0], src_strides[1], dst->w, dst->h, dst->pps[0], dst->pps[1]);*/
  ret = sws_scale(conv->sws, (const uint8_t * const*)src_ppx, src_strides, 0,
   src_h, dst->ppx, dst->pps);
  if (ret == dst->h) {
    return 0;
  }
  return ret;
}
