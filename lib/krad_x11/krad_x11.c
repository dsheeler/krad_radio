#include "krad_x11.h"

void kr_x11_destroy(kr_x11 *x11) {
  if (x11->capture_enabled == 1) {
    kr_x11_disable_capture(x11);
  }
  free(x11);
}

kr_x11 *kr_x11_create() {
  kr_x11 *x11;
  int s;
  x11 = calloc(1, sizeof (kr_x11));
  x11->connection = xcb_connect(NULL, &x11->screen_number);
  x11->iter = xcb_setup_roots_iterator(xcb_get_setup(x11->connection));
  s = x11->screen_number;
  for (; x11->iter.rem; --s, xcb_screen_next(&x11->iter)) {
    if (s == 0) {
      x11->screen = x11->iter.data;
      x11->screen_width = x11->screen->width_in_pixels;
      x11->screen_height = x11->screen->height_in_pixels;
      x11->screen_bit_depth = x11->screen->root_depth;
    }
  }
  return x11;
}

void kr_x11_enable_capture(kr_x11 *x11, uint32_t window_id) {
  xcb_get_geometry_reply_t *geo;
  xcb_query_tree_reply_t *tree;
  xcb_translate_coordinates_cookie_t translateCookie;
  xcb_translate_coordinates_reply_t *trans;
  geo = xcb_get_geometry_reply(x11->connection,
        xcb_get_geometry(x11->connection, window_id), NULL);
  if (geo == NULL) {
    window_id = 0;
  } else {
    tree = xcb_query_tree_reply(x11->connection,
     xcb_query_tree(x11->connection, window_id), NULL);
    if (tree == NULL) {
      window_id = 0;
      free(geo);
    } else {
      translateCookie = xcb_translate_coordinates(x11->connection, window_id,
       x11->screen->root, geo->x, geo->y);
      trans = xcb_translate_coordinates_reply(x11->connection, translateCookie,
       NULL);
      if (trans == NULL) {
        window_id = 0;
        free(tree);
        free(geo);
      } else {
        x11->width = geo->width;
        x11->height = geo->height;
        x11->x = trans->dst_x - geo->x;
        x11->y = trans->dst_y - geo->y;
        free(trans);
        free(tree);
        free(geo);
      }
    }
  }
  if (window_id == 0) {
    x11->width = x11->screen_width;
    x11->height = x11->screen_height;
    x11->window = x11->screen->root;
  }
  x11->img = xcb_image_create_native(x11->connection,x11->width, x11->height,
   XCB_IMAGE_FORMAT_Z_PIXMAP, x11->screen_bit_depth, 0, ~0, 0);
  if (!x11->img) {
    exit(15);
  }
  x11->stride = x11->img->stride;
  x11->shminfo.shmid = shmget(IPC_PRIVATE, x11->img->stride * x11->img->height,
   (IPC_CREAT | 0666));
  if (x11->shminfo.shmid == (uint32_t)-1) {
    xcb_image_destroy(x11->img);
    failfast("shminfo fail");
  }
  x11->shminfo.shmaddr = shmat(x11->shminfo.shmid, 0, 0);
  x11->img->data = x11->shminfo.shmaddr;
  if (x11->img->data == (uint8_t *)-1) {
    xcb_image_destroy(x11->img);
    failfast("xcb image fail");
  }
  x11->shminfo.shmseg = xcb_generate_id(x11->connection);
  xcb_shm_attach(x11->connection, x11->shminfo.shmseg, x11->shminfo.shmid, 0);
  x11->capture_enabled = 1;
}

void kr_x11_disable_capture(kr_x11 *x11) {
  if (x11->capture_enabled == 1) {
    xcb_shm_detach(x11->connection, x11->shminfo.shmseg);
    xcb_image_destroy(x11->img);
    shmdt(x11->shminfo.shmaddr);
    shmctl(x11->shminfo.shmid, IPC_RMID, 0);
    x11->capture_enabled = 0;
  }
}

int kr_x11_capture(kr_x11 *x11, kr_image *image) {
  if ((x11 == NULL) || (image == NULL)) {
    return 0;
  }
  x11->number = xcb_image_shm_get(x11->connection, x11->screen->root,
   x11->img, x11->shminfo, x11->x, x11->y, 0xffffffff);
  x11->reply = xcb_shm_get_image_reply(x11->connection, x11->cookie, NULL);
  if (x11->reply) {
    free(x11->reply);
  }
  memset(image, 0, sizeof(kr_image));
  image->px = x11->img->data;
  image->w = x11->width;
  image->h = x11->height;
  image->ppx[0] = image->px;
  image->pps[0] = x11->stride;
  image->fmt = PIX_FMT_RGB32;
  return 1;
}
