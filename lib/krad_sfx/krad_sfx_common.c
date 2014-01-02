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

int kr_sfxeftctlstr(kr_sfx_effect_type type, char *string) {
  if (type == KR_SFX_EQ) {
    if ((strlen(string) == 2) && (strncmp(string, "db", 2) == 0)) {
      return KR_SFX_DB;
    }
    if ((strlen(string) == 2) && (strncmp(string, "hz", 2) == 0)) {
      return KR_SFX_HZ;
    }
    if ((strlen(string) == 2) && (strncmp(string, "bw", 2) == 0)) {
      return KR_SFX_BW;
    }
    if ((strlen(string) == 9) && (strncmp(string, "bandwidth", 9) == 0)) {
      return KR_SFX_BW;
    }
  }
  if (((type == KR_SFX_LOWPASS) || (type == KR_SFX_HIGHPASS))) {
    if ((strlen(string) == 2) && (strncmp(string, "hz", 2) == 0)) {
      return KR_SFX_HZ;
    }
    if ((strlen(string) == 2) && (strncmp(string, "bw", 2) == 0)) {
      return KR_SFX_BW;
    }
    if ((strlen(string) == 9) && (strncmp(string, "bandwidth", 9) == 0)) {
      return KR_SFX_BW;
    }
  }
  if (type == KR_SFX_ANALOG) {
    if ((strlen(string) == 5) && (strncmp(string, "blend", 5) == 0)) {
      return KR_SFX_BLEND;
    }
    if ((strlen(string) == 5) && (strncmp(string, "drive", 5) == 0)) {
      return KR_SFX_DRIVE;
    }
  }
  return 0;
}
