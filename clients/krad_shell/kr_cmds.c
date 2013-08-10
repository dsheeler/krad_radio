#include "krad_shell.h"

void kr_delivery_final_reset (kr_client_t *client);

void kr_client_response_wait_print_curses (kr_client_t *client) {

  kr_crate_t *crate;
  char *string;
  int wait_time_ms;
  int length;
  int got_all_delivery;

  got_all_delivery = 0;
  string = NULL;
  crate = NULL;  
  wait_time_ms = 250;

  while (1) {
    kr_delivery_get (client, &crate);
    if (crate == NULL) {
      if (kr_poll (client, wait_time_ms)) {
        kr_delivery_recv (client);
      } else {
        break;
      }
    } else {
      length = kr_uncrate_string (crate, &string);
      if (length > 0) {
        printw ("%s\n", string);
        kr_string_recycle (&string);
      }
      kr_crate_recycle (&crate);
      if (kr_delivery_final (client)) {
        kr_delivery_final_reset (client);
        got_all_delivery = 1;
        break;
      }
    }
  }

  if (got_all_delivery == 0) {
    printw ("No response after waiting %dms\n", wait_time_ms);
  }
}

int get_logname (kr_client_t *client, char *logname, int max) {

  kr_crate_t *crate;
  crate = NULL;
  int wait_ms;
  int ret;
  
  kr_system_info (client);
  wait_ms = 250;
  ret = 0;

  while (kr_delivery_get_until_final (client, &crate, wait_ms)) {
    if (crate != NULL) {
      if (kr_crate_loaded (crate)) {
        if ((crate->addr->path.unit == KR_STATION) && (crate->addr->path.subunit.zero == KR_STATION_UNIT)) {
          ret = strlen(crate->inside.radio->logname);
          if (ret > 0) {
            if (ret < max) {
              strncpy (logname, crate->inside.radio->logname, max);
              printw ("log %s\n", logname);
            } else {
              ret = 0;
            }
          }
        }
      }
      kr_crate_recycle (&crate);
    }
  }
  return ret;
}

void print_logname (kr_client_t *client) {

  char logname[256];

  if (get_logname (client, logname, 256)) {
    printw ("%s\n", logname);
  }
}

void tail_log (kr_client_t *client, int lines, int dofollow) {

  int ret;
  char logname[256];
  char last[32];
  char *args[6];
  
  if (lines < 0) {
    lines = 0;
  }
  
  snprintf (last, sizeof (last), "%d", lines);

  if (get_logname (client, logname, 256)) {
    kr_client_destroy (&client);
    args[0] = "tail";
    args[1] = "-n";
    args[2] = last;
    if (dofollow == 1) {
      args[3] = "-f";
      args[4] = logname;
      args[5] = NULL;    
    } else {
      args[3] = logname;
      args[4] = NULL;
    }
    ret = execv ("/usr/bin/tail", args);
    if (ret == -1) {
      printw ("Error running tail...\n");
    }
    exit (1);
  }
}


int krad_shell_cmd (kr_shell_t *kr_shell , char *sysname ,int argc , char *argv[]) {

	kr_client_t *client;
  int ret;
  kr_unit_control_t uc;

  client = kr_shell->client;
  ret = 0;

 /* Krad Radio Commands */

  if ((strncmp(argv[0], "ls", 2) == 0) && (strlen(argv[0]) == 2)) {
    if (argc == 1) {

      kr_mixer_portgroup_list (client);
      kr_client_response_wait_print_curses (client);

      kr_compositor_subunit_list (client);
      kr_client_response_wait_print_curses (client);

      kr_transponder_subunit_list (client);
      kr_client_response_wait_print_curses (client);
    }
  }

  if ((((strncmp(argv[0], "lsv", 3) == 0) || 
        (strncmp(argv[0], "lsd", 3) == 0) ||
        (strncmp(argv[0], "lsa", 3) == 0)) &&
        (strlen(argv[0]) == 3)) || (strncmp(argv[0], "adapters", 8) == 0)) {
    if (argc == 1) {
      kr_transponder_adapters (client);
      kr_client_response_wait_print_curses (client);
    }
  }

  if ((strlen(argv[0]) == 4) && (strncmp(argv[0], "info", 4) == 0)) {
    kr_system_info (client);
    kr_client_response_wait_print_curses (client);
    printf ("\n");
    kr_mixer_info (client);
    kr_client_response_wait_print_curses (client);
    printf ("\n");
    kr_compositor_info (client);
    kr_client_response_wait_print_curses (client);
    printf ("\n");
    kr_transponder_info_get (client);
    kr_client_response_wait_print_curses (client);
  }

  if ((strlen(argv[0]) == 7) && (strncmp(argv[0], "sysinfo", 7) == 0)) {
    kr_system_info (client);
    kr_client_response_wait_print_curses (client);
  }
  
  if (strncmp(argv[0], "setdir", 6) == 0) {
    if (argc == 2) {
      kr_set_dir (client, argv[1]);
    }
  }

  if (strncmp(argv[0], "log", 3) == 0) {
    print_logname (client);
  }
  
  if ((strncmp(argv[0], "tailf", 5) == 0) || (strncmp(argv[0], "ftail", 5) == 0)) {
    if (argc == 1) {
      tail_log (client, 25, 1);
    }
    if (argc == 2) {
      tail_log (client, atoi(argv[1]), 1);
    }    
  }

  if (strncmp(argv[0], "tail", 4) == 0) {
    if (argc == 1) {
      tail_log (client, 25, 0);
    }
    if (argc == 2) {
      tail_log (client, atoi(argv[1]), 0);
    }    
  }

  if (strncmp(argv[0], "tags", 4) == 0) {

    if (argc == 1) {
      kr_tags (client, NULL);    
      kr_client_response_wait_print_curses (client);
    }
    if (argc == 2) {
      kr_tags (client, argv[1]);    
      kr_client_response_wait_print_curses (client);
    }          

  } else {
    if (strncmp(argv[0], "tag", 3) == 0) {
      if (argc == 2) {
        kr_tag (client, NULL, argv[1]);
        kr_client_response_wait_print_curses (client);
      }
      if (argc == 3) {
        kr_tag (client, argv[1], argv[2]);
        kr_client_response_wait_print_curses (client);            
      }    
    }
  }

  if (strncmp(argv[0], "stag", 4) == 0) {
    if (argc == 3) {
      kr_set_tag (client, NULL, argv[1], argv[2]);
    }
    if (argc == 4) {
      kr_set_tag (client, argv[1], argv[2], argv[3]);
    }
  }

  if (((strlen(argv[0]) == 6) && (strncmp(argv[0], "remote", 6) == 0)) || 
      ((strlen(argv[0]) == 7) && (strncmp(argv[0], "remotes", 7) == 0))) {
    kr_remote_list (client);
    kr_client_response_wait_print_curses (client);
  }

  if ((strncmp(argv[0], "remoteon", 8) == 0) || (strncmp(argv[0], "remote_on", 9) == 0)) {
    ret = 0;
    if (argc == 2) {
      ret = kr_remote_on (client, NULL, atoi(argv[1]));
    }
    if (argc == 3) {
      ret = kr_remote_on (client, argv[1], atoi(argv[2]));
    }
    if (ret == -1) {
      printf ("Invalid Port Specified\n");
    }
  }  

  if ((strncmp(argv[0], "remoteoff", 9) == 0) || (strncmp(argv[0], "remote_off", 10) == 0)) {
    ret = 0;
    if (argc == 1) {
      ret = kr_remote_off (client, NULL, 0);
    }
    if (argc == 2) {
      ret = kr_remote_off (client, NULL, atoi(argv[1]));
    }
    if (argc == 3) {
      ret = kr_remote_off (client, argv[1], atoi(argv[2]));
    }
    if (ret == -1) {
      printf ("Invalid Port Specified\n");
    }
  }

  if ((strncmp(argv[2], "webon", 5) == 0) || (strncmp(argv[2], "interweb", 5))) {
    if (argc == 4) {
      kr_web_enable (client, atoi(argv[3]), "", "", "");
    }
    if (argc == 5) {
      kr_web_enable (client, atoi(argv[3]), argv[4], "", "");
    }
    if (argc == 6) {
      kr_web_enable (client, atoi(argv[3]), argv[4], argv[5], "");
    }
    if (argc == 7) {
      kr_web_enable (client, atoi(argv[3]), argv[4], argv[5], argv[6]);
    }
  }     

  if (strncmp(argv[0], "weboff", 6) == 0) {
    if (argc == 1) {
      kr_web_disable (client);
    }
  }

  if (strncmp(argv[0], "oscon", 5) == 0) {
    if (argc == 2) {
      kr_osc_enable (client, atoi(argv[1]));
    }
  }      

  if (strncmp(argv[0], "oscoff", 6) == 0) {
    if (argc == 1) {
      kr_osc_disable (client);
    }
  }

  /* Krad Mixer Commands */

  if (strncmp(argv[0], "lm", 2) == 0) {
    if (argc == 1) {
      kr_mixer_portgroup_list (client);
      kr_client_response_wait_print_curses (client);
    }
  }

  if (((strncmp(argv[0], "m", 3) == 0) && (strlen(argv[0]) == 1)) ||
      ((strncmp(argv[0], "mix", 3) == 0) && (strlen(argv[0]) == 3)) ||
      ((strncmp(argv[0], "mixer", 5) == 0) && (strlen(argv[0]) == 5))) {
    if (argc == 1) {
      kr_mixer_info (client);
      kr_client_response_wait_print_curses (client);
    }
  }      

  if (strncmp(argv[0], "setrate", 7) == 0) {
    if (argc == 2) {
      kr_mixer_set_sample_rate (client, atoi(argv[1]));
    }
  }        

  if (strncmp(argv[0], "tone", 4) == 0) {
    if (argc == 2) {
      kr_mixer_push_tone (client, argv[1][0]);
    }
  }

  if (strncmp(argv[0], "portinfo", 8) == 0) {
    if (argc == 2) {
      kr_mixer_portgroup_info (client, argv[1]);
      kr_client_response_wait_print_curses (client);
    }
  }

  if (strncmp(argv[2], "input", 5) == 0) {
    if (argc == 4) {
      kr_mixer_create_portgroup(client, argv[3], "input", "jack", 2);
    }
    if (argc == 5) {
      kr_mixer_create_portgroup(client, argv[3], "input", "jack", atoi(argv[4]));
    }
  }      

  if (strncmp(argv[2], "output", 6) == 0) {
    if (argc == 4) {
      kr_mixer_create_portgroup(client, argv[3], "output", "jack", 2);
    }
    if (argc == 5) {
      kr_mixer_create_portgroup(client, argv[3], "output", "jack", atoi(argv[4]));
    }
  }
  
  if (strncmp(argv[2], "auxout", 6) == 0) {
    if (argc == 4) {
      kr_mixer_create_portgroup(client, argv[3], "auxout", "jack", 2);
    }
    if (argc == 5) {
      kr_mixer_create_portgroup(client, argv[3], "auxout", "jack", atoi(argv[4]));
    }
  }

  if (strncmp(argv[0], "plug", 4) == 0) {
    if (argc == 3) {
      kr_mixer_plug_portgroup (client, argv[1], argv[2]);
    }
  }

  if (strncmp(argv[0], "unplug", 6) == 0) {
    if (argc == 2) {
      kr_mixer_unplug_portgroup (client, argv[1], "");
    }
    if (argc == 3) {
      kr_mixer_unplug_portgroup (client, argv[1], argv[2]);
    }        
  }            

  if (strncmp(argv[0], "map", 3) == 0) {
    if (argc == 4) {
      kr_mixer_update_portgroup_map_channel (client, argv[1], atoi(argv[2]), atoi(argv[3]));
    }
  }

  if (strncmp(argv[0], "mixmap", 3) == 0) {
    if (argc == 4) {
      kr_mixer_update_portgroup_mixmap_channel (client, argv[1], atoi(argv[2]), atoi(argv[3]));
    }
  }      

  if (strncmp(argv[0], "crossfade", 9) == 0) {
    if (argc == 2) {
      kr_mixer_set_portgroup_crossfade_group (client, argv[1], "");
    }
    if (argc == 3) {
      kr_mixer_set_portgroup_crossfade_group (client, argv[1], argv[2]);
    }
  }
  
  if ((strncmp(argv[0], "rmcrossfade", 11) == 0) || (strncmp(argv[0], "nocrossfade", 11) == 0)) {
    if (argc == 2) {
      kr_mixer_set_portgroup_crossfade_group (client, argv[1], "");
    }
  }     

  if (strncmp(argv[0], "xmms2", 5) == 0) {
    if (argc == 3) {
      if ((strncmp(argv[2], "play", 4) == 0) || (strncmp(argv[2], "pause", 5) == 0) ||
        (strncmp(argv[2], "stop", 4) == 0) || (strncmp(argv[2], "next", 4) == 0) ||
        (strncmp(argv[2], "prev", 4) == 0)) {
        kr_mixer_portgroup_xmms2_cmd (client, argv[1], argv[2]);
        return 0;
      } else {
        kr_mixer_bind_portgroup_xmms2 (client, argv[1], argv[2]);
      }
    }
  }  

  if (strncmp(argv[0], "noxmms2", 7) == 0) {
    if (argc == 2) {
      kr_mixer_unbind_portgroup_xmms2 (client, argv[1]);
    }
  }

  if (((argc == 3) || (argc == 4)) &&
      (((strlen(argv[0]) == 1) && (strncmp(argv[0], "s", 1) == 0)) ||
       ((strlen(argv[0]) == 3) && (strncmp(argv[0], "set", 3) == 0)))) {

    memset (&uc, 0, sizeof (uc));
    if (kr_string_to_address (argv[1], &uc.address)) {
      if (argc == 3) {
        uc.value.real = atof(argv[2]);
        uc.duration = 0;
      }
      if (argc == 4) {
        uc.value.real = atof(argv[2]);
        uc.duration = atoi(argv[3]);
      }
      kr_unit_control_set (client, &uc);
    }
  }
  
  if (((argc == 2) || (argc == 3)) &&
      (((strlen(argv[0]) > 2) && (strchr(argv[0], '/') != NULL)) ||
       (0))) {

    memset (&uc, 0, sizeof (uc));
    if (kr_string_to_address (argv[0], &uc.address)) {
      kr_unit_control_data_type_from_address (&uc.address, &uc.data_type);
      if (uc.data_type == KR_FLOAT) {
        uc.value.real = atof(argv[1]);
      }
      if (uc.data_type == KR_INT32) {
        uc.value.integer = atoi(argv[1]);
      }
      if (uc.data_type == KR_STRING) {
        uc.value.string = argv[1];
      }
      if (argc == 2) {
        uc.duration = 0;
      }
      if (argc == 3) {
        uc.duration = atoi(argv[2]);
      }
      kr_unit_control_set (client, &uc);
    }
  }
  
  if ((argc == 2) && ((strncmp(argv[0], "rm", 2) == 0) && (strlen(argv[0]) == 2)) &&
      (((strlen(argv[1]) > 2) && (strchr(argv[1], '/') != NULL)) ||
       (0))) {

    memset (&uc, 0, sizeof (uc));
    if (kr_string_to_address (argv[1], &uc.address)) {
      kr_unit_destroy (client, &uc.address);
    }
  }

  /* Krad Compositor Commands */

  if ((strncmp(argv[0], "lc", 2) == 0) && (strlen(argv[0]) == 2)) {
    if (argc == 1) {
      kr_compositor_subunit_list (client);
      kr_client_response_wait_print_curses (client);
    }
  }
  
  if ((strlen(argv[0]) == 9) && (strncmp(argv[0], "addsprite", 9) == 0)) {
    if (argc == 2) {
      kr_compositor_subunit_create (client, KR_SPRITE, argv[1], NULL);
    }
  }
  
  if ((strlen(argv[0]) == 7) && (strncmp(argv[0], "addtext", 7) == 0)) {
    if (argc == 2) {
      kr_compositor_subunit_create (client, KR_TEXT, argv[1], NULL);
    }
    if (argc == 3) {
      kr_compositor_subunit_create (client, KR_TEXT, argv[1], argv[2]);
    }
  }
  
  if ((strlen(argv[0]) > 5) && (strncmp(argv[0], "addvec", 6) == 0)) {
    if (argc == 2) {
      kr_compositor_subunit_create (client, KR_VECTOR, argv[1], NULL);
    }
  }

  if ((strncmp(argv[0], "comp", 4) == 0) ||
      ((strncmp(argv[0], "c", 1) == 0) && (strlen(argv[0]) == 1))) {
    if (argc == 1) {
      kr_compositor_info (client);
      kr_client_response_wait_print_curses (client);
    }
  }

  if (strncmp(argv[0], "res", 3) == 0) {
    if (argc == 3) {
      kr_compositor_set_resolution (client, atoi(argv[1]), atoi(argv[2]));
    }
  }

  if (strncmp(argv[0], "fps", 3) == 0) {
    if (argc == 2) {
      kr_compositor_set_frame_rate (client, atoi(argv[1]) * 1000, 1000);
    }      
    if (argc == 3) {
      kr_compositor_set_frame_rate (client, atoi(argv[1]), atoi(argv[2]));
    }
  }

  if (strncmp(argv[0], "background", 10) == 0) {
    if (argc == 2) {
      kr_compositor_background (client, argv[1]);
    }
  }      

  /* Krad Transponder Commands */      

  if ((strlen(argv[0]) == 4) && (strncmp(argv[0], "xpdr", 4) == 0)) {
    kr_transponder_info (client);
    kr_client_response_wait_print_curses (client);
  }

  if ((strncmp(argv[0], "ll", 2) == 0) && (strlen(argv[0]) == 2)) {
    if (argc == 1) {
      kr_transponder_subunit_list (client);
      kr_client_response_wait_print_curses (client);
    }
  }

  if (strncmp(argv[0], "record", 7) == 0) {
    if (argc == 2) {
      kr_transponder_subunit_create (client, "record", argv[1]);
    }
  }
  
  if (strncmp(argv[0], "encode", 6) == 0) {
    if (argc == 2) {
      kr_transponder_subunit_create (client, "encode", argv[1]);
    }
  }
  
  if (strncmp(argv[0], "decode", 6) == 0) {
    if (argc == 2) {
      kr_transponder_subunit_create (client, "decode", argv[1]);
    }
  }
  
  if ((strncmp(argv[0], "play", 4) == 0) ||
      (strncmp(argv[0], "demux", 5) == 0)) {
    if (argc == 2) {
      kr_transponder_subunit_create (client, "demux", argv[1]);
    }
  }

  if (strncmp(argv[0], "mux", 3) == 0) {
    if (argc == 2) {
      kr_transponder_subunit_create (client, "mux", argv[1]);
    }
  }
  
  if (strncmp(argv[0], "capture", 7) == 0) {
    if (argc == 2) {
      kr_transponder_subunit_create (client, "rawin", argv[1]);
    }
  }

  if (strncmp(argv[0], "display", 7) == 0) {
    if (argc == 1) {
      kr_transponder_subunit_create (client, "rawout", "");
    }
  }

  if (strncmp(argv[0], "receiver_on", 11) == 0) {
    if (argc == 2) {
      kr_transponder_receiver_enable (client, atoi(argv[1]));
    }
  }

  if (strncmp(argv[0], "receiver_off", 12) == 0) {
    if (argc == 1) {
      kr_transponder_receiver_disable (client);
    }
  }

  if (strncmp(argv[0], "transmitter_on", 14) == 0) {
    if (argc == 2) {
      kr_transponder_transmitter_enable (client, atoi(argv[1]));
    }
  }

  if (strncmp(argv[0], "transmitter_off", 15) == 0) {
    if (argc == 1) {
      kr_transponder_transmitter_disable (client);
    }
  }

  return 0;

}




