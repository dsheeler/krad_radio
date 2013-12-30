#include "krad_alsa.h"

#include "../tools/krad_debug.c"

static int test_alsa_card(int card_num) {
  kr_alsa *alsa;
  kr_alsa_info info;
  int ret;
  ret = 0;
  alsa = NULL;
  memset(&info, 0, sizeof(info));
  alsa = kr_alsa_create(card_num);
  if (alsa == NULL) {
    fprintf(stderr, "Could not create for alsa card %d\n", card_num);
    return -1;
  }
  printf("kr_alsa_create card num %d\n", card_num);

  ret = kr_alsa_destroy(alsa);
  printf("kr_alsa_destroy ret: %d\n", ret);

  printf("ALSA Card %d worked\n", card_num);
  return 0;
}

int main(int argc, char *argv[]) {
  int ret;
  int card_num;
  krad_debug_init(argv[0]);
  card_num = 0;
  if (argc == 2) {
    card_num = atoi(argv[1]);
  }
  ret = test_alsa_card(card_num);
  return ret;
}
