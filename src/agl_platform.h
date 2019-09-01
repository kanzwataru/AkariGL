#ifndef AKARI_PLATFORM_H
#define AKARI_PLATFORM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

char *agl_filesys_load(const char *path, size_t *size_out);

#endif /* AKARI_PLATFORM_H */
