#include "krad_jack.h"

void kr_jack_portgroup_samples_callback(int frames, void *userdata,
 float **samples) {

  kr_jack_portgroup *portgroup = (kr_jack_portgroup *)userdata;
  int c;
  float *temp;

  for (c = 0; c < portgroup->channels; c++) {
    if (portgroup->direction == KR_AIN) {
      temp = jack_port_get_buffer (portgroup->ports[c], frames);
      memcpy (portgroup->samples[c], temp, frames * 4);
      samples[c] = portgroup->samples[c];
    } else {
      samples[c] = jack_port_get_buffer (portgroup->ports[c], frames);
    }
  }
}
/*
void kr_jack_check_connection (kr_jack *krad_jack, char *remote_port) {

  int p;
  int flags;
  jack_port_t *port;

  if (strlen(remote_port)) {
    for (p = 0; p < 256; p++) {
      if ((krad_jack->stay_connected[p] != NULL) && (krad_jack->stay_connected_to[p] != NULL)) {
        if ((strncmp(krad_jack->stay_connected_to[p], remote_port, strlen(remote_port))) == 0) {

          port = jack_port_by_name (krad_jack->client, remote_port);
          flags = jack_port_flags (port);

          if (flags == JackPortIsOutput) {
            //printk ("Krad Jack: Replugging %s to %s", remote_port, krad_jack->stay_connected[p]);
            jack_connect (krad_jack->client, remote_port, krad_jack->stay_connected[p]);
          } else {
            //printk ("Krad Jack: Replugging %s to %s", krad_jack->stay_connected[p], remote_port);
            jack_connect (krad_jack->client, krad_jack->stay_connected[p], remote_port);
          }
        }
      }
    }
  }
}

void kr_jack_stay_connection (kr_jack *krad_jack, char *port, char *remote_port) {

  int p;

  if (strlen(port) && strlen(remote_port)) {
    for (p = 0; p < 256; p++) {
      if (krad_jack->stay_connected[p] == NULL) {
        krad_jack->stay_connected[p] = strdup(port);
        krad_jack->stay_connected_to[p] = strdup(remote_port);
        break;
      }
    }
  }
}

void kr_jack_unstay_connection (kr_jack *krad_jack, char *port, char *remote_port) {

  int p;

  for (p = 0; p < 256; p++) {
    if (krad_jack->stay_connected[p] != NULL) {
      if ((krad_jack->stay_connected[p] != NULL) && (krad_jack->stay_connected_to[p] != NULL)) {
        if (((strncmp(krad_jack->stay_connected[p], port, strlen(port))) == 0) &&
           ((strlen(remote_port) == 0) || (strncmp(krad_jack->stay_connected_to[p], remote_port, strlen(remote_port)) == 0))) {
          free (krad_jack->stay_connected[p]);
          free (krad_jack->stay_connected_to[p]);
          break;
        }
      }
    }
  }
}

void kr_jack_port_registration_callback (jack_port_id_t portid, int regged, void *arg) {

  kr_jack *krad_jack = (kr_jack *)arg;

  jack_port_t *port;

  port = jack_port_by_id (krad_jack->client, portid);

  if (regged == 1) {
    //printk ("Krad Jack: %s registered", jack_port_name (port));
    if (jack_port_is_mine(krad_jack->client, port) == 0) {
      kr_jack_check_connection (krad_jack, (char *)jack_port_name(port));
    }
  } else {
    //printk ("Krad Jack: %s unregistered", jack_port_name (port));
  }
}

void kr_jack_port_connection_callback (jack_port_id_t a, jack_port_id_t b, int connect, void *arg) {

  kr_jack *krad_jack = (kr_jack *)arg;

  jack_port_t *ports[2];

  ports[0] = jack_port_by_id (krad_jack->client, a);
  ports[1] = jack_port_by_id (krad_jack->client, b);

  if (connect == 1) {
    //printk ("Krad Jack: %s connected to %s ", jack_port_name (ports[0]), jack_port_name (ports[1]));
  } else {
    //printk ("Krad Jack: %s disconnected from %s ", jack_port_name (ports[0]), jack_port_name (ports[1]));
  }

}
*/
void kr_jack_portgroup_plug(kr_jack_portgroup *portgroup, char *remote_name) {

  const char **ports;
  int flags;
  int c;

  flags = 0;

  if (strlen(remote_name) == 0) {
    return;
  }

  ports = jack_get_ports(portgroup->kr_jack->client, remote_name, JACK_DEFAULT_AUDIO_TYPE, flags);

  if (ports) {
    for (c = 0; c < portgroup->channels; c++) {
      if (ports[c]) {
        //kr_jack_stay_connection (portgroup->kr_jack, (char *)jack_port_name(portgroup->ports[c]), (char *)ports[c]);
        if (portgroup->direction == KR_AIN) {
          //printk ("Krad Jack: Plugging %s to %s", ports[c], jack_port_name(portgroup->ports[c]));
          jack_connect (portgroup->kr_jack->client, ports[c], jack_port_name(portgroup->ports[c]));
        } else {
          //printk ("Krad Jack: Plugging %s to %s", jack_port_name(portgroup->ports[c]), ports[c]);
          jack_connect (portgroup->kr_jack->client, jack_port_name(portgroup->ports[c]), ports[c]);
        }
      } else {
        return;
      }
    }
  }
}

void kr_jack_portgroup_unplug(kr_jack_portgroup *portgroup, char *remote_name) {

  int c;

  for (c = 0; c < portgroup->channels; c++) {
    //kr_jack_unstay_connection (portgroup->kr_jack, (char *)jack_port_name(portgroup->ports[c]), remote_name);
    jack_port_disconnect (portgroup->kr_jack->client, portgroup->ports[c]);
  }
}

void kr_jack_portgroup_destroy(kr_jack_portgroup *portgroup) {

  int c;

  for (c = 0; c < portgroup->channels; c++) {

    jack_port_unregister(portgroup->kr_jack->client, portgroup->ports[c]);

    portgroup->ports[c] = NULL;

    if (portgroup->direction == KR_AIN) {
      free (portgroup->samples[c]);
    }
  }

  free(portgroup);
}


kr_jack_portgroup *kr_jack_portgroup_create (kr_jack *krad_jack, char *name,
 int direction, int channels) {

  int c;
  int port_direction;
  char portname[256];
  kr_jack_portgroup *portgroup;

  portgroup = calloc (1, sizeof(kr_jack_portgroup));

  portgroup->kr_jack = krad_jack;
  portgroup->channels = channels;
  portgroup->direction = direction;
  strcpy ( portgroup->name, name );

  if (portgroup->direction == KR_AIN) {
    port_direction = JackPortIsInput;
  } else {
    port_direction = JackPortIsOutput;
  }

  for (c = 0; c < portgroup->channels; c++) {

    strcpy ( portname, name );

    if (portgroup->channels > 1) {
      strcat(portname, "_");
      strcat(portname, kr_mixer_channeltostr(c));
    }

    portgroup->ports[c] = jack_port_register (portgroup->kr_jack->client,
                          portname,
                          JACK_DEFAULT_AUDIO_TYPE,
                          port_direction,
                          0);

    if (portgroup->ports[c] == NULL) {
      printke("Krad Jack: Could not reg port, prolly a dupe reg: %s", portname);
      free(portgroup);
      return NULL;
    }

    if (portgroup->direction == KR_AIN) {
      portgroup->samples[c] = calloc (1, 16384);
    }
  }

  return portgroup;
}

int kr_jack_xrun (void *arg) {

  kr_jack *krad_jack = (kr_jack *)arg;

  krad_jack->xruns++;
  printke ("Krad Jack: %s xrun number %d!", krad_jack->name, krad_jack->xruns);

  return 0;
}

int kr_jack_process (jack_nframes_t nframes, void *arg) {

  kr_jack *krad_jack = (kr_jack *)arg;

  if (krad_jack->set_thread_name_process == 0) {
    krad_system_set_thread_name ("kr_jack_mix");
    krad_jack->set_thread_name_process = 1;
  }

  //FIXME callback go here

  return 0;
}

void kr_jack_shutdown (void *arg) {

  //kr_jack *krad_jack = (kr_jack *)arg;

  printke("Krad Jack: shutdown callback, oh dear!");
}

void kr_jack_destroy (kr_jack *krad_jack) {

  int p;
  jack_client_close(krad_jack->client);

  for (p = 0; p < 256; p++) {
    if (krad_jack->stay_connected[p] != NULL) {
      free(krad_jack->stay_connected[p]);
    }
    if (krad_jack->stay_connected_to[p] != NULL) {
      free(krad_jack->stay_connected_to[p]);
    }
  }

  free(krad_jack);
}

int kr_jack_detect () {
  return kr_jack_detect_for_jack_server_name (NULL);
}

int kr_jack_detect_for_jack_server_name (char *server_name) {

  jack_client_t *client;
  jack_options_t options;
  jack_status_t status;
  char name[128];

  sprintf(name, "kr_jack_detect_%d", rand());

  if (server_name != NULL) {
    options = JackNoStartServer | JackServerName;
  } else {
    options = JackNoStartServer;
  }

  client = jack_client_open(name, options, &status, server_name);

  if (client == NULL) {
    return 0;
  } else {
    jack_client_close (client);
    return 1;
  }
}

kr_jack *kr_jack_create() {
  return kr_jack_create_for_jack_server_name(NULL);
}

kr_jack *kr_jack_create_for_jack_server_name(char *server_name) {

  kr_jack *krad_jack;

  if ((krad_jack = calloc (1, sizeof (kr_jack))) == NULL) {
    failfast ("Krad Jack memory alloc failure\n");
  }

  krad_system_set_thread_name ("kr_jack");

  krad_jack->name = "kradradio"; // FIXME get this or set it somehows?;

  if (server_name != NULL) {
    krad_jack->options = JackNoStartServer | JackServerName;
    krad_jack->server_name[sizeof(krad_jack->server_name) - 1] = '\0';
    snprintf(krad_jack->server_name, sizeof(krad_jack->server_name) - 2, "%s",
     server_name);
  } else {
    krad_jack->options = JackNoStartServer;
    krad_jack->server_name[0] = '\0';
  }

  krad_jack->client = jack_client_open(krad_jack->name, krad_jack->options,
   &krad_jack->status, krad_jack->server_name);
  if (krad_jack->client == NULL) {
    failfast ("Krad Jack: jack_client_open() failed, status = 0x%2.0x",
     krad_jack->status);
    if (krad_jack->status & JackServerFailed) {
      failfast ("Krad Jack: Unable to connect to JACK server");
    }
  }

  if (krad_jack->status & JackNameNotUnique) {
    krad_jack->name = jack_get_client_name(krad_jack->client);
    printke ("Krad Jack: unique name `%s' assigned", krad_jack->name);
  }

  krad_jack->sample_rate = jack_get_sample_rate(krad_jack->client);
  krad_jack->period_size = jack_get_buffer_size(krad_jack->client);

  // Set up Callbacks

  jack_set_process_callback (krad_jack->client, kr_jack_process, krad_jack);
  jack_on_shutdown (krad_jack->client, kr_jack_shutdown, krad_jack);
  jack_set_xrun_callback (krad_jack->client, kr_jack_xrun, krad_jack);
  //jack_set_port_registration_callback(krad_jack->client, kr_jack_port_registration_callback, krad_jack );
  //jack_set_port_connect_callback(krad_jack->client, kr_jack_port_connection_callback, krad_jack );

  // Activate

  if (jack_activate (krad_jack->client)) {
    failfast ("Krad Jack: cannot activate client");
  }

  krad_jack->active = 1;

  return krad_jack;
}

