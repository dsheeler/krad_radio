#ifndef KRS_BATCH_H
#define KRS_BATCH_H
#include "krad_shell.h"

void krs_replay (kr_shell_t *kr_shell,char **cmdsplit);
void krs_save (kr_shell_t *kr_shell,char **cmdsplit);
void krs_run (kr_shell_t *kr_shell,char **cmdsplit);

#endif