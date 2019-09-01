#ifndef AKARI_PLATFORM_H
#define AKARI_PLATFORM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

char *agl_filesys_load(const char *path, size_t *size_out);
float *agl_filesys_obj_load(const char *path, size_t *vert_num);

#endif /* AKARI_PLATFORM_H */
