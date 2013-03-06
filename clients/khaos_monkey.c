#include "kr_client.h"

#define MAX_MONKEYS_ON_THE_LAMB 5
#define MAX_MONKEY_LIFE 666
#define INCIDENTS 100

typedef struct khaos_monkey_St khaos_monkey_t;

struct khaos_monkey_St {
  char station_sysname[128];
  kr_client_t *client;
  int lifetime;
  uint32_t number;
	pthread_t on_the_lamb_thread;
};

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
  //printf ("*** Delivery Start: \n");

  kr_delivery_get (client, &crate);

  if (crate != NULL) {

    //kr_address_debug_print (crate->addr); 

    /* Crate sometimes can be converted
       to a integer, float or string */
    
    if (kr_uncrate_string (crate, &string)) {
      //printf ("String: \n%s\n", string);
      kr_string_recycle (&string);
    }
    
    if (kr_uncrate_int (crate, &integer)) {
      //printf ("Int: %d\n", integer);
      /* or but check first always! */
      // if (kr_crate_has_int (crate)) {
      //   crate->integer;
      // }
    }
    
    if (kr_uncrate_float (crate, &real)) {
      //printf ("Float: %f\n", real);
      /* or but check first always! */
      // if (kr_crate_has_float (crate)) {
      //   crate->real;
      // }
    }
    
    //crate->notice  << a type/reason/event    
    
    /* Crate has a rep struct */
    
    if (kr_crate_loaded (crate)) {
      //my_print (crate);
    }

    kr_crate_recycle (&crate);
    //printf ("*** Delivery End\n\n\n");
  }
}

void accept_some_deliveries (kr_client_t *client) {

  int wait_ms;
  
  wait_ms = 250;

  while (kr_delivery_wait_until_final (client, wait_ms)) {
    get_delivery (client);
  }
}

void rage (kr_client_t *client) {

  //kr_tags (client, NULL);    
  //kr_delivery_accept_and_report (client);

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
}

void summon_monkey (khaos_monkey_t *monkey) {

  char monkey_name[256];
  int len;
  int i;
  
  len = rand() % 255;
  
  monkey->lifetime = rand() % MAX_MONKEY_LIFE;
  
  for (i = 0; i < len; i++) {
    monkey_name[i] = rand();
  }
  monkey_name[i] = '\0';

  monkey->client = kr_client_create (monkey_name);

  if (monkey->client == NULL) {
    fprintf (stderr, "Could create client\n");
    pthread_exit (NULL);
  }

  if (!kr_connect (monkey->client, monkey->station_sysname)) {
    fprintf (stderr, "Could not connect to %s krad radio daemon\n", monkey->station_sysname);
    kr_client_destroy (&monkey->client);
    pthread_exit (NULL);
  }

  if (rand() % 2) {
    printf ("Monkey %u: I will break your radio %d times!\n",
            monkey->number, monkey->lifetime);
  } else {
    printf ("Monkey %u: I AM ON THE LAMB! I will live for %d rages!\n",
            monkey->number, monkey->lifetime);
  }
}

void *on_the_lamb (void *arg) {

  khaos_monkey_t *monkey = (khaos_monkey_t *)arg;

  summon_monkey (monkey);

  int b;
  int ret;
  unsigned int timeout_ms;
  
  ret = 0;
  b = 0;
  timeout_ms = 3000;
  
  kr_subscribe_all (monkey->client);

  //printf ("Running the one shot demo\n");
  rage (monkey->client);


  kr_subscribe_all (monkey->client);
  
  //printf ("Waiting for up to %"PRIu64" deliveries for up to %ums each\n",
  //        max, timeout_ms);
  
  for (b = 0; b < monkey->lifetime; b++) {
    ret = kr_wait (monkey->client, timeout_ms);
    if (ret > 0) {
      get_delivery (monkey->client);
    } else {
      //printf (".");
      //fflush (stdout);
    }
    
    if ((b % 1423) > 1222) {
      rage (monkey->client);
    }
  }
  
  kr_disconnect (monkey->client);
  kr_client_destroy (&monkey->client);

  return NULL;
}

khaos_monkey_t *monkey_on_the_lamb (char *sysname) {

  static uint32_t monkey_number = 0;
  khaos_monkey_t *monkey;
  monkey = calloc (1, sizeof(khaos_monkey_t));
  monkey->number = monkey_number++;
  strncpy ( monkey->station_sysname, sysname, sizeof (monkey->station_sysname));
	pthread_create (&monkey->on_the_lamb_thread, NULL, on_the_lamb, (void *)monkey);
	
	return monkey;
}

void catch_monkey (khaos_monkey_t *monkey) {
  pthread_join (monkey->on_the_lamb_thread, NULL);
  free (monkey);
}

void uncage_monkeys (char *sysname) {

  int m;
  int lamb;
  khaos_monkey_t *monkeys[MAX_MONKEYS_ON_THE_LAMB];
  
  srand (time(NULL));  
  lamb = 2 + rand() % (MAX_MONKEYS_ON_THE_LAMB - 2);

  printf ("Uncaging %d monkeys\n", lamb);

  for (m = 0; m < lamb; m++) {
    monkeys[m] = monkey_on_the_lamb (sysname);
  }
  
  for (m = 0; m < lamb; m++) {
    catch_monkey (monkeys[m]);
    printf ("Caught monkey %d!\n", m);
  }
}

#include "krad_ebml2w.h"

void kr_print_ebml (unsigned char *buffer, int len) {

  int i;
  
  i = 0;

  printf ("Raw EBML: \n");
  for (i = 0; i < len; i++) {
    printf ("%02X", buffer[i]);
  }
  printf ("\nEnd Raw EBML\n");
}

int test_kr_ebml2 (char *filename) {

  kr_ebml2_t *ebml;
  //unsigned char *my_element;
  int fd;
  int ret;

  fd = 0;

  if (!filename) {
    fprintf (stderr, "Specify a file!\n");
    return 1;
  }

  fd = open ( filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
  if (fd < 0) {
    fprintf (stderr, "Cant open %s!\n", filename);  
    return 1;
  }
  
  ebml = kr_ebml2_create ();
  
  kr_ebml2_pack_header (ebml, "testy", 2, 2);
  /*
  kr_ebml2_pack_element (ebml, EBML_ID_CLUSTER);
  kr_ebml2_pack_element (ebml, EBML_ID_TRACK_UID);
  kr_ebml2_pack_element (ebml, EBML_ID_TRACK);
  
  kr_ebml2_start_element (ebml, EBML_ID_TAGS, &my_element);
  
  kr_ebml2_pack_element (ebml, EBML_ID_CODECDATA);
  kr_ebml2_pack_element (ebml, EBML_ID_CLUSTER_TIMECODE);
  kr_ebml2_pack_element (ebml, EBML_ID_SEGMENT_INFO);
  kr_ebml2_pack_element (ebml, EBML_ID_MUXINGAPP);


  kr_ebml2_pack_int32 (ebml, EBML_ID_TAG, 666);
  kr_ebml2_pack_int64 (ebml, EBML_ID_TAG_SIMPLE, 44444444444444);
  kr_ebml2_pack_string (ebml, EBML_ID_TAG_STRING, "When I was a young girl!");

  kr_ebml2_finish_element (ebml, my_element);
  */
  ret = write (fd, ebml->buffer, ebml->pos);
  close (fd);

  kr_print_ebml (ebml->buffer, ebml->pos);
  kr_ebml2_destroy (&ebml);

  if (ret < 0) {
    fprintf (stderr, "Write failed!\n");
    return 1;
  }
  
  return 0;
}

int main (int argc, char *argv[]) {
/*
  int incidents;
  char *sysname;

  incidents = INCIDENTS;
  sysname = NULL;

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

  while (incidents--) {
    printf ("There has been an incident... #[%d] of %d\n",
            INCIDENTS - incidents, INCIDENTS);

    uncage_monkeys (sysname);
  }
*/
/*
  int fd;
  int ret;
  int err;
  int clusters;
  kr_ebml2_t *ebml;

  uint32_t element;
  uint64_t data_size;

  clusters = 0;
  element = 0;
  data_size = 0;

  fd = 0;

  if (argc < 2) {
    fprintf (stderr, "Specify a file!\n");
    return 1;
  }

  fd = open ( argv[1], O_RDONLY );
  if (fd < 0) {
    return 1;
  }
  
  ebml = kr_ebml2_create ();
  ret = read (fd, ebml->buf, KRAD_EBML2_BUF_SZ);
  if (ret < 0) {
    close (fd);
    kr_ebml2_destroy (&ebml);
    return 1;
  }

  printf ("read %d bytes\n", ret);

  err = kr_ebml2_unpack_id (ebml, &element, &data_size);
  if (!err) {
    while (ebml->pos < ret) {
    
      err = kr_ebml2_unpack_id (ebml, &element, &data_size);
      if (err == -1) {
        printf ("parsing error!\n");
        break;
      }
      if (element != EBML_ID_SEGMENT) {
        kr_ebml2_advance (ebml, data_size);
      }
      if (element == EBML_ID_CLUSTER) {
        clusters++;
      }
    }
  }
  
  if (clusters > 0) {
    printf ("Found %d clusters!\n", clusters);
  }
  
  kr_ebml2_destroy (&ebml);
*/

  return test_kr_ebml2 (argv[1]);


}
