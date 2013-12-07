#include "krad_v4l2.h"

int test_v4l2_device(int dev_num) {

  kr_v4l2 *v4l2;
  kr_v4l2_info *info;
  kr_v4l2_mode mode;
  int ret;
  kr_v4l2_setup setup;
  char device[256];

  ret = 0;
  v4l2 = NULL;
  info = NULL;
  memset(&mode, 0, sizeof(mode));

  snprintf(device, sizeof(device), "/dev/video%d", dev_num);
  setup.dev = dev_num;
  setup.priority = 0;
  mode.width = 640;
  mode.height = 360;
  mode.num = 30;
  mode.den = 1;

  v4l2 = kr_v4l2_create(&setup);
  if (v4l2 == NULL) {
    printf("kr_v4l2_create returned NULL\n");
    return -1;
  }

  ret = kr_v4l2_mode_set(v4l2, &mode);
  printf("kr_v4l2_mode_set ret: %d\n", ret);

/*  ret = kr_v4l2_stat(v4l2, &info);
  printf("kr_v4l2_stat ret: %d\n", ret); */

  ret = kr_v4l2_capture(v4l2, 1);
  printf("kr_v4l2_capture on ret: %d\n", ret);

/*  ret = kr_v4l2_stat(v4l2, &info);
  printf("kr_v4l2_stat ret: %d\n", ret); */

  ret = kr_v4l2_poll(v4l2, 1000);
  printf("kr_v4l2_poll ret: %d\n", ret);

/*  ret = kr_v4l2_stat(v4l2, &info);
  printf("kr_v4l2_stat ret: %d\n", ret); */

  ret = kr_v4l2_capture(v4l2, 0);
  printf("kr_v4l2_capture off ret: %d\n", ret);

/*  ret = kr_v4l2_stat(v4l2, &info);
  printf("kr_v4l2_stat ret: %d\n", ret); */

  ret = kr_v4l2_destroy(v4l2);
  printf("kr_v4l2_destroy ret: %d\n", ret);

  printf("%s worked\n", device);

  return 0;
}

int main(int argc, char *argv[]) {

  int i;
  int dev_count;

  i = 0;
  dev_count = kr_v4l2_dev_count();

  if (dev_count < 1) {
    fprintf(stderr, "No V4L2 devices detected..\n");
    return 1;
  }

  for (i = 0; i < dev_count; i++) {
    test_v4l2_device(i);
  }

  return 0;
}
