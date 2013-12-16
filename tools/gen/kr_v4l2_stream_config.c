#include "kr_v4l2_stream_config.h"
#include "../kr_config/kr_config.c"

int handle_config(struct kr_v4l2s_params *params, char *configname) {
  kr_cfg *config;
  config = kr_config_open(configname);

  if (config == NULL) {
    fprintf(stderr,"Error reading config!\n");
    return -1;
  }

  if (config->get_val(config, "width")) {
    params->width = atoi(config->value);
  }
  if (config->get_val(config, "height")) {
    params->height = atoi(config->value);
  }
  if (config->get_val(config, "fps_numerator")) {
    params->fps_numerator = atoi(config->value);
  }
  if (config->get_val(config, "fps_denominator")) {
    params->fps_denominator = atoi(config->value);
  }
  if (config->get_val(config, "video_bitrate")) {
    params->video_bitrate = atoi(config->value);
  }
  if (config->get_val(config, "host")) {
    params->host = strdup(config->value);
  }
  if (config->get_val(config, "port")) {
    params->port = atoi(config->value);
  }
  if (config->get_val(config, "mount")) {
    params->mount = strdup(config->value);
  }
  if (config->get_val(config, "password")) {
    params->password = strdup(config->value);
  }
  if (config->get_val(config, "device")) {
    params->device = strdup(config->value);
  }

  kr_config_close(config);

  return 0;
}

