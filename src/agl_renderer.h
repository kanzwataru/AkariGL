#ifndef AKARI_RENDERER_H
#define AKARI_RENDERER_H
#include <stdlib.h>
#include <glad/glad.h>

typedef size_t MeshID;
typedef GLuint ShaderID;

struct MeshData {
    float  *vertices;
    GLuint *elements;
    size_t  vert_num;
    size_t  elem_num;
};

/* mesh functions */
MeshID agl_upload_new_mesh(struct MeshData *data);
void agl_cleanup_mesh(MeshID id);
void agl_render_mesh(MeshID id, struct MeshData *data, ShaderID shader);
/* */

/* shader functions */
ShaderID agl_compile_shader(const char *vert_src, const char *frag_src, const char *geom_src);
ShaderID agl_load_compile_shader(const char *vert_path, const char *frag_path, const char *geom_path);
/* */

/* renderer functions */
void agl_renderer_init(int width, int height);
void agl_renderer_quit(void);

#endif /* AKARI_RENDERER_H */
