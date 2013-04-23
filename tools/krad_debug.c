static char logfile[256];

void show_log () {

  int ret;
  char *args[3];

  args[0] = "cat";
  args[1] = logfile;
  args[2] = NULL;

  ret = execv ("/bin/cat", args);
  if (ret == -1) {
    printf ("Error running cat...\n");
  }
}

void krad_debug_init (char *name) {

  krad_system_init ();

  sprintf (logfile, "%s/kr_%s_%"PRIu64".log",
           getenv ("HOME"), name, krad_unixtime ());

  krad_system_log_on (logfile);

  printf ("Logging to: %s\n", logfile);
}

void krad_debug_shutdown () {
  krad_system_log_off ();

  printf ("\nDebug log: %s\n\n", logfile);
  
  show_log ();
}
