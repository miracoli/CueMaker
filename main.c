#include <stdio.h> // for printf
#include <unistd.h> // for getcwd
#include <dirent.h> // for opendir, readdir
#include <string.h> // for strrchr, strcmp
#include <stdbool.h>

bool EndsWiths( char *fileName, char* extension) {
  fileName = strrchr(fileName, '.');

  if( fileName != NULL )
    return( !strcasecmp(fileName, extension) );

  return false;
}

int main(int argc, char **argv) {
  char path[255];
  char fileName[255];
  FILE *fptr;

  printf("Cuesheet Maker (c) 2019 by Dirk Petrautzki\n");
  if(argc > 2) {
    fprintf(stderr, "Usage: %s [path_to_iso_file]\n", argv[0]);
    return 1;
  }
  if(argc == 2) {
    strcpy(path, argv[1]);
  } else {
    if (getcwd(path, sizeof(path)) == NULL) {
      fprintf(stderr, "getcwd() error\n");
      return 1;
    }
  }

  DIR *d;

  struct dirent *dir;

  d = opendir(path);
  int isoCount = 0;
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if(EndsWiths(dir->d_name, ".iso")) {
        ++isoCount;
        strcpy(fileName, dir->d_name);
      }
    }
  } else {
    fprintf(stderr, "Failed to open path %s\n", path);
    return 1;
  }
  if(isoCount == 0) {
    fprintf(stderr, "No ISO file found in path: %s\n", path);
    return 1;
  } else if(isoCount > 1) {
    fprintf(stderr, "Multiple ISO files found in path: %s\n", path);
    return 1;
  }

  printf("%s found.\n", fileName);

  int len = strlen(path);
  path[len] = '/';
  strncpy(&path[len + 1], fileName, strlen(fileName) -3);
  strcpy(&path[len + 1 + strlen(fileName) -3], "cue");

  fptr = fopen(path, "w");

  if(fptr == NULL) {
    fprintf(stderr, "Could not open %s for writing\n", path);
    return 1;
  }

  int trackCounter = 1;
  fprintf(fptr, "FILE \"%s\" BINARY\n", fileName);
  fprintf(fptr, "  TRACK %02d MODE1/2048\n", trackCounter++);
  fprintf(fptr, "    INDEX 01 00:00:00\n");
  fprintf(fptr, "    POSTGAP 00:02:00\n");

  rewinddir(d);

  while ((dir = readdir(d)) != NULL) {
    if(EndsWiths(dir->d_name, ".wav") || EndsWiths(dir->d_name, ".wave") || EndsWiths(dir->d_name, ".flac")) {
      fprintf(fptr, "FILE \"%s\" WAVE\n", dir->d_name);
      printf("%s found.\n", dir->d_name);
      fprintf(fptr, "  TRACK %02d AUDIO\n", trackCounter++);
      fprintf(fptr, "    PREGAP 00:02:00\n");
      fprintf(fptr, "    INDEX 01 00:00:00\n");
    } else if(EndsWiths(dir->d_name, ".mp3")) {
      fprintf(fptr, "FILE \"%s\" MP3\n", dir->d_name);
      printf("%s found.\n", dir->d_name);
      fprintf(fptr, "  TRACK %02d AUDIO\n", trackCounter++);
      fprintf(fptr, "    PREGAP 00:02:00\n");
      fprintf(fptr, "    INDEX 01 00:00:00\n");
    }
  }

  closedir(d);

  fclose(fptr);
  printf("%s generated.\n", path);
  return 0;
}

