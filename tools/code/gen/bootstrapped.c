#include "bootstrapped.h"
#include <string.h>

char *codegen_enum_to_string(cgen_enum val) {
  switch (val) {
    case CGEN_KR_SPRITE_INFO:
      return "kr_sprite_info";
    case CGEN_KR_TEXT_INFO:
      return "kr_text_info";
    case CGEN_KR_VECTOR_INFO:
      return "kr_vector_info";
    case CGEN_KR_COMPOSITOR_PATH_INFO:
      return "kr_compositor_path_info";
    case CGEN_KR_COMPOSITOR_INFO:
      return "kr_compositor_info";
    case CGEN_KR_JACK_SETUP_INFO:
      return "kr_jack_setup_info";
    case CGEN_KR_JACK_INFO:
      return "kr_jack_info";
    case CGEN_KR_JACK_PATH_INFO:
      return "kr_jack_path_info";
    case CGEN_KR_MIXER_INFO:
      return "kr_mixer_info";
    case CGEN_KR_MIXER_PATH_INFO:
      return "kr_mixer_path_info";
    case CGEN_KR_EQ_BAND_INFO:
      return "kr_eq_band_info";
    case CGEN_KR_EQ_INFO:
      return "kr_eq_info";
    case CGEN_KR_LOWPASS_INFO:
      return "kr_lowpass_info";
    case CGEN_KR_HIGHPASS_INFO:
      return "kr_highpass_info";
    case CGEN_KR_ANALOG_INFO:
      return "kr_analog_info";
    case CGEN_KR_ADAPTER_INFO:
      return "kr_adapter_info";
    case CGEN_KR_ADAPTER_PATH_INFO:
      return "kr_adapter_path_info";
    case CGEN_KR_TRANSPONDER_INFO:
      return "kr_transponder_info";
    case CGEN_KR_TRANSPONDER_PATH_IO_INFO:
      return "kr_transponder_path_io_info";
    case CGEN_KR_TRANSPONDER_PATH_INFO:
      return "kr_transponder_path_info";
    case CGEN_KR_V4L2_INFO:
      return "kr_v4l2_info";
    case CGEN_KR_V4L2_OPEN_INFO:
      return "kr_v4l2_open_info";
    case CGEN_KR_WAYLAND_INFO:
      return "kr_wayland_info";
    case CGEN_KR_WAYLAND_PATH_INFO:
      return "kr_wayland_path_info";
  }
  return "Unknown";
}

cgen_enum codegen_string_to_enum(char *string) {
  if (!strcmp(string,"kr_sprite_info")) {
    return CGEN_KR_SPRITE_INFO;
  }
  if (!strcmp(string,"kr_text_info")) {
    return CGEN_KR_TEXT_INFO;
  }
  if (!strcmp(string,"kr_vector_info")) {
    return CGEN_KR_VECTOR_INFO;
  }
  if (!strcmp(string,"kr_compositor_path_info")) {
    return CGEN_KR_COMPOSITOR_PATH_INFO;
  }
  if (!strcmp(string,"kr_compositor_info")) {
    return CGEN_KR_COMPOSITOR_INFO;
  }
  if (!strcmp(string,"kr_jack_setup_info")) {
    return CGEN_KR_JACK_SETUP_INFO;
  }
  if (!strcmp(string,"kr_jack_info")) {
    return CGEN_KR_JACK_INFO;
  }
  if (!strcmp(string,"kr_jack_path_info")) {
    return CGEN_KR_JACK_PATH_INFO;
  }
  if (!strcmp(string,"kr_mixer_info")) {
    return CGEN_KR_MIXER_INFO;
  }
  if (!strcmp(string,"kr_mixer_path_info")) {
    return CGEN_KR_MIXER_PATH_INFO;
  }
  if (!strcmp(string,"kr_eq_band_info")) {
    return CGEN_KR_EQ_BAND_INFO;
  }
  if (!strcmp(string,"kr_eq_info")) {
    return CGEN_KR_EQ_INFO;
  }
  if (!strcmp(string,"kr_lowpass_info")) {
    return CGEN_KR_LOWPASS_INFO;
  }
  if (!strcmp(string,"kr_highpass_info")) {
    return CGEN_KR_HIGHPASS_INFO;
  }
  if (!strcmp(string,"kr_analog_info")) {
    return CGEN_KR_ANALOG_INFO;
  }
  if (!strcmp(string,"kr_adapter_info")) {
    return CGEN_KR_ADAPTER_INFO;
  }
  if (!strcmp(string,"kr_adapter_path_info")) {
    return CGEN_KR_ADAPTER_PATH_INFO;
  }
  if (!strcmp(string,"kr_transponder_info")) {
    return CGEN_KR_TRANSPONDER_INFO;
  }
  if (!strcmp(string,"kr_transponder_path_io_info")) {
    return CGEN_KR_TRANSPONDER_PATH_IO_INFO;
  }
  if (!strcmp(string,"kr_transponder_path_info")) {
    return CGEN_KR_TRANSPONDER_PATH_INFO;
  }
  if (!strcmp(string,"kr_v4l2_info")) {
    return CGEN_KR_V4L2_INFO;
  }
  if (!strcmp(string,"kr_v4l2_open_info")) {
    return CGEN_KR_V4L2_OPEN_INFO;
  }
  if (!strcmp(string,"kr_wayland_info")) {
    return CGEN_KR_WAYLAND_INFO;
  }
  if (!strcmp(string,"kr_wayland_path_info")) {
    return CGEN_KR_WAYLAND_PATH_INFO;
  }
  return 0;
}

