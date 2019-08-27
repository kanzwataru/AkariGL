#include "agl_renderer.h"
#include "renderer/debug.h"
#include <stdio.h>
#include <assert.h>

#define MAX_MESHES  4096

struct MeshInfo {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

struct RenderData {
    struct MeshInfo meshes[MAX_MESHES];
    size_t          mesh_top;
};

static struct RenderData *rd;  /* TODO: make this swappable */

void agl_renderer_init(int width, int height)
{
    rd = calloc(1, sizeof(*rd));
    assert(rd);

    glEnable(GL_MULTISAMPLE);
    glViewport(0, 0, width, height);
}

void agl_renderer_quit(void)
{
    free(rd);
    rd = NULL;
}

MeshID agl_upload_new_mesh(struct MeshData *data)
{
    MeshID id = ++rd->mesh_top;
    struct MeshInfo *mesh = &rd->meshes[id];
    assert(id < MAX_MESHES);  /* TODO: proper error handling */

    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->vbo);
    glGenBuffers(1, &mesh->ebo);

    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data->vert_num * 3, data->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * data->elem_num, data->elements, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    check_gl_error();

    return id;
}

void agl_cleanup_mesh(MeshID id)
{
    struct MeshInfo *mesh = &rd->meshes[id];

    glDeleteVertexArrays(1, &mesh->vao);
    glDeleteBuffers(1, &mesh->vbo);
    glDeleteBuffers(1, &mesh->ebo);

    check_gl_error();
}

void agl_render_mesh(MeshID id, struct MeshData *data, ShaderID shader)
{
    struct MeshInfo *mesh = &rd->meshes[id];

    glUseProgram(shader);
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, data->elem_num, GL_UNSIGNED_INT, 0);

    check_gl_error();
}
