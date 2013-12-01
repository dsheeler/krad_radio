#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <common_gen.h>

int main () {

  char buffer[4096];
  int res;
  uber_St uber;
  kr_transponder_path_io_info tpio;
  kr_transponder_path_info tpinfo;

  memset(&tpio,0,sizeof(tpio));
  memset(&tpinfo,0,sizeof(tpinfo));

  uber.type = CGEN_KR_TRANSPONDER_PATH_IO_INFO;
  uber.actual = &tpio;

  printf("kr_transponder_path_io_info to json...\n");

  res = info_pack_to_json(buffer, &uber, 4096);

  buffer[res+1] = '\0';
  printf("%s\n",buffer);

  uber.type = CGEN_KR_TRANSPONDER_PATH_INFO;
  uber.actual = &tpinfo;

  printf("kr_transponder_path_info to json...\n");

  res = info_pack_to_json(buffer, &uber, 4096);

  buffer[res+1] = '\0';
  printf("%s\n",buffer);

/*  kr_sprite_info sprite;
  struct kr_perspective_view persp;
  kr_compositor_path_info comp_path_info;

  memset(&sprite,0,sizeof(sprite));
  memset(&persp,0,sizeof(persp));
  memset(&comp_path_info,0,sizeof(comp_path_info));

  strcpy(sprite.filename,"/path/to/sprite.png");

  kr_sprite_info_random(&sprite);

  uber.type = CGEN_KR_SPRITE_INFO;
  uber.actual = &sprite;

  printf("serializing kr_sprite_info to json...\n");

  res = info_pack_to_json(buffer, &uber, 4096);

  buffer[res+1] = '\0';
  printf("%s\n",buffer);

  kr_perspective_view_random(&persp);

  uber.type = CGEN_KR_PERSPECTIVE_VIEW;
  uber.actual = &persp;

  printf("serializing kr_perspective_view to json...\n");

  res = info_pack_to_json(buffer, &uber, 4096);

  buffer[res+1] = '\0';
  printf("%s\n",buffer);

  kr_compositor_path_info_random(&comp_path_info);

  uber.type = CGEN_KR_COMPOSITOR_PATH_INFO;
  uber.actual = &comp_path_info;

  printf("serializing kr_compositor_path_info to json...\n");

  res = info_pack_to_json(buffer, &uber, 4096);

  buffer[res+1] = '\0';
  printf("%s\n",buffer);*/
  
  return 0;
}
