#include "kr_client.h"

void my_remote_print (kr_remote_t *remote) {
  printf ("oh its a remote! %d on interface %s\n",
          remote->port,
          remote->interface);
}

void my_tag_print (kr_tag_t *tag) {
  printf ("The tag I wanted: %s - %s\n",
          tag->name,
          tag->value);
}

void my_portgroup_print (kr_portgroup_t *portgroup) {
  printf ("oh its a portgroup called %s and the volume is %0.2f%%\n",
           portgroup->sysname,
           portgroup->volume[0]);
}

void my_compositor_print (kr_compositor_t *compositor) {

  printf ("Compositor Resolution: %d x %d Frame Rate: %d / %d - %f\n",
					 compositor->width, compositor->height,
					 compositor->fps_numerator, compositor->fps_denominator,
					 ((float)compositor->fps_numerator / (float)compositor->fps_denominator));
}

void my_sprite_print (kr_sprite_t *sprite) {

  printf ("Sprite stuf: %d %f\n",
					 sprite->controls.width, sprite->controls.rotation);
}

void my_text_print (kr_text_t *text) {

  printf ("text stuf: %d %f\n",
					 text->controls.width, text->controls.rotation);
}

void my_vector_print (kr_vector_t *vector) {

  printf ("vector stuf: %d %f\n",
					 vector->controls.width, vector->controls.rotation);
}

void my_videoport_print (kr_port_t *videoport) {

  printf ("videoport stuf: %d %f\n",
					 videoport->controls.width, videoport->controls.rotation);
}

void my_mixer_print (kr_mixer_t *mixer) {

  printf ("Mixer Sample Rate: %d\n",
					 mixer->sample_rate);
}

void my_print (kr_crate_t *crate) {

  if ((crate->addr->path.unit == KR_MIXER) && (crate->addr->path.subunit.zero == KR_UNIT)) {
    my_mixer_print (crate->inside.mixer);
  }
  if ((crate->addr->path.unit == KR_COMPOSITOR) && (crate->addr->path.subunit.zero == KR_UNIT)) {
    my_compositor_print (crate->inside.compositor);
  }
  if ((crate->addr->path.unit == KR_COMPOSITOR) && (crate->addr->path.subunit.zero == KR_SPRITE)) {
    my_sprite_print (crate->inside.sprite);
  }
  if ((crate->addr->path.unit == KR_COMPOSITOR) && (crate->addr->path.subunit.zero == KR_TEXT)) {
    my_text_print (crate->inside.text);
  } 
  if ((crate->addr->path.unit == KR_COMPOSITOR) && (crate->addr->path.subunit.zero == KR_VECTOR)) {
    my_vector_print (crate->inside.vector);
  } 
  if ((crate->addr->path.unit == KR_COMPOSITOR) && (crate->addr->path.subunit.zero == KR_VIDEOPORT)) {
    my_videoport_print (crate->inside.videoport);
  } 
  if ((crate->addr->path.unit == KR_MIXER) && (crate->addr->path.subunit.mixer_subunit == KR_PORTGROUP)) {
    my_portgroup_print (crate->inside.portgroup);
  }
}

void get_delivery (kr_client_t *client) {

  kr_crate_t *crate;
  char *string;
  int integer;
  float real;

  integer = 0;
  real = 0.0f;
  string = NULL;
  crate = NULL;
  printf ("\n*** Delivery Start: \n");

  kr_delivery_get (client, &crate);

  if (crate != NULL) {

    kr_address_debug_print (crate->addr); 

    /* Crate sometimes can be converted
       to a integer, float or string */
    
    if (kr_uncrate_string (crate, &string)) {
      printf ("String: \n%s\n", string);
      kr_string_recycle (&string);
    }
    
    if (kr_uncrate_int (crate, &integer)) {
      printf ("Int: %d\n", integer);
      /* or but check first always! */
      // if (kr_crate_has_int (crate)) {
      //   crate->integer;
      // }
    }
    
    if (kr_uncrate_float (crate, &real)) {
      printf ("Float: %f\n", real);
      /* or but check first always! */
      // if (kr_crate_has_float (crate)) {
      //   crate->real;
      // }
    }
    
    //crate->notice  << a type/reason/event    
    
    /* Crate has a rep struct */
    
    if (kr_crate_loaded (crate)) {
      my_print (crate);
    }

    kr_crate_recycle (&crate);
    printf ("*** Delivery End\n\n");
  }
}

void take_deliveries_long_time (kr_client_t *client) {

  int b;
  int ret;
  uint64_t max;
  unsigned int timeout_ms;
  
  ret = 0;
  b = 0;
  max = 10000;
  timeout_ms = 3000;
  

  kr_subscribe_all (client);
  
  printf ("Waiting for up to %"PRIu64" deliveries for up to %ums each\n",
          max, timeout_ms);
  
  for (b = 0; b < max; b++) {
    ret = kr_wait (client, timeout_ms);
    if (ret > 0) {
      get_delivery (client);
    } else {
      printf (".");
      fflush (stdout);
    }
  }
}

void accept_some_deliveries (kr_client_t *client) {

  int wait_ms;
  
  wait_ms = 250;

  while (kr_delivery_wait_until_final (client, wait_ms)) {
    get_delivery (client);
  }
}

void one_shot_demo (kr_client_t *client) {

  kr_tags (client, NULL);    
  kr_delivery_accept_and_report (client);

  kr_system_info (client);
  accept_some_deliveries (client);

  kr_remote_list (client);
  accept_some_deliveries (client);

  kr_compositor_info (client);
  accept_some_deliveries (client);
  
  kr_mixer_info (client);
  accept_some_deliveries (client);

  kr_mixer_portgroups (client);
  accept_some_deliveries (client);
  
  kr_compositor_subunit_list (client);
  accept_some_deliveries (client);
}

int main (int argc, char *argv[]) {

  kr_client_t *client;
  char *sysname;

  sysname = NULL;
  client = NULL;

  if (argc < 2) {
    fprintf (stderr, "Specify a station sysname!\n");
    return 1;
  }

  if (krad_valid_host_and_port (argv[1])) {
    sysname = argv[1];
  } else {
    if (!krad_valid_sysname(argv[1])) {
      fprintf (stderr, "Invalid station sysname!\n");
      return 1;
    } else {
      sysname = argv[1];
    }
  }

  client = kr_client_create ("krad api test");

  if (client == NULL) {
    fprintf (stderr, "Could create client\n");
    return 1;
  }

  if (!kr_connect (client, sysname)) {
    fprintf (stderr, "Could not connect to %s krad radio daemon\n", sysname);
    kr_client_destroy (&client);
    return 1;
  }

  printf ("Connected to %s!\n", sysname);

  printf ("Running the one shot demo\n");
  one_shot_demo (client);
  
  printf ("Now getting into the business\n");
  take_deliveries_long_time (client);

  printf ("Disconnecting from %s..\n", sysname);
  kr_disconnect (client);
  printf ("Disconnected from %s.\n", sysname);
  printf ("Destroying client..\n");
  kr_client_destroy (&client);
  printf ("Client Destroyed.\n");
  
  return 0;
}
