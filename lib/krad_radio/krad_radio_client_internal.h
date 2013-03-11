//#ifndef KRAD_CLIENT_INTERNAL_H
//#define KRAD_CLIENT_INTERNAL_H

#include "krad_ipc_client.h"
#include "krad_radio_client.h"

struct kr_client_St {
  krad_ipc_client_t *krad_ipc_client;


  kr_ebml2_t *ebml2;
  kr_io2_t *io;
  
  int autosync;

  char *name;

  int subscriber;
  int last_delivery_was_final;
  
  
  
  
  kr_ebml2_t *ebml_in;
  kr_io2_t *io_in;
  
  
  
};

struct kr_shm_St {
  int fd;
  char *buffer;
  uint64_t size;
};

typedef int (*rep_to_string_t)( unsigned char *, uint64_t, char ** );


int kr_send_fd (kr_client_t *client, int fd);


//#endif
