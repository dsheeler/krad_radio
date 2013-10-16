#include "hexutils.h"

void print_usage() {
  printf("hextool file1 file2 ...\n");
  printf("Takes a list of filenames and generates const data headers.\n");
  return;
}

int main(int argc, char *argv[]) {

  int nfiles;

  nfiles = argc - 1;

  if (!nfiles) {
    print_usage();
    return 0;
  }

  hexgen(&argv[1], nfiles);

  printf("All done!\n");

}
