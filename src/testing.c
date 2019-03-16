#include "agl.h"

#define WIDTH  800
#define HEIGHT 600

static float quad_verts[] = {
    0.5f,   0.5f, 0.0f,
    0.5f,  -0.5f, 0.0f,
   -0.5f,  -0.5f, 0.0f,
   -0.5f,   0.5f, 0.0f
};

static GLuint quad_elements[] = {
    0, 1, 2,
    0, 2, 3
};

static struct MeshData quad_data = {
    quad_verts,
    quad_elements,
    sizeof(quad_verts) / sizeof(float) / 3,
    sizeof(quad_elements) / sizeof(GLuint)
};

static ShaderID flat_shader;
static MeshID   quad_id;

int main(void)
{
    agl_window_init("AkariGL Testing", WIDTH, HEIGHT);
    agl_renderer_init(WIDTH, HEIGHT);

    quad_id = agl_upload_new_mesh(&quad_data);
    flat_shader = agl_load_compile_shader("res/shaders/flat.vert", "res/shaders/flat.frag");

    glClearColor(0.2f, 0.3f, 0.3f, 0.0f);

    while(!agl_window_should_quit()) {
        glClear(GL_COLOR_BUFFER_BIT);

        agl_render_mesh(quad_id, &quad_data, flat_shader);

        agl_window_swap_buffers();
    }

    agl_renderer_quit();
    agl_window_quit();
    
    return 0;
}
