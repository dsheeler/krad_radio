#include "krad_sfx_common.h"

char *kr_strsfxeft(kr_sfx_effect_type type) {
  switch (type) {
    case KR_SFX_NONE:
      return "Unknown";
    case KR_SFX_EQ:
      return "eq";
    case KR_SFX_LOWPASS:
      return "lowpass";
    case KR_SFX_HIGHPASS:
      return "highpass";
    case KR_SFX_ANALOG:
      return "analog";
  }
  return "Unknown";
}

char *kr_strsfxeftctl(kr_sfx_effect_control control) {
  switch (control) {
    case KR_SFX_DB:
      return "db";
    case KR_SFX_HZ:
      return "hz";
    case KR_SFX_BW:
      return "bw";
    case KR_SFX_PASSTYPE:
      return "type";
    case KR_SFX_DRIVE:
      return "drive";
    case KR_SFX_BLEND:
      return "blend";
  }
  return "Unknown";
}

kr_sfx_effect_type kr_sfxeftstr(char *string) {
  if (((strlen(string) == 2) && (strncmp(string, "lp", 2) == 0)) ||
      ((strlen(string) == 7) && (strncmp(string, "lowpass", 7) == 0))) {
    return KR_SFX_LOWPASS;
  }
  if (((strlen(string) == 2) && (strncmp(string, "hp", 2) == 0)) ||
      ((strlen(string) == 8) && (strncmp(string, "highpass", 8) == 0))) {
    return KR_SFX_HIGHPASS;
  }
  if ((strlen(string) == 2) && (strncmp(string, "eq", 2) == 0)) {
    return KR_SFX_EQ;
  }
  if ((strlen(string) == 6) && (strncmp(string, "analog", 6) == 0)) {
    return KR_SFX_ANALOG;
  }
  return KR_SFX_NONE;
}
