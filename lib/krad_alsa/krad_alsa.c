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
  char *pcm_name;
  char *pcm_subname;
  int pcm_device;
  int pcm_playback;
  int pcm_capture;
  snd_pcm_info_t *pcm_info;
  kr_alsa_info *info;
  pcm_device = -1;
  alsa = calloc(1, sizeof(kr_alsa));
  if (!alsa) return NULL;
  info = &alsa->info;
  info->card = card;
  sprintf(dev_name, "hw:%d", info->card);
  snd_pcm_info_alloca(&pcm_info);


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


  printk("Krad ALSA: Created card %d: %s -- %s", info->card, info->name,
   info->longname);
  ret = snd_ctl_open(&alsa->ctl, dev_name, 0);
  if (ret != 0) {
    printk("Could not open %s", dev_name);
    return alsa;
  }
  do {
    ret = snd_ctl_pcm_next_device(alsa->ctl, &pcm_device);
    if ((ret == 0) && (pcm_device >= 0)) {
      pcm_capture = 0;
      pcm_playback = 0;
      memset(pcm_info, 0, snd_pcm_info_sizeof());
      snd_pcm_info_set_device(pcm_info, pcm_device);
      snd_pcm_info_set_stream(pcm_info, SND_PCM_STREAM_PLAYBACK);
      snd_pcm_info_set_subdevice(pcm_info, 0);
      ret = snd_ctl_pcm_info(alsa->ctl, pcm_info);
      if (ret == 0) {
        pcm_playback = 1;
      }
      memset(pcm_info, 0, snd_pcm_info_sizeof());
      snd_pcm_info_set_device(pcm_info, pcm_device);
      snd_pcm_info_set_stream(pcm_info, SND_PCM_STREAM_CAPTURE);
      snd_pcm_info_set_subdevice(pcm_info, 0);
      ret = snd_ctl_pcm_info(alsa->ctl, pcm_info);
      if (ret == 0) {
        pcm_capture = 1;
      }
      pcm_name = snd_pcm_info_get_name(pcm_info);
      pcm_subname = snd_pcm_info_get_subdevice_name(pcm_info);
      printk("PCM %d: %s - %s Capture: %d Playback: %d", pcm_device, pcm_name,
       pcm_subname, pcm_capture, pcm_playback);
    }
  } while (pcm_device >= 0);
  return alsa;
}
