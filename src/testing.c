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
    mat4 model;
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

void draw_scene(void)
{
    mat4 proj;
    glm_perspective(glm_rad(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, proj);

    mat4 view;
    glm_mat4_identity(view);
    glm_translate(view, (vec3){0.0f, 0.0f, -4.0f});

    vec3 light_dir = {-0.7f, 0.5f, 0.4f};
    glm_normalize(light_dir);

    // spinning cube
    glUseProgram(flat_shader);
    glBindVertexArray(cube_info.vao);

    int model_loc = glGetUniformLocation(flat_shader, "model");
    int view_loc = glGetUniformLocation(flat_shader, "view");
    int proj_loc = glGetUniformLocation(flat_shader, "proj");
    int light_dir_loc = glGetUniformLocation(flat_shader, "light_dir");
    int color_loc = glGetUniformLocation(flat_shader, "color");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &cube_info.model[0][0]);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &proj[0][0]);
    glUniform3fv(light_dir_loc, 1, &light_dir[0]);
    glUniform3f(color_loc, 1.0f, 0.5f, 0.3f);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    // small cube
    mat4 small_model;
    glm_mat4_identity(small_model);
    glm_translate_x(small_model, -0.5f);
    glm_translate_y(small_model, 1.5f);
    glm_scale(small_model, (vec4){0.5f, 0.5f, 0.5f, 1.0f});

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &small_model[0][0]);
    glUniform3f(color_loc, 0.5f, 0.35f, 0.45f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // ground plane
    mat4 ground_model;

    glm_mat4_identity(ground_model);
    glm_translate_y(ground_model, -1.5);
    ground_model[0][0] *= 100;
    ground_model[2][2] *= 100;

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &ground_model[0][0]);
    glUniform3f(color_loc, 0.9f, 0.7f, 0.8f);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

int main(void)
{
    agl_window_init("AkariGL Testing", WIDTH, HEIGHT);
    agl_renderer_init(WIDTH, HEIGHT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    quad_id = agl_upload_new_mesh(&quad_data);
    flat_shader = agl_load_compile_shader("res/shaders/flat.vert", "res/shaders/flat.frag");

    upload_cube();
    glm_mat4_identity(cube_info.model);

    glClearColor(0.2f, 0.3f, 0.3f, 0.0f);

    while(!agl_window_should_quit()) {
        // *** update ***
        glm_rotate_x(cube_info.model, glm_rad(1.0f), cube_info.model);
        glm_rotate_y(cube_info.model, glm_rad(0.5f), cube_info.model);
        glm_rotate_z(cube_info.model, glm_rad(0.2f), cube_info.model);

        // *** render ***
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        draw_scene();

        agl_window_swap_buffers();
    }

    agl_renderer_quit();
    agl_window_quit();
    
    return 0;
}
