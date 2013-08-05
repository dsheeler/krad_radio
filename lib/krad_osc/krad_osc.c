#include "krad_osc.h"

static inline void rcopy4 (void *dst, void *src);
static int address_handler (char *in, char *out);
static void krad_osc_parse_message (krad_osc_t *krad_osc, unsigned char *message, int size);
static void *krad_osc_listening_thread (void *arg);

static inline void rcopy4 (void *dst, void *src) {

	unsigned char *a_dst;
	unsigned char *a_src;

	a_dst = dst;
	a_src = src;

  a_dst[0] = a_src[3];
  a_dst[1] = a_src[2];
  a_dst[2] = a_src[1];
  a_dst[3] = a_src[0];
}

static int address_handler (char *in, char *out) {

  char *binds[64];
  char *replaces[64];
  int b;
  int r;

  r = 0;
  b = 0;

  binds[b] = "/1/fader1";
  replaces[b++] = "m/Music/c";

  binds[b] = "/1/fader2";
  replaces[b++] = "m/Music";

  binds[b] = "/1/fader3";
  replaces[b++] = "m/Music2";

  binds[b] = "/4/multifader1/1";
  replaces[b++] = "m/Music/eq/0/db";
  binds[b] = "/4/multifader1/2";
  replaces[b++] = "m/Music/eq/1/db";
  binds[b] = "/4/multifader1/3";
  replaces[b++] = "m/Music/eq/2/db";
  binds[b] = "/4/multifader1/4";
  replaces[b++] = "m/Music/eq/3/db";
  binds[b] = "/4/multifader1/5";
  replaces[b++] = "m/Music/eq/4/db";
  binds[b] = "/4/multifader1/6";
  replaces[b++] = "m/Music/eq/5/db";
  binds[b] = "/4/multifader1/7";
  replaces[b++] = "m/Music/eq/6/db";
  binds[b] = "/4/multifader1/8";
  replaces[b++] = "m/Music/eq/7/db";
  binds[b] = "/4/multifader1/9";
  replaces[b++] = "m/Music/eq/8/db";
  binds[b] = "/4/multifader1/10";
  replaces[b++] = "m/Music/eq/9/db";

  binds[b] = "/4/multifader2/1";
  replaces[b++] = "m/Music2/eq/0/db";
  binds[b] = "/4/multifader2/2";
  replaces[b++] = "m/Music2/eq/1/db";
  binds[b] = "/4/multifader2/3";
  replaces[b++] = "m/Music2/eq/2/db";
  binds[b] = "/4/multifader2/4";
  replaces[b++] = "m/Music2/eq/3/db";
  binds[b] = "/4/multifader2/5";
  replaces[b++] = "m/Music2/eq/4/db";
  binds[b] = "/4/multifader2/6";
  replaces[b++] = "m/Music2/eq/5/db";
  binds[b] = "/4/multifader2/7";
  replaces[b++] = "m/Music2/eq/6/db";
  binds[b] = "/4/multifader2/8";
  replaces[b++] = "m/Music2/eq/7/db";
  binds[b] = "/4/multifader2/9";
  replaces[b++] = "m/Music2/eq/8/db";
  binds[b] = "/4/multifader2/10";
  replaces[b++] = "m/Music2/eq/9/db";

  for (r = 0; r < b; r++) {
    if (strcmp (in, binds[r]) == 0) {
      strcpy (out, replaces[r]);
      return 1;
    }
  }

  strcpy (out, in);

  return 0;
}

static void krad_osc_parse_message (krad_osc_t *krad_osc, unsigned char *message, int size) {

  int d;
  int datalen;
  int pos;
	int len;
	char *typetag;

  int ints[4];
  float floats[4];

	char debugmsg[128];
  int dpos;
  kr_unit_control_t uc;
  char address[64];

	if (message[0] == '#') {
		printk ("Krad OSC: message is a %d byte bundle", size);
	} else {
		//printk ("Krad OSC message address pattern: %s\n", message);
		//address = (char *)message;
		len = strlen ((char *)message) + 1;
		while ((len % 4) != 0) {
			len++;
		}
    len += 1;
		typetag = (char *)message + len;
		//printk ("Krad OSC message typetag: %s\n", typetag);
    len += 1;
    datalen = 0;

    if ((strlen(typetag) == 0) || (strlen(typetag) > 4)) {
      return;
    }

    for (pos = 0; typetag[pos] != '\0'; pos++) {
      if ((typetag[pos] != 'f') && (typetag[pos] != 'i')) {
        break;
      } else {
        datalen++;
      }
		}

    len += pos;
		while ((len % 4) != 0) {
			len++;
		}

    //printk ("len %d datalen %d size %d", len, datalen, size);

    debugmsg[0] = '\0';
    dpos = 0;

    for (d = 0; d < datalen; d++) {
      pos = len + d * 4;
      //printk ("pos %d", pos);
      if (typetag[d] == 'f') {
        //osc_dcopy (&floats[d], message + pos);
        rcopy4 (&floats[d], message + pos);
        dpos += sprintf(debugmsg + dpos, " %f", floats[d]);
      }
      if (typetag[d] == 'i') {
        memcpy (&ints[d], message + pos, 4);
        dpos += sprintf(debugmsg + dpos, "%d", ints[d]);
      }
    }

    address_handler ((char *)message, address);

    printk ("Krad OSC: %s%s", address, debugmsg);

    memset (&uc, 0, sizeof (uc));
    if (kr_string_to_address (address, &uc.address)) {
      kr_unit_control_data_type_from_address (&uc.address, &uc.data_type);
      if (uc.data_type == KR_FLOAT) {
        uc.value.real = floats[0] * 100.0f;
        if ((uc.address.path.unit == KR_MIXER) &&
            (uc.address.path.subunit.mixer_subunit == KR_PORTGROUP) &&
            (uc.address.control.portgroup_control == KR_CROSSFADE)) {
          uc.value.real = ((floats[0] * 200.0f) - 100.0f);
        } else {
          if ((uc.address.path.unit == KR_MIXER) &&
              (uc.address.path.subunit.mixer_subunit == KR_EFFECT) &&
              (uc.address.control.effect_control == KR_SFX_DB)) {
            uc.value.real = ((floats[0] * 18.0f) - 9.0f);
          }
        }
        kr_unit_control_set (krad_osc->client, &uc);
      }
    }
	}
}

static void *krad_osc_listening_thread (void *arg) {

	krad_osc_t *krad_osc = (krad_osc_t *)arg;

	int ret;
	int addr_size;
	struct sockaddr_in remote_address;
	struct pollfd sockets[1];
	unsigned char buffer[OSC_BUF_SIZE];

	krad_system_set_thread_name ("kr_osc");

	printk ("Krad OSC: Listening thread starting");
  printk ("Krad OSC: Using UDP port: %d", krad_osc->port);

	addr_size = 0;
	ret = 0;
	memset (&remote_address, 0, sizeof(remote_address));

	addr_size = sizeof (remote_address);

	while (krad_osc->stop_listening == 0) {

    if (!(kr_connected (krad_osc->client))) {
      if (!(kr_connect (krad_osc->client, krad_osc->sysname))) {
        printke ("Could not connect to %s krad radio daemon", krad_osc->sysname);
        usleep (250000);
        continue;
      }
    }

		sockets[0].fd = krad_osc->sd;
		sockets[0].events = POLLIN;

		ret = poll (sockets, 1, 250);

		if (ret < 0) {
			printke ("Krad OSC: Failed on poll");
			krad_osc->stop_listening = 1;
			break;
		}

		if (ret > 0) {
			ret = recvfrom (krad_osc->sd,
			                buffer, OSC_BUF_SIZE, 0,
							        (struct sockaddr *)&remote_address,
							        (socklen_t *)&addr_size);

			if (ret == -1) {
				printke ("Krad OSC Failed on recvfrom");
				krad_osc->stop_listening = 1;
				break;
			}

			if (ret > 0) {
				krad_osc_parse_message (krad_osc, buffer, ret);
			}
		}
	}

  kr_disconnect (krad_osc->client);

	close (krad_osc->sd);
	krad_osc->port = 0;
	krad_osc->listening = 0;

	printk ("Krad OSC Listening thread exiting");

	return NULL;
}

void krad_osc_stop_listening (krad_osc_t *krad_osc) {

	if (krad_osc->listening == 1) {
		krad_osc->stop_listening = 1;
		pthread_join (krad_osc->listening_thread, NULL);
		krad_osc->stop_listening = 0;
	}
}

int krad_osc_listen (krad_osc_t *krad_osc, int port) {

	if (krad_osc->listening == 1) {
		krad_osc_stop_listening (krad_osc);
	}

	krad_osc->port = port;
	krad_osc->listening = 1;

	krad_osc->local_address.sin_family = AF_INET;
	krad_osc->local_address.sin_port = htons (krad_osc->port);
	krad_osc->local_address.sin_addr.s_addr = htonl (INADDR_ANY);

	if ((krad_osc->sd = socket (AF_INET, SOCK_DGRAM, 0)) < 0) {
		printke ("Krad OSC system call socket error");
		krad_osc->listening = 0;
		krad_osc->port = 0;
		return 0;
	}

	if (bind (krad_osc->sd, (struct sockaddr *)&krad_osc->local_address, sizeof(krad_osc->local_address)) == -1) {
		printke ("Krad OSC bind error for udp port %d", krad_osc->port);
		krad_osc->listening = 0;
		krad_osc->port = 0;
		return 0;
	}

	pthread_create (&krad_osc->listening_thread, NULL, krad_osc_listening_thread, (void *)krad_osc);

	return 1;
}


void krad_osc_destroy (krad_osc_t *krad_osc) {

	if (krad_osc->listening == 1) {
		krad_osc_stop_listening (krad_osc);
	}
  kr_client_destroy (&krad_osc->client);
	free (krad_osc);
}

krad_osc_t *krad_osc_create (char *sysname) {

	krad_osc_t *krad_osc = calloc (1, sizeof(krad_osc_t));
	strncpy (krad_osc->sysname, sysname, sizeof(krad_osc->sysname));
  krad_osc->client = kr_client_create ("krad osc client");
  if (krad_osc->client == NULL) {
    printke ("Could create client");
    free (krad_osc);
    return NULL;
  }
	return krad_osc;
}
