#include "agl.h"
#define DEBUG
#include "renderer/debug.h"
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

#define WIDTH  1280
#define HEIGHT 720

struct Model {
    float *data;
    size_t vert_count;
    GLuint vao;
    GLuint vbo;
    mat4 model;
};

static const float screen_quad_verts[] = {
    -1, -1, 0,    0, 0, 1,    0, 0,
     1, -1, 0,    0, 0, 1,    1, 0,
    -1,  1, 0,    0, 0, 1,    0, 1,
     1, -1, 0,    0, 0, 1,    1, 0,
     1,  1, 0,    0, 0, 1,    1, 1,
    -1,  1, 0,    0, 0, 1,    0, 1
};

static struct Model cube_info;
static struct Model suzanne_info;
static struct Model sphere_info;
static struct Model screen_quad = {
    screen_quad_verts,
    6, 0, 0, {{0}}
};

static ShaderID debug_shader;
static ShaderID flat_shader;
static ShaderID lit_shader;
static ShaderID shadvolume_shader;
static ShaderID post_direct_shader;
static ShaderID post_lines_laplacian_shader;
static vec3 light_dir = {0,0,0};
static vec3 ambient_col = {0.05f, 0.025f, 0.1f};
static unsigned int counter;

float *load_obj(const char *path, size_t *out_vert_count)
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

    float *data = calloc(attrib.num_faces, 8 * sizeof(float));
    assert(data);

    *out_vert_count = attrib.num_faces;

    float *tri = data;
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
    }

    return data;
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

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
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
    glm_perspective(glm_rad(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 10.0f, proj);

    mat4 view;
    glm_mat4_identity(view);
    glm_translate(view, (vec3){0.0f, 0.0f, -4.0f});
    //glm_scale(view, (vec4){100.0f, 100.0f, 100.0f, 1.0f});

    // spinning cube
    glUseProgram(shader);
    glBindVertexArray(cube_info.vao);

    int model_loc = glGetUniformLocation(shader, "model");
    int view_loc = glGetUniformLocation(shader, "view");
    int proj_loc = glGetUniformLocation(shader, "proj");
    int light_dir_loc = glGetUniformLocation(shader, "light_dir");
    int color_loc = glGetUniformLocation(shader, "color");
    int ambient_loc = glGetUniformLocation(shader, "ambient");
    int cel_split_loc = glGetUniformLocation(shader, "cel_split_point");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &cube_info.model[0][0]);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &proj[0][0]);
    glUniform3fv(light_dir_loc, 1, &light_dir[0]);
    glUniform3fv(ambient_loc, 1, &ambient_col[0]);
    glUniform3f(color_loc, 1.0f, 0.5f, 0.3f);
    glUniform1f(cel_split_loc, 0.0f);

    glDrawArrays(GL_TRIANGLES, 0, cube_info.vert_count);

    // small cube
    mat4 small_model;
    glm_mat4_identity(small_model);
    glm_translate_x(small_model, -0.5f);
    glm_translate_y(small_model, 1.5f);
    glm_scale(small_model, (vec4){0.5f, 0.5f, 0.5f, 1.0f});

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &small_model[0][0]);
    glUniform3f(color_loc, 0.5f, 0.35f, 0.45f);
    glDrawArrays(GL_TRIANGLES, 0, cube_info.vert_count);

    // ground plane
    //if(shader != shadvolume_shader) {
        mat4 ground_model;

        glm_mat4_identity(ground_model);
        glm_translate_y(ground_model, -1.5);
        ground_model[0][0] *= 100;
        ground_model[2][2] *= 100;

        glUniformMatrix4fv(model_loc, 1, GL_FALSE, &ground_model[0][0]);
        glUniform3f(color_loc, 0.9f, 0.7f, 0.8f);

        glDrawArrays(GL_TRIANGLES, 0, cube_info.vert_count);
    //}

    glUniform1f(cel_split_loc, 0.5f);

    // suzanne
    glBindVertexArray(suzanne_info.vao);

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &suzanne_info.model[0][0]);
    glUniform3f(color_loc, 0.9f, 0.2f, 0.1f);
    glDrawArrays(GL_TRIANGLES, 0, suzanne_info.vert_count);

    // sphere
    glBindVertexArray(sphere_info.vao);

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &sphere_info.model[0][0]);
    glUniform3f(color_loc, 0.1f, 0.2f, 0.9f);
    glDrawArrays(GL_TRIANGLES, 0, sphere_info.vert_count);
}

void draw_fullscreen(void)
{
    ShaderID shader = debug_shader;

    mat4 quad_model;
    glm_mat4_identity(quad_model);
    //glm_scale(quad_model, (vec4){2, 2, 2, 2});

    glUseProgram(shader);
    glBindVertexArray(screen_quad.vao);
    int model_loc = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &quad_model[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void draw_postprocess(ShaderID shader, GLuint col_buffer, GLuint depth_buffer)
{
    glUseProgram(shader);
    glBindVertexArray(screen_quad.vao);
    int col_buffer_loc = glGetUniformLocation(shader, "col_buffer");
    glUniform1i(col_buffer_loc, 0);
    int depth_buffer_loc = glGetUniformLocation(shader, "depth_buffer");
    glUniform1i(depth_buffer_loc, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, col_buffer);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depth_buffer);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

GLuint framebuffer_setup(GLuint *col_buffer, GLuint *depth_buffer)
{
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, col_buffer);
    glBindTexture(GL_TEXTURE_2D, *col_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *col_buffer, 0);

    glGenTextures(1, depth_buffer);
    glBindTexture(GL_TEXTURE_2D, *depth_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, *depth_buffer, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Regular Framebuffer is not complete\n");
    }

    return framebuffer;
}

GLuint framebuffer_multisample_setup(void)
{
    GLuint framebuffer, col_buffer, depth_buffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &col_buffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, col_buffer);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, WIDTH, HEIGHT, false);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, col_buffer, 0);

    glGenTextures(1, &depth_buffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_buffer);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT, false);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depth_buffer, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "Multisample Framebuffer is not complete\n");
    }

    return framebuffer;
}

void resolve_framebuffer(GLuint src, GLuint dest)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest);
    glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
}

int main(void)
{
    agl_window_init("AkariGL Testing", WIDTH, HEIGHT);
    agl_renderer_init(WIDTH, HEIGHT);

    //quad_id = agl_upload_new_mesh(&quad_data);
    debug_shader = agl_load_compile_shader("res/shaders/debug.vert", NULL, "res/shaders/debug.frag");
    flat_shader = agl_load_compile_shader("res/shaders/flat.vert", NULL, "res/shaders/flat.frag");
    lit_shader = agl_load_compile_shader("res/shaders/flat.vert", NULL, "res/shaders/lit.frag");
    shadvolume_shader = agl_load_compile_shader("res/shaders/shadow.vert", "res/shaders/shadow.glsl", "res/shaders/shadow.frag");
    post_direct_shader = agl_load_compile_shader("res/shaders/post.vert", NULL, "res/shaders/post_direct.frag");
    post_lines_laplacian_shader = agl_load_compile_shader("res/shaders/post.vert", NULL, "res/shaders/post_lines_laplacian.frag");

    cube_info.data = load_obj("res/meshes/cube.obj", &cube_info.vert_count);
    suzanne_info.data = load_obj("res/meshes/suzanne.obj", &suzanne_info.vert_count);
    sphere_info.data = load_obj("res/meshes/sphere.obj", &sphere_info.vert_count);
    upload_model(&cube_info);
    upload_model(&screen_quad);
    upload_model(&suzanne_info);
    upload_model(&sphere_info);
    //print_model(&cube_info);
    glm_mat4_identity(cube_info.model);
    glm_mat4_identity(suzanne_info.model);
    glm_mat4_identity(sphere_info.model);

    glm_mat4_identity(suzanne_info.model);
    glm_translate_x(suzanne_info.model, -1.8f);
    glm_translate_y(suzanne_info.model, -0.2f);
    glm_rotate_x(suzanne_info.model, glm_rad(-30), suzanne_info.model);
    glm_scale(suzanne_info.model, (vec4){0.5f, 0.5f, 0.5f, 1.0f});

    // set up framebuffer
    GLuint multisample_target, resolved_target;
    GLuint col_buffer, depth_buffer;
    multisample_target = framebuffer_multisample_setup();
    resolved_target = framebuffer_setup(&col_buffer, &depth_buffer);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(0.15f, 0.15f, 0.3f, 0.0f);

    while(!agl_window_should_quit()) {
        // *** update ***
        glm_rotate_x(cube_info.model, glm_rad(1.0f), cube_info.model);
        glm_rotate_y(cube_info.model, glm_rad(0.5f), cube_info.model);
        glm_rotate_z(cube_info.model, glm_rad(0.2f), cube_info.model);

        glm_rotate_y(suzanne_info.model, glm_rad(-0.2f), suzanne_info.model);

        glm_mat4_identity(sphere_info.model);
        glm_translate_y(sphere_info.model, sinf((float)counter++ * 0.01f) * 0.5f);
        glm_translate_x(sphere_info.model, 1.5f);
        glm_scale(sphere_info.model, (vec4){0.4f, 0.4f, 0.4f, 1.0f});

        light_dir[0] = sinf((float)counter++ * 0.01f) * 0.8f;
        light_dir[1] = 1;
        light_dir[2] = sinf((float)counter++ * 0.01f) * 0.8f;
        //light_dir[0] = 0;
        //light_dir[1] = 1;
        //light_dir[2] = 0;
        glm_normalize(light_dir);

        // *** render ***
        glBindFramebuffer(GL_FRAMEBUFFER, multisample_target);
        glDepthMask(GL_TRUE);
        glStencilMask(0xFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
#define DRAW_SHADOWED
#ifndef DRAW_SHADOWED
        //draw_fullscreen();
        //draw_scene(flat_shader);
        draw_scene(shadvolume_shader);

#else

        // depth prepass
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glColorMask(0, 0, 0, 0);
        glDepthFunc(GL_LESS);
        draw_scene(flat_shader);
        glDepthFunc(GL_LEQUAL);

        // shadow volumes
        glEnable(GL_STENCIL_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_DEPTH_CLAMP);
        glDisable(GL_CULL_FACE);

        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
        draw_scene(shadvolume_shader);

        // shaded render
        glDisable(GL_DEPTH_CLAMP);
        glEnable(GL_CULL_FACE);
        glColorMask(1, 1, 1, 1);
        //glDisable(GL_STENCIL_TEST);

        glStencilFunc(GL_NOTEQUAL, 0x0, 0xFF);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
        draw_scene(flat_shader);
        //draw_fullscreen();

        // lit render
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_EQUAL, 0x0, 0xFF);
        //glDepthFunc(GL_ALWAYS); draw_fullscreen();
        draw_scene(lit_shader);
        glDisable(GL_STENCIL_TEST);

        // postprocess
        resolve_framebuffer(multisample_target, resolved_target);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        draw_postprocess(post_lines_laplacian_shader, col_buffer, depth_buffer);
#endif
        agl_window_swap_buffers();
    }

    agl_renderer_quit();
    agl_window_quit();
    
    free(cube_info.data);

    return 0;
}
