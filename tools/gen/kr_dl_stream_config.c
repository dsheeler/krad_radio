#include "kr_dl_stream_config.h"
#include "../kr_config/kr_config.c"

int handle_config(struct kr_dlstream_params *params, char *configname) {
  kr_cfg *config;
  config = kr_config_open(configname);

  if (config == NULL) {
    fprintf(stderr,"Error reading config!\n");
    return -1;
  }

  if (config->get_val(config, "input_width")) {
    params->input_width = atoi(config->value);
  }
  if (config->get_val(config, "input_height")) {
    params->input_height = atoi(config->value);
  }
  if (config->get_val(config, "input_fps_numerator")) {
    params->input_fps_numerator = atoi(config->value);
  }
  if (config->get_val(config, "input_fps_denominator")) {
    params->input_fps_denominator = atoi(config->value);
  }
  if (config->get_val(config, "input_device")) {
    snprintf(params->input_device, sizeof(params->input_device), "%s", config->value);
  }
  if (config->get_val(config, "video_input_connector")) {
    snprintf(params->video_input_connector, sizeof(params->video_input_connector), "%s", config->value);
  }
  if (config->get_val(config, "audio_input_connector")) {
    snprintf(params->audio_input_connector, sizeof(params->audio_input_connector), "%s", config->value);
  }
  if (config->get_val(config, "encoding_width")) {
    params->encoding_width = atoi(config->value);
  }
  if (config->get_val(config, "encoding_height")) {
    params->encoding_height = atoi(config->value);
  }
  if (config->get_val(config, "encoding_fps_numerator")) {
    params->encoding_fps_numerator = atoi(config->value);
  }
  if (config->get_val(config, "encoding_fps_denominator")) {
    params->encoding_fps_denominator = atoi(config->value);
  }
  if (config->get_val(config, "video_bitrate")) {
    params->video_bitrate = atoi(config->value);
  }
  if (config->get_val(config, "audio_quality")) {
    params->audio_quality = atof(config->value);
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

  kr_config_close(config);

  return 0;
}

