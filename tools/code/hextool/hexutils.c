#include "hexutils.h"

static int file_size (const char *filename) {
  struct stat sb;
  if (stat (filename, &sb) != 0) {
    fprintf(stderr, "'stat' failed for '%s': %s.\n",
     filename, strerror(errno));
    exit(EXIT_FAILURE);
  }
  return sb.st_size;
}

void hexdump (char *filename, uint8_t *buffer, int len) {
  int i;
  char *fname;
  int strl;
  char *p;
  FILE *fp;

  fname = basename(filename);

  if (!fname)
    return;

  p = strrchr(fname,'.');
  strl = strlen(fname);

  if (p)
    p[0] = '\0';

  char hname[strl+6];
  sprintf(hname,"kr_%s_%s.h",fname,&p[1]);
  fp = fopen(hname,"a+");

  printf("Generating %s...\n",hname);

  if (fp < 0) {
    fprintf(stderr, "'fopen' failed for '%s': %s.\n",
     hname, strerror(errno));
    exit(EXIT_FAILURE);
  }
  
  fprintf(fp,"const unsigned char kr_%s_%s[] = {",fname,&p[1]);

  for (i = 0; i < len; i++) {
    if (!(i%10)) {
      fprintf(fp,"\n  ");
    }
    
    if (i < (len - 1)) {
      fprintf(fp,"0x%02x, ",buffer[i]);
    } else {
      fprintf(fp,"0x%02x",buffer[i]);
    }
  }

  fprintf(fp,"\n};\n\n");

  fprintf(fp,"const unsigned int kr_%s_%s_len = %d;\n",fname,&p[1],len);

  fclose(fp);

}

int hexgen(char *filenames[], int n) {

  FILE *fp;
  int i;
  int res;
  int nmemb;

  for (i = 0; i < n; i++) {
    nmemb = file_size(filenames[i]);
    fp = fopen(filenames[i], "r");
    if (fp < 0) {
      return -1;
    }
    uint8_t buffer[nmemb];
    res = fread(buffer, sizeof(uint8_t), nmemb, fp);
    if (res == nmemb) {
      hexdump(filenames[i], buffer, nmemb);
    }
  }

  fclose(fp);

  return 0;
}
