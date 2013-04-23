#include "batch.h"


void krs_replay (kr_shell_t *kr_shell,char **cmdsplit) {
	int p;
	int m;
	char **splitted;
	int to = atoi (cmdsplit[1]);
	history_t *history = kr_shell->history;
	char **his = history->his;
	int h = kr_shell->cmds;

	char *sysname = kr_shell->sysname;

	if (to <= h-1) {

		for (p=to+1;p>1;p--) {

			splitted = split (his[h-p],&m);

			if (iscmd (splitted[0],kr_shell)) {
				krad_shell_cmd (kr_shell,sysname,m,splitted);
			}
			else {

                  // cmd not valid 
				char *errstr1 = " is not a valid command";
				char *errstr = calloc (strlen (errstr1)+strlen (splitted[m-1]) + 1,sizeof (char));
				sprintf (errstr,"%s%s",splitted[m-1],errstr1);
				shell_print (errstr);
				free (errstr);

			}

			free (splitted);

		}

	}
}

void krs_save (kr_shell_t *kr_shell,char **cmdsplit) {

	int p;
	int j=0;
	int to = atoi (cmdsplit[1]);
	kr_batch_t *batch = calloc (1, sizeof(kr_batch_t));
	kr_shell->batches = realloc (kr_shell->batches,sizeof (kr_batch_t*)*(kr_shell->nbatches+1));
	history_t *history = kr_shell->history;
	char **his = history->his;
	int h = kr_shell->cmds;

	if (to <= h-1) {

    // setting batch name 

		batch->name = strndup (cmdsplit[2],strlen (cmdsplit[2]) + 1);

    // allocating space for commands inside batch struct 

		batch->cmds = calloc (to, sizeof (char*));
		batch->ncmds = 0;

		for (p=to+1;p>1;p--) {

			batch->cmds[j] = strndup (his[h-p],strlen (his[h-p]) + 1);
			batch->ncmds++;
			j++;

		}

		kr_shell->batches[kr_shell->nbatches] = batch;
		kr_shell->nbatches++;

	}
}

void krs_run (kr_shell_t *kr_shell,char **cmdsplit) {

	int i,j;
	int found = 0;
	int m;
	kr_batch_t **batches = kr_shell->batches;
	kr_batch_t *batch;
	char **splitted;
	char *sysname = kr_shell->sysname;

	for (i=0;i<kr_shell->nbatches;i++) {

		batch = batches[i];

		if (!strcmp (cmdsplit[1],batch->name)) {

			for (j=0;j<batch->ncmds;j++) {

				splitted = split (batch->cmds[j],&m);

				if (iscmd (splitted[0],kr_shell)) {
					krad_shell_cmd (kr_shell,sysname,m,splitted);
				}
				else {

          // cmd not valid 
					char *errstr1 = " is not a valid command";
					char *errstr = calloc (strlen (errstr1)+strlen (splitted[0]) + 1,sizeof (char));
					sprintf (errstr,"%s%s",splitted[0],errstr1);
					shell_print (errstr);
					free (errstr);
				}

				free (splitted);
			}

			found++;
		}

	}

	if (!found) {
		shell_print ("No batch with that name");
	}
}