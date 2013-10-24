int32_t kr_sprite_info_to_json (char *output, kr_sprite_info *infostruct, uint32_t max_len);
int32_t kr_text_info_to_json (char *output, kr_text_info *infostruct, uint32_t max_len);
int32_t kr_vector_info_to_json (char *output, kr_vector_info *infostruct, uint32_t max_len);
int32_t kr_compositor_path_info_to_json (char *output, kr_compositor_path_info *infostruct, uint32_t max_len);
int32_t kr_compositor_info_to_json (char *output, kr_compositor_info *infostruct, uint32_t max_len);
int32_t kr_mixer_info_to_json (char *output, kr_mixer_info *infostruct, uint32_t max_len);
int32_t kr_mixer_path_info_to_json (char *output, kr_mixer_path_info *infostruct, uint32_t max_len);
int32_t kr_eq_band_info_to_json (char *output, kr_eq_band_info *infostruct, uint32_t max_len);
int32_t kr_eq_info_to_json (char *output, kr_eq_info *infostruct, uint32_t max_len);
int32_t kr_lowpass_info_to_json (char *output, kr_lowpass_info *infostruct, uint32_t max_len);
int32_t kr_highpass_info_to_json (char *output, kr_highpass_info *infostruct, uint32_t max_len);
int32_t kr_analog_info_to_json (char *output, kr_analog_info *infostruct, uint32_t max_len);
int32_t kr_adapter_info_to_json (char *output, kr_adapter_info *infostruct, uint32_t max_len);
int32_t kr_adapter_path_info_to_json (char *output, kr_adapter_path_info *infostruct, uint32_t max_len);
int32_t kr_transponder_info_to_json (char *output, kr_transponder_info *infostruct, uint32_t max_len);
int32_t kr_transponder_path_io_info_to_json (char *output, kr_transponder_path_io_info *infostruct, uint32_t max_len);
int32_t kr_transponder_path_info_to_json (char *output, kr_transponder_path_info *infostruct, uint32_t max_len);

int32_t kr_sprite_info_to_json (char *output, kr_sprite_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"filename\": \"%s\",\"controls\": %u",infostruct->filename,infostruct->controls); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_text_info_to_json (char *output, kr_text_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"text\": \"%s\",\"font\": \"%s\",\"red\": %0.2f,\"green\": %0.2f,\"blue\": %0.2f,\"controls\": %u",infostruct->text,infostruct->font,infostruct->red,infostruct->green,infostruct->blue,infostruct->controls); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_vector_info_to_json (char *output, kr_vector_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"type\": %u,\"red\": %0.2f,\"green\": %0.2f,\"blue\": %0.2f,\"controls\": %u",infostruct->type,infostruct->red,infostruct->green,infostruct->blue,infostruct->controls); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_compositor_path_info_to_json (char *output, kr_compositor_path_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"name\": \"%s\",\"type\": %u,\"height\": %u,\"crop_x\": %u,\"crop_y\": %u,\"crop_width\": %u,\"crop_height\": %u,\"view\": %u,\"controls\": %u",infostruct->name,infostruct->type,infostruct->height,infostruct->crop_x,infostruct->crop_y,infostruct->crop_width,infostruct->crop_height,infostruct->view,infostruct->controls); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_compositor_info_to_json (char *output, kr_compositor_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"width\": %u,\"height\": %u,\"fps_numerator\": %u,\"fps_denominator\": %u,\"sprites\": %u,\"vectors\": %u,\"texts\": %u,\"inputs\": %u,\"outputs\": %u,\"frames\": %llu,\"background_filename\": \"%s\"",infostruct->width,infostruct->height,infostruct->fps_numerator,infostruct->fps_denominator,infostruct->sprites,infostruct->vectors,infostruct->texts,infostruct->inputs,infostruct->outputs,infostruct->frames,infostruct->background_filename); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_mixer_info_to_json (char *output, kr_mixer_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"period_size\": %u,\"sample_rate\": %u,\"inputs\": %u,\"buses\": %u,\"outputs\": %u,\"frames\": %llu,\"timecode\": %llu,\"clock\": \"%s\"",infostruct->period_size,infostruct->sample_rate,infostruct->inputs,infostruct->buses,infostruct->outputs,infostruct->frames,infostruct->timecode,infostruct->clock); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_mixer_path_info_to_json (char *output, kr_mixer_path_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"name\": \"%s\",\"bus\": \"%s\",\"crossfade_group\": \"%s\",\"channels\": %u,\"type\": %u,\"fade\": %0.2f,\"volume\": %0.2f,\"map\": %d,\"mixmap\": %d,\"rms\": %0.2f,\"peak\": %0.2f,\"delay\": %d,\"lowpass\": {\"bw\": %0.2f,\"hz\": %0.2f},\"highpass\": {\"bw\": %0.2f,\"hz\": %0.2f},\"analog\": {\"drive\": %0.2f,\"blend\": %0.2f},\"eq\": {\"band\": {\"db\": %0.2f,\"bw\": %0.2f,\"hz\": %0.2f}}",infostruct->name,infostruct->bus,infostruct->crossfade_group,infostruct->channels,infostruct->type,infostruct->fade,infostruct->volume,infostruct->map,infostruct->mixmap,infostruct->rms,infostruct->peak,infostruct->delay); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_eq_band_info_to_json (char *output, kr_eq_band_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"db\": %0.2f,\"bw\": %0.2f,\"hz\": %0.2f",infostruct->db,infostruct->bw,infostruct->hz); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_eq_info_to_json (char *output, kr_eq_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"band\": {\"db\": %0.2f,\"bw\": %0.2f,\"hz\": %0.2f}",); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_lowpass_info_to_json (char *output, kr_lowpass_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"bw\": %0.2f,\"hz\": %0.2f",infostruct->bw,infostruct->hz); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_highpass_info_to_json (char *output, kr_highpass_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"bw\": %0.2f,\"hz\": %0.2f",infostruct->bw,infostruct->hz); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_analog_info_to_json (char *output, kr_analog_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"drive\": %0.2f,\"blend\": %0.2f",infostruct->drive,infostruct->blend); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_adapter_info_to_json (char *output, kr_adapter_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"api\": %u,\"wayland\": {\"v4l2\": {\"decklink\": {",infostruct->api); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_adapter_path_info_to_json (char *output, kr_adapter_path_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"api\": %u,\"name\": \"%s\",\"dir\": %u,\"wayland\": {\"v4l2\": %u,\"decklink\": {",infostruct->api,infostruct->name,infostruct->dir,infostruct->wayland.v4l2); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_transponder_info_to_json (char *output, kr_transponder_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"active_paths\": %u",infostruct->active_paths); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_transponder_path_io_info_to_json (char *output, kr_transponder_path_io_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"type\": %u,\"compositor_path_info\": {\"name\": \"%s\",\"type\": %u,\"height\": %u,\"crop_x\": %u,\"crop_y\": %u,\"crop_width\": %u,\"crop_height\": %u,\"view\": %u,\"controls\": %u},\"adapter_path_info\": {\"api\": %u,\"name\": \"%s\",\"dir\": %u,\"wayland\": {\"v4l2\": %u,\"decklink\": {}",infostruct->type); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}

int32_t kr_transponder_path_info_to_json (char *output, kr_transponder_path_info *infostruct, uint32_t max_len) {
  int32_t len; 

  len = snprintf(output,max_len,"\"name\": \"%s\",\"input\": {\"type\": %u,\"compositor_path_info\": {\"name\": \"%s\",\"type\": %u,\"height\": %u,\"crop_x\": %u,\"crop_y\": %u,\"crop_width\": %u,\"crop_height\": %u,\"view\": %u,\"controls\": %u},\"adapter_path_info\": {\"api\": %u,\"name\": \"%s\",\"dir\": %u,\"wayland\": {\"v4l2\": %u,\"decklink\": {}},\"output\": {\"type\": %u,\"compositor_path_info\": {\"name\": \"%s\",\"type\": %u,\"height\": %u,\"crop_x\": %u,\"crop_y\": %u,\"crop_width\": %u,\"crop_height\": %u,\"view\": %u,\"controls\": %u},\"adapter_path_info\": {\"api\": %u,\"name\": \"%s\",\"dir\": %u,\"wayland\": {\"v4l2\": %u,\"decklink\": {}}",infostruct->name); 

  if (len>max_len) {
    return 0;
  } else {
    return len;
  } 

}
