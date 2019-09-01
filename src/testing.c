#include "agl.h"
#define DEBUG
#include "renderer/debug.h"
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

#define WIDTH  800
#define HEIGHT 600

/*
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
*/

struct Model {
    float *data;
    size_t vert_count;
    GLuint vao;
    GLuint vbo;
    mat4 model;
};

static struct Model cube_info;

static ShaderID debug_shader;
static ShaderID flat_shader;
static ShaderID shad_shader;
static MeshID   quad_id;
static vec3 light_dir = {0,0,0};
static unsigned int counter;

void load_obj(const char *path, float **data, size_t *out_vert_count)
{
    char *obj_file;
    size_t obj_file_size;

    tinyobj_material_t *materials;
    tinyobj_shape_t *shapes;
    tinyobj_attrib_t attrib;
    size_t material_count;
    size_t shape_count;

    obj_file = agl_filesys_load(path, &obj_file_size);
    assert(obj_file);
    obj_file_size = strlen(obj_file);

    int ret = tinyobj_parse_obj(&attrib, &shapes, &shape_count, &materials, &material_count, obj_file, obj_file_size, TINYOBJ_FLAG_TRIANGULATE);
    if(ret != 0) {
        fprintf(stderr, "obj import failed: %d\n", ret);
        exit(EXIT_FAILURE);
    }

    *data = calloc(attrib.num_faces, 8 * sizeof(float));
    assert(data);

    *out_vert_count = attrib.num_faces;

    size_t face_offset = 0;
    float *tri = *data;
    for(size_t i = 0; i < attrib.num_faces; ++i) {
        tinyobj_vertex_index_t id = attrib.faces[i];
        tri[0] = attrib.vertices[3 * (size_t)id.v_idx + 0];
        tri[1] = attrib.vertices[3 * (size_t)id.v_idx + 1];
        tri[2] = attrib.vertices[3 * (size_t)id.v_idx + 2];

        tri[3] = attrib.normals[3 * (size_t)id.vn_idx + 0];
        tri[4] = attrib.normals[3 * (size_t)id.vn_idx + 1];
        tri[5] = attrib.normals[3 * (size_t)id.vn_idx + 2];

        tri[6] = attrib.texcoords[2 * (size_t)id.vt_idx + 0];
        tri[7] = attrib.texcoords[2 * (size_t)id.vt_idx + 1];

        tri += 8;
        //face_offset += (size_t)attrib.face_num_verts[i];
    }
}

void upload_model(struct Model *model)
{
    glGenVertexArrays(1, &model->vao);
    glGenBuffers(1, &model->vbo);

    glBindVertexArray(model->vao);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, model->vert_count * (8 * sizeof(float)), model->data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    //glEnableVertexAttribArray(2);
}

void print_model(struct Model *model)
{
    for(size_t i = 0; i < model->vert_count; ++i) {
        printf("[%5d] v %-5f %-5f %-5f \t vn %-5f %-5f %-5f \t vt %-5f %-5f\n", i,
               model->data[0 + (i * 8)], model->data[1 + (i * 8)], model->data[2 + (i * 8)],
               model->data[3 + (i * 8)], model->data[4 + (i * 8)], model->data[5 + (i * 8)],
               model->data[6 + (i * 8)], model->data[7 + (i * 8)]);
    }
}

void draw_scene(ShaderID shader)
{
    mat4 proj;
    glm_perspective(glm_rad(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, proj);

    mat4 view;
    glm_mat4_identity(view);
    glm_translate(view, (vec3){0.0f, 0.0f, -4.0f});

    // spinning cube
    glUseProgram(shader);
    glBindVertexArray(cube_info.vao);

    int model_loc = glGetUniformLocation(shader, "model");
    int view_loc = glGetUniformLocation(shader, "view");
    int proj_loc = glGetUniformLocation(shader, "proj");
    int light_dir_loc = glGetUniformLocation(shader, "light_dir");
    int color_loc = glGetUniformLocation(shader, "color");
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

void draw_fullscreen(void)
{
    ShaderID shader = debug_shader;

    mat4 quad_model;
    glm_mat4_identity(quad_model);
    glm_scale(quad_model, (vec4){2, 2, 2, 2});

    glUseProgram(shader);
    glBindVertexArray(cube_info.vao);
    int model_loc = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &quad_model[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

int main(void)
{
    agl_window_init("AkariGL Testing", WIDTH, HEIGHT);
    agl_renderer_init(WIDTH, HEIGHT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //quad_id = agl_upload_new_mesh(&quad_data);
    debug_shader = agl_load_compile_shader("res/shaders/debug.vert", NULL, "res/shaders/debug.frag");
    flat_shader = agl_load_compile_shader("res/shaders/flat.vert", NULL, "res/shaders/flat.frag");
    shad_shader = agl_load_compile_shader("res/shaders/shadow.vert", "res/shaders/shadow.glsl", "res/shaders/shadow.frag");

    load_obj("res/meshes/cube.obj", &cube_info.data, &cube_info.vert_count);
    //cube_info.data = agl_filesys_obj_load("res/meshes/cube.obj", &cube_info.vert_count);
    assert(cube_info.data);
    upload_model(&cube_info);
    print_model(&cube_info);
    glm_mat4_identity(cube_info.model);

    glClearColor(0.2f, 0.3f, 0.3f, 0.0f);

    while(!agl_window_should_quit()) {
        // *** update ***
        glm_rotate_x(cube_info.model, glm_rad(1.0f), cube_info.model);
        glm_rotate_y(cube_info.model, glm_rad(0.5f), cube_info.model);
        glm_rotate_z(cube_info.model, glm_rad(0.2f), cube_info.model);

        light_dir[0] = sinf((float)counter++ * 0.01f) * 0.8f;
        light_dir[1] = 1;
        light_dir[2] = 1;
        glm_normalize(light_dir);

        // *** render ***
        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //draw_fullscreen();
        draw_scene(flat_shader);


        /*
        //glCullFace(GL_BACK);
        //draw_scene(shad_shader);

        //draw_scene(shad_shader);
        // depth prepass
        glDrawBuffer(GL_NONE);
        draw_scene(flat_shader);

        // shadow volumes
        glEnable(GL_STENCIL_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_DEPTH_CLAMP);
        glDisable(GL_CULL_FACE);

        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
        draw_scene(shad_shader);

        // lit render
        glDisable(GL_DEPTH_CLAMP);
        glEnable(GL_CULL_FACE);
        glDrawBuffer(GL_BACK);

        glStencilFunc(GL_EQUAL, 0x0, 0xFF);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
        //glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
        //draw_scene(flat_shader);
        draw_fullscreen();

        // shaded render
        glDisable(GL_STENCIL_TEST);
        */

        agl_window_swap_buffers();
    }

    agl_renderer_quit();
    agl_window_quit();
    
    free(cube_info.data);

    return 0;
}
