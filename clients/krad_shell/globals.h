#ifndef KRS_GLOBALS_H
#define KRS_GLOBALS_H

#include "krad_shell.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#ifndef max
  #define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
  #define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif

extern int cmds_n;
extern int cmd_defs_n;
extern char *cmdhelp[];
extern char *cmds[];
extern int resized;
extern kr_cdef_t krad_shell_cmd_def[];

#endif
