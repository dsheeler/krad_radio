#include "krad_jack_helpers.h"

int kr_jack_direction_to_index(int val) {
  switch (val) {
    case KR_JACK_INPUT:
      return 0;
    case KR_JACK_OUTPUT:
      return 1;
  }
  return -1;
}

char *kr_strfr_kr_jack_direction(int val) {
  switch (val) {
    case KR_JACK_INPUT:
      return "kr_jack_input";
    case KR_JACK_OUTPUT:
      return "kr_jack_output";
  }
  return NULL;
}

int kr_strto_kr_jack_direction(char *string) {
  if (!strcmp(string,"kr_jack_input")) {
    return KR_JACK_INPUT;
  }
  if (!strcmp(string,"kr_jack_output")) {
    return KR_JACK_OUTPUT;
  }

  return -1;
}

int kr_jack_state_to_index(int val) {
  switch (val) {
    case KR_JACK_OFFLINE:
      return 0;
    case KR_JACK_ONLINE:
      return 1;
  }
  return -1;
}

char *kr_strfr_kr_jack_state(int val) {
  switch (val) {
    case KR_JACK_OFFLINE:
      return "kr_jack_offline";
    case KR_JACK_ONLINE:
      return "kr_jack_online";
  }
  return NULL;
}

int kr_strto_kr_jack_state(char *string) {
  if (!strcmp(string,"kr_jack_offline")) {
    return KR_JACK_OFFLINE;
  }
  if (!strcmp(string,"kr_jack_online")) {
    return KR_JACK_ONLINE;
  }

  return -1;
}

int kr_jack_setup_info_init(kr_jack_setup_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_jack_setup_info));

  return 0;
}

int kr_jack_setup_info_valid(kr_jack_setup_info *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_jack_setup_info_random(kr_jack_setup_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_jack_setup_info));
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_jack_info_init(kr_jack_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_jack_info));

  return 0;
}

int kr_jack_info_valid(kr_jack_info *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_jack_info_random(kr_jack_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_jack_info));
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_jack_path_info_init(kr_jack_path_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_jack_path_info));

  return 0;
}

int kr_jack_path_info_valid(kr_jack_path_info *st) {
  if (st == NULL) {
    return -1;
  }


  return 0;
}

int kr_jack_path_info_random(kr_jack_path_info *st) {
  if (st == NULL) {
    return -1;
  }

  memset(st, 0, sizeof(kr_jack_path_info));
  if (st == NULL) {
    return -1;
  }


  return 0;
}

