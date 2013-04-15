#ifndef KRAD_PLAYER_COMMON_H
#define KRAD_PLAYER_COMMON_H

typedef enum {
  REVERSE,
  FORWARD
} kr_direction_t;

typedef struct kr_packet_St kr_packet_t;

struct kr_packet_St {
  uint8_t *buffer;
  size_t size;
  
  uint32_t track;
};


#endif
