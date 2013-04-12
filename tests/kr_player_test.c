#include "kr_player.h"

int main (int argc, char **argv) {

  kr_player_t *player;

  player = kr_player_create ();
  if (player == NULL) {
    fprintf (stderr, "Could not create player :/\n");
    return 1;
  }

  kr_player_play (player);
  kr_player_pause (player);
  kr_player_seek (player, 666);
  kr_player_direction_set (player, FORWARD);  
  kr_player_direction_set (player, REVERSE);  
  kr_player_speed_set (player, 1.2);
  kr_player_speed_set (player, 122.4545);
  kr_player_stop (player);    
  kr_player_play (player);
  kr_player_pause (player);  

  kr_player_destroy (&player);

  printf ("kr_player test completed.\n");

  return 0;
}
