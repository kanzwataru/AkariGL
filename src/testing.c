#include "agl.h"
#define DEBUG
#include "renderer/debug.h"

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

static float cube_vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

static struct MeshData quad_data = {
    quad_verts,
    quad_elements,
    sizeof(quad_verts) / sizeof(float) / 3,
    sizeof(quad_elements) / sizeof(GLuint)
};

static struct {
    GLuint vao;
    GLuint vbo;
} cube_info;

static ShaderID flat_shader;
static MeshID   quad_id;

void upload_cube(void)
{
    glGenVertexArrays(1, &cube_info.vao);
    glGenBuffers(1, &cube_info.vbo);

    glBindVertexArray(cube_info.vao);

    glBindBuffer(GL_ARRAY_BUFFER, cube_info.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

int main(void)
{
    agl_window_init("AkariGL Testing", WIDTH, HEIGHT);
    agl_renderer_init(WIDTH, HEIGHT);

    glEnable(GL_DEPTH_TEST);

    quad_id = agl_upload_new_mesh(&quad_data);
    flat_shader = agl_load_compile_shader("res/shaders/flat.vert", "res/shaders/flat.frag");

    upload_cube();

    mat4 proj;
    glm_perspective(glm_rad(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, proj);

    mat4 model;
    glm_mat4_identity(model);

    mat4 view;
    glm_mat4_identity(view);
    glm_translate(view, (vec3){0.0f, 0.0f, -3.0f});

    vec3 light_dir = {0.5f, 0.2f, 0.1f};
    glm_normalize(light_dir);

    glClearColor(0.2f, 0.3f, 0.3f, 0.0f);

    while(!agl_window_should_quit()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(flat_shader);
        glBindVertexArray(cube_info.vao);

        glm_rotate_x(model, glm_rad(1.0f), model);
        glm_rotate_y(model, glm_rad(0.5f), model);
        glm_rotate_z(model, glm_rad(0.2f), model);

        int model_loc = glGetUniformLocation(flat_shader, "model");
        int view_loc = glGetUniformLocation(flat_shader, "view");
        int proj_loc = glGetUniformLocation(flat_shader, "proj");
        int light_dir_loc = glGetUniformLocation(flat_shader, "light_dir");
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &proj[0][0]);
        glUniform3fv(light_dir_loc, 1, &light_dir[0]);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        //agl_render_mesh(quad_id, &quad_data, flat_shader);

        agl_window_swap_buffers();
    }

    agl_renderer_quit();
    agl_window_quit();
    
    return 0;
}
