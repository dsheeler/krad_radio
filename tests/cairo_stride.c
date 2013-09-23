#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cairo/cairo.h>
#include <pixman.h>

/*
*  gcc -Wall cairo_stride.c -o cairo_stride `pkg-config --libs --cflags cairo
*  pixman-1`
*/

static void cairo_stride(int from, int to) {

  int i;
  int stride;
  int format;

  format = CAIRO_FORMAT_ARGB32;
  to++;
  for (i = from; i < to; i++) {
    stride = cairo_format_stride_for_width(format, i);
    if (stride != (i * 4)) {
      printf("Stride for %d is %d ( / 4 ) = %d\n", i, stride, stride / 4);
    }
  }
}

int main(int argc, char *argv[]) {

  if (argc == 2) {
    cairo_stride(1, atoi(argv[1]));
  } else {
    cairo_stride(1, 4096);
  }

  printf(" Pixman Version: %s\n", pixman_version_string());
  printf(" Cairo Version:  %s\n", cairo_version_string());

  return 0;
}
