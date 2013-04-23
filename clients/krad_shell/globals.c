#include "globals.h"

int resized = 0;

kr_cdef_t krad_shell_cmd_def[] = {
  {"record","video /my/video/path/video.webm vp8 0 0 0 0"},
  {"transmit","video host port mount password vp8 0 0 0 0"},
  {"addtext","\"my text\" 0 0 0 4 1.0f 1.0f 0.0f 0.255f 0.255f 0.255f \"sans\""},
  {"settext","id 0 0 0 4 1.0f 1.0f 0.0f 0.255f 0.255f 0.255f"},
  {"addsprite","/path/to/sprite.jpg 0 0 0 4 1.0f 1.0f 0.0f"},
  {"setsprite","id 0 0 0 4 1.0f 1.0f 0.0f"},
  {"setport","id 0 0 0 0 0 0 0 0 1.0f 0.0f"}
};

char *cmdhelp[] = {"help: addfx","help: addsprite","help: addtext","help: capture","help: clear","help: closedisplay","help: comp","help: cpu","help: destroy","help: display","help: exit","help: fps","help: getrate","help: help","help: info","help: input","help: jsnap","help: kill","help: launch","help: lc","help: link","help: listen_off","help: listen_on","help: ll","help: lm","help: load","help: ls","help: lssprites","help: lstext","help: map","help: mix","help: mixmap","help: noxmms2","help: oscoff","help: oscon","help: output","help: play","help: plug","help: quit","help: receiver_off","help: receiver_on","help: record","help: remoteoff","help: remoteon","help: replay","help: res","help: rmfx","help: rmport","help: rmsprite","help: rmtext","help: run","help: save","help: set","help: setdir","help: setfx","help: setport","help: setrate","help: setsprite","help: sleep","help: snap","help: stag","help: tag","help: tags","help: tone","help: transmitter_off","help: transmitter_on","help: unplug","help: update","help: uptime","help: weboff","help: webon","help: xmms2"};

char *cmds[] = {"addfx","addsprite","addtext","capture","clear","closedisplay","comp","cpu","destroy","display","exit","fps","getrate","help","info","input","jsnap","kill","launch","lc","link","listen_off","listen_on","ll","lm","load","ls","lssprites","lstext","map","mix","mixmap","noxmms2","oscoff","oscon","output","play","plug","quit","receiver_off","receiver_on","record","remoteoff","remoteon","replay","res","rmfx","rmport","rmsprite","rmtext","run","save","set","setdir","setfx","setport","setrate","setsprite","sleep","snap","stag","tag","tags","tone","transmitter_off","transmitter_on","unplug","update","uptime","weboff","webon","xmms2"};

int cmds_n = ARRAY_SIZE(cmds);
int cmd_defs_n = ARRAY_SIZE(krad_shell_cmd_def);
