#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sched.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

void udp_redir (int inport, char *outhost, int outport) {

  int ret;
  int in_sd;
  int out_sd;
        struct sockaddr_in remote_client;
  int slen;
  unsigned char *buffer;
  struct sockaddr_in local_address;
  struct sockaddr_in remote_address;

  slen = sizeof (remote_address);

  buffer = calloc (1, 8192);

        in_sd = socket (AF_INET, SOCK_DGRAM, 0);
  out_sd = socket (AF_INET, SOCK_DGRAM, 0);

  memset((char *) &local_address, 0, sizeof(local_address));
  local_address.sin_family = AF_INET;
  local_address.sin_port = htons (inport);
  local_address.sin_addr.s_addr = htonl(INADDR_ANY);

  memset((char *) &remote_client, 0, sizeof(remote_client));
  remote_client.sin_port = htons(outport);
  remote_client.sin_family = AF_INET;

  if (inet_pton(remote_client.sin_family, outhost, &(remote_client.sin_addr)) != 1) {
          printf ("inet_pton() failed");
          exit (1);
        }

  if (bind (in_sd, (struct sockaddr *)&local_address, sizeof(local_address)) == -1 ) {
    printf ("bind error\n");
    exit (1);
    }

  printf ("hi!\n");

  while (1) {

    ret = recvfrom (in_sd, buffer, 2000, 0, (struct sockaddr *)&remote_address, (socklen_t *)&slen);

    if (ret == -1) {
      printf ("failed recvin udp\n");
      exit (1);
    }

    ret = sendto (out_sd, buffer, ret, 0,
                 (struct sockaddr *) &remote_client,
                 sizeof(remote_client));

  static int pkts = 0;
  printf("pkts %d\r", pkts++);
    if (ret == -1) {
      printf ("failed sendto udp\n");
      exit (1);
    }
  }
}


int main (int argc, char *argv[]) {


  udp_redir (atoi(argv[1]), "67.188.0.2", 3777);


  return 0;

}
