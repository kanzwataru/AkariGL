#include "agl_platform.h"
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define FILESYS_CHUNK_SIZE 2097152      /* 2 MiB */

static char *seek_endline(char *p)
{
    assert(p);

    while(1) {
        ++p;
        if(*p == '\n' || *p == 0)
            return p;
    }
}

static char *seek_next_line(char *p)
{
    assert(p);

    p = seek_endline(p) + 1;
    if(*p != 0) {
        return p;
    }
    else {
        return NULL;
    }
}

static char *seek_next_word(char *p)
{
    assert(p);

    ++p;
    while(*p != 0) {
        if(isspace(*p)) {
            if(*(p + 1) == 0)
                return NULL;
            else
                return p + 1;
        }

        ++p;
    }

    return NULL;
}

static bool head_matches(char *p, const char *tok)
{
    size_t tok_len = strlen(tok);
    size_t match_len = 0;

    assert(p);
    assert(tok_len != 0);

    while(*p != 0 && *p != '\n') {
        if(*p == tok[match_len]) {
            ++match_len;
            if(match_len == tok_len) {
                return true;
            }
        }
        else {
            return false;
        }

        ++p;
    }

    /* reached end of line or file, so no match */
    return false;
}

static char *seek_to(char *p, const char *tok)
{
    assert(p);

    while(p && *p != 0) {
        if(head_matches(p, tok)) {
            return p;
        }
        else {
            ++p;
        }
    }
}

static char *seek_first_occurance(char *p, const char *tok)
{
    assert(p);

    while(p && *p != 0 && *p != '\n') {
        if(head_matches(p, tok)) {
            return p;
        }
        else {
            p = seek_next_line(p);
        }
    }

    return NULL;
}

static size_t count_matching_lines(char *p, const char *tok)
{
    assert(p);

    size_t match_count = 0;
    while(p && *p != 0 && *p != '\n') {
        if(head_matches(p, tok)) {
            ++match_count;
        }

        p = seek_next_line(p);
    }

    return match_count;
}

float *agl_filesys_obj_load(const char *path, size_t *vert_num)
{
    struct obj_o {
        float *v;
        unsigned long *v_idx;
        size_t v_count;

        float *vt;
        unsigned long *vt_idx;
        size_t vt_count;

        float *vn;
        unsigned long *vn_idx;
        size_t vn_count;

        size_t f_count;
    } o;

    size_t obj_size;
    char *obj = agl_filesys_load(path, &obj_size);
    char *p = obj;

    p = seek_first_occurance(p, "o"); /* only first object is supported for now */
    assert(p);
    char *o_str_head = p;

    /* count number of things */
    p = o_str_head;
    if((p = seek_first_occurance(p, "v"))) {
        o.v_count = count_matching_lines(p, "v");
    }
    p = o_str_head;
    if((p = seek_first_occurance(p, "vt"))) {
        o.vt_count = count_matching_lines(p, "vt");
    }
    p = o_str_head;
    if((p = seek_first_occurance(p, "vn"))) {
        o.vn_count = count_matching_lines(p, "vn");
    }
    p = o_str_head;
    if((p = seek_first_occurance(p, "f"))) {
        o.f_count = count_matching_lines(p, "f");
    }

    assert(o.v_count && o.f_count);

    /* allocate memory */
    o.v = calloc(o.v_count * 3, sizeof(float));
    o.vt = calloc(o.vt_count * 2, sizeof(float));
    o.vn = calloc(o.vn_count * 3, sizeof(float));
    o.v_idx = calloc(o.f_count * 3, sizeof(unsigned long));
    o.vt_idx = calloc(o.f_count * 3, sizeof(unsigned long));
    o.vn_idx = calloc(o.f_count * 3, sizeof(unsigned long));

    /* parse */
    {
        p = o_str_head;
        float *v = o.v;
        while((p = seek_first_occurance(p, "v"))) {
            char *s, *e;
            e = seek_next_word(p);
            for(int i = 0; i < 3; ++i) {
                s = e;
                e = seek_next_word(s); assert(s);
                v[i] = strtof(s, &e);
            }

            v += 3;
            p = e;
        }
    }

    {
        /*
        p = o_str_head;
        unsigned long *f = o.f;
        while((p = seek_first_occurance(p, "f"))) {
            char *s, *e;
            e = seek_next_word(p);
            s = e;
            for(int i = 0; i < 3; ++ i) {
                for(int j = 0; j < 3; ++j) {
                    s = e;
                    e = seek_first_occurance(s, "/") + 1;
                    f[j] = strtoul(s, &e, 0);
                }
                f += 3;
                e = seek_next_word(s);
            }

            p = e;
        }
        */

        p = o_str_head;
        size_t face_count = 0;
        while((p = seek_first_occurance(p, "f"))) {
            char *s, *e;
            e = seek_next_word(p);
            s = e;
            for(size_t i = 0; i < 3; ++i) {
                s = e;
                e = seek_first_occurance(s, "/") + 1;
                o.v_idx[i + (3 * face_count)] = strtoul(s, &e, 0) - 1;

                s = e;
                e = seek_first_occurance(s, "/") + 1;
                o.vn_idx[i + (3 * face_count)] = strtoul(s, &e, 0) - 1;

                s = e;
                e = seek_first_occurance(s, "/") + 1;
                o.vt_idx[i + (3 * face_count)] = strtoul(s, &e, 0) - 1;
            }

            ++face_count;
            p = e;
        }
    }

    if(o.vt_count) {
        p = o_str_head;
        float *vt = o.vt;
        while((p = seek_first_occurance(p, "vt"))) {
            char *s, *e;
            e = seek_next_word(p);
            for(int i = 0; i < 2; ++i) {
                s = e;
                e = seek_next_word(s); assert(s);
                vt[i] = strtof(s, &e);
            }

            vt += 3;
            p = e;
        }
    }

    if(o.vn_count) {
        p = o_str_head;
        float *vn = o.vn;
        while((p = seek_first_occurance(p, "vn"))) {
            char *s, *e;
            e = seek_next_word(p);
            for(int i = 0; i < 2; ++i) {
                s = e;
                e = seek_next_word(s); assert(s);
                vn[i] = strtof(s, &e);
            }

            vn += 3;
            p = e;
        }
    }

    /* convert to unindexed */
    size_t per_vertex_size = 3 + 2 + 3;
    size_t vert_count = o.f_count * 3;
    float *data = calloc(vert_count * per_vertex_size, sizeof(float));

    float *vert = data;
    for(size_t i = 0; i < vert_count; ++i) {
        float *vpos = &vert[0 + (i * 8)];
        float *vnrm = &vert[3 + (i * 8)];
        float *vuv = &vert[6 + (i * 8)];

        vpos[0] = o.v[0 + o.v_idx[i]];
        vpos[1] = o.v[1 + o.v_idx[i]];
        vpos[2] = o.v[2 + o.v_idx[i]];

        vnrm[0] = o.vn[0 + o.vn_idx[i]];
        vnrm[1] = o.vn[1 + o.vn_idx[i]];
        vnrm[2] = o.vn[2 + o.vn_idx[i]];

        vuv[0] = o.vt[0 + o.vt_idx[i]];
        vuv[1] = o.vt[1 + o.vn_idx[i]];
    }

    /* free temporary data */
    free(o.v);
    free(o.vt);
    free(o.vn);
    free(o.v_idx);
    free(o.vt_idx);
    free(o.vn_idx);

    /* return data */
    *vert_num = vert_count;
    return data;
}

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
