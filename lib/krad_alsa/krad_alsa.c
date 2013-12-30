#include "krad_alsa.h"

struct kr_alsa {
  kr_alsa_info info;
  snd_ctl_t *ctl;
};

int kr_alsa_destroy(kr_alsa *alsa) {
  if (!alsa) return -1;
  if (alsa->ctl) {
    snd_ctl_close(alsa->ctl);
    alsa->ctl = NULL;
  }
  printk("Krad ALSA: Destroyed card %d: %s", alsa->info.card,
   alsa->info.name);
  free(alsa);
  return 0;
}

kr_alsa *kr_alsa_create(int card) {
  kr_alsa *alsa;
  int ret;
  char *name;
  char dev_name[8];
  int sub_device;
  kr_alsa_info *info;
  sub_device = 0;
  alsa = calloc(1, sizeof(kr_alsa));
  if (!alsa) return NULL;
  info = &alsa->info;
  info->card = card;
  ret = snd_card_get_name(info->card, &name);
  if (ret == 0) {
    strncpy(info->name, name, sizeof(info->name));
    free(name);
  }
  ret = snd_card_get_longname(info->card, &name);
  if (ret == 0) {
    strncpy(info->longname, name, sizeof(info->longname));
    free(name);
  }
  printk("Krad ALSA: Created card %d: %s -- %s\n", info->card, info->name,
   info->longname);

  sprintf(dev_name, "hw:%d", info->card);
  ret = snd_ctl_open(&alsa->ctl, dev_name, sub_device);
  if (ret == 0) {
    ret = snd_ctl_pcm_next_device(alsa->ctl, &sub_device);
    if (sub_device >= 0) {
      printf("got subdevice! %d\n", sub_device);
    }
  }
  return alsa;
}
