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

  if (conv == NULL) return -2;
  if (src == NULL) return -3;
  if (dst == NULL) return -4;
/*
  conv->sws = sws_getCachedContext(conv->sws, src_w, src_h, src->format,
   dst->width, dst->height, dst->format, conv->quality, NULL, NULL, NULL);

  if (converter == NULL) {
    printke("Krad v4l2s: could not sws_getCachedContext");
    return -5;
  }

  vmedium->v.pps[0] = v4l2s->params->width;
  vmedium->v.pps[1] = v4l2s->params->width/2;
  vmedium->v.pps[2] = v4l2s->params->width/2;
  vmedium->v.ppx[0] = vmedium->data;
  vmedium->v.ppx[1] = vmedium->data + v4l2s->params->width * (v4l2s->params->height);
  vmedium->v.ppx[2] = vmedium->data + v4l2s->params->width *
                     (v4l2s->params->height) +
                     ((v4l2s->params->width * (v4l2s->params->height)) /4);

  sws_scale(converter, (const uint8_t * const*)src->px,
   src->pps, crop_y, crop_height, dst->ppx, vmedium->v.pps);
*/
  return 0;
}
