#include "krad_alsa.h"

struct kr_alsa {
  kr_alsa_info info;
};

int kr_alsa_destroy(kr_alsa *alsa) {
  if (!alsa) return -1;
  printk("Krad ALSA: Destroyed card %d: %s", alsa->info.card,
   alsa->info.name);
  free(alsa);
  return 0;
}

kr_alsa *kr_alsa_create(int card) {
  kr_alsa *alsa;
  int ret;
  char *name;
  kr_alsa_info *info;
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

  char **hints;
  ret = snd_device_name_hint(info->card, "pcm", (void***)&hints);
  if (ret == 0) {
    char** n = hints;
    while (*n != NULL) {
      name = snd_device_name_get_hint(*n, "NAME");
      if (name != NULL && 0 != strcmp("null", name)) {
        printk("pcm name: %s\n", name);
        free(name);
      }
      name = snd_device_name_get_hint(*n, "DESC");
      if (name != NULL && 0 != strcmp("null", name)) {
        printk("pcm desc: %s\n", name);
        free(name);
      }
      name = snd_device_name_get_hint(*n, "IOID");
      if (name != NULL && 0 != strcmp("null", name)) {
        printk("pcm ioid: %s\n", name);
        free(name);
      }
      n++;
    }
    snd_device_name_free_hint((void**)hints);
  }
  return alsa;
}
