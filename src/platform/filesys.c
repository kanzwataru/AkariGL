#include "agl_platform.h"
#define FILESYS_CHUNK_SIZE 2097152      /* 2 MiB */

char *agl_filesys_load(const char *path, size_t *size_out) 
{
    FILE *file;
    char *data = NULL;
    char *temp;
    size_t size = 0;
    size_t used = 0;
    size_t n;

    file = fopen(path, "rb");
    if(!file)
        goto file_err;

    while(1) {
        /* check if we need to allocate another chunk's worth */
        if(used + FILESYS_CHUNK_SIZE + 1 > size) {
            size = used + FILESYS_CHUNK_SIZE +1;

            if(size <= used) /* integer overflow check */
                goto data_err;

            temp = realloc(data, size);
            if(!temp)
                goto data_err;

            data = temp;
        }

        /* read chunk */
        n = fread(data + used, 1, FILESYS_CHUNK_SIZE, file);
        if(n == 0)
            break;

        used += n;
    }

    /* last minute error checking */
    if(ferror(file))
        goto data_err;

    /* shrink buffer to size of data */
    temp = realloc(data, used + 1);
    if(!temp)
        goto data_err;

    data = temp;
    data[used] = '\0'; /* null-terminate just in case */

    /* size_out is not required and can be NULL */
    if(size_out)
        *size_out = size;

    return data;
data_err:
    free(data);
    fclose(file);
file_err:
    return NULL;
}
