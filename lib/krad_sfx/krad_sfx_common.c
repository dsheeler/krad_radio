char *sfxtypetostr(kr_sfx_type type) {
  switch (type) {
    case KR_NOFX:
      return "Unknown";
    case KR_EQ:
      return "eq";
    case KR_LOWPASS:
      return "lowpass";
    case KR_HIGHPASS:
      return "highpass";
    case KR_ANALOG:
      return "analog";
  }
  return "Unknown";
}

char *sfxctltostr(kr_sfx_control control) {
  switch (control) {
    case DB:
      return "db";
    case HZ:
      return "hz";
    case BW:
      return "bw";
    case TYPE:
      return "type";
    case DRIVE:
      return "drive";
    case BLEND:
      return "blend";
  }
  return "Unknown";
}

