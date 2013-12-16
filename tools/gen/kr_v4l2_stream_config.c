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
  if (config->get_val(config, "fps_num")) {
    params->fps_num = atoi(config->value);
  }
  if (config->get_val(config, "fps_den")) {
    params->fps_den = atoi(config->value);
  }
  if (config->get_val(config, "bitrate")) {
    params->bitrate = atoi(config->value);
  }
  if (config->get_val(config, "host")) {
    snprintf(params->host, sizeof(params->host), "%s", config->value);
  }
  if (config->get_val(config, "port")) {
    params->port = atoi(config->value);
  }
  if (config->get_val(config, "mount")) {
    snprintf(params->mount, sizeof(params->mount), "%s", config->value);
  }
  if (config->get_val(config, "password")) {
    snprintf(params->password, sizeof(params->password), "%s", config->value);
  }
  if (config->get_val(config, "device")) {
    params->device = atoi(config->value);
  }

  kr_config_close(config);

  return 0;
}

