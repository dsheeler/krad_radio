#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#define MAX_SCAN 51200
#define MAX_RESULTS 51200

void dirscan_print_results(char **results, int len) {
  while (len--) printf("%s\n", results[len]);
}

void dirscan_free_results(char **results, int len) {
  while (len--) free(results[len]);
  free(results);
}

char **dirscan(char *path, char *match, int *matches) {

  DIR *dirp;
  char **scan_dirs;
  char upstr[4096];
  char pathstr[4096];
  char buf[1024];
  struct dirent *cur_dir;
  int error;
  int i;
  char **results;
  int scanned;
  int max;
  int errors;

  if (matches == NULL) {
    return NULL;
  }
  if (path == NULL) {
    *matches = -1;
    return NULL;
  }
  *matches = 0;
  errors = 0;
  scanned = 0;
  max = MAX_SCAN;
  upstr[4095] = '\0';
  pathstr[4095] = '\0';
  results = calloc(MAX_RESULTS, sizeof(char *));
  scan_dirs = calloc(MAX_SCAN, sizeof(char *));
  scan_dirs[0] = realpath(path, NULL);
  for (;;) {
    for (i = 0; i < max; i++) {
      if (scan_dirs[i] != NULL) {
        break;
      }
    }
    if (i == max) break;
    /*printf("scanning %s\n", scan_dirs[i]);*/
    dirp = opendir(scan_dirs[i]);
    snprintf(upstr, sizeof(upstr) - 1, "%s", scan_dirs[i]);
    free(scan_dirs[i]);
    scan_dirs[i] = NULL;
    if (dirp == NULL) {
      printf("could not scan %s\n", upstr);
      errors++;
      continue;
    }
    for (;;) {
      error = readdir_r(dirp, (struct dirent *)buf, &cur_dir);
      if (error) {
        errno = error;
        perror("readdir_r");
        printf("Could not readdir from %s\n", upstr);
        errors++;
        break;
      }
      if (cur_dir == NULL) break;
      if (cur_dir->d_type == DT_REG) {
        if (strstr(cur_dir->d_name, match)) {
          results[*matches] = strdup(cur_dir->d_name);
          *matches += 1;
          if (*matches + 1 == MAX_RESULTS) break;
        }
      } else if ((cur_dir->d_type == DT_DIR)
        && ((cur_dir->d_name[0] != '.') && (strlen(cur_dir->d_name) != 1))
        && ((memcmp(cur_dir->d_name, "..", 2) != 0)
        && (strlen(cur_dir->d_name) != 2))) {
        for (i = 0; i < max; i++) {
          if (scan_dirs[i] == NULL) {
            snprintf(pathstr, (sizeof(pathstr) - 1), "%s/%s", upstr,
             cur_dir->d_name);
            scan_dirs[i] = realpath(pathstr, NULL);
            /*printf("%s added\n", scan_dirs[i]);*/
            break;
          }
        }
        if (i == max) {
          printf("Limit of dirs to scan reached\n");
          break;
        }
      }
    }
    closedir(dirp);
    scanned++;
    if (*matches + 1 == MAX_RESULTS) {
      printf("result limit reached\n");
      break;
    }
  }
  for (i = 0; i < max; i++) {
    if (scan_dirs[i] != NULL) {
      free(scan_dirs[i]);
      scan_dirs[i] = NULL;
    }
  }
  free(scan_dirs);
  printf("scanned %d dirs %d matches %d errors\n", scanned, *matches, errors);
  return results;
}

int main(int argc, char **argv){

  int ret;
  char **results;
  char *match;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s path\n", argv[0]);
    return 1;
  }

  match = ".h";
  results = dirscan(argv[1], match, &ret);
  if (ret < 0) {
    return 1;
  }

  dirscan_print_results(results, ret);
  dirscan_free_results(results, ret);

  return 0;
}
