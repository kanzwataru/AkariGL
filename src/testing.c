#include <stdio.h>
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define WIDTH  800
#define HEIGHT 600

typedef struct {
    GLuint  id;     /* vbo */
    GLuint  config; /* vao */
    float  *vertices;
    size_t  vert_num;
} Mesh;

static const char *vertex_shader = 
"#version 150 core\n"
"in vec2 position;\n"
"void main() {\n"
"   gl_Position = vec4(position, 0.0, 1.0);\n"
"}";

static const char *fragment_shader =
"#version 150 core\n"
"out vec4 out_color;\n"
"void main() {\n"
"   out_color = vec4(1.0, 0.5, 0.3, 1.0);\n"
"}";

static SDL_Window    *window;
static SDL_Renderer  *renderer;
static SDL_GLContext gl_context;

static SDL_Rect blah = {32, 64, 128, 256};

static float tri_verts[] = {
    0.0f,  0.5f,
    0.5f, -0.5f,
   -0.5f, -0.5f
};
static Mesh tri = {0, 0, tri_verts, sizeof(tri_verts) / sizeof(float)};
static GLuint flat_shader;

static int panic_sdl(const char *err) 
{
    fprintf(stderr, "%s: %s\n", err, SDL_GetError());
    exit(-1);
}

static GLuint create_mesh_configuration(void) /* this could have different configuration types */
{
    GLuint vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    return vao;
}

static void upload_mesh(Mesh *mesh) 
{
    glGenBuffers(1, &mesh->id);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->vert_num, mesh->vertices, GL_STATIC_DRAW);
}

static void cleanup_mesh(Mesh *mesh)
{
    glDeleteVertexArrays(1, &mesh->config);
    glDeleteBuffers(1, &mesh->id);
}

static void check_shader_compilation(GLuint shader)
{
    GLint status;
    char  msg_buf[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if(status != GL_TRUE) {
        glGetShaderInfoLog(shader, 512, NULL, msg_buf);
        fprintf(stderr, "*** SHADER COMPILATION FAILED ***\n, %s\n\n", msg_buf);
    }
}

static void check_program_compilation(GLuint program)
{
    GLint status;
    char  msg_buf[512];

    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if(status != GL_TRUE) {
        glGetProgramInfoLog(program, 512, NULL, msg_buf);
        fprintf(stderr, "*** PROGRAM COMPILATION FAILED ***\n, %s\n\n", msg_buf);
    }
}

static GLuint compile_shader(const char *vert_src, const char *frag_src)
{
    GLuint vert, frag, shader;

    vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vert_src, NULL);
    glCompileShader(vert);
    check_shader_compilation(vert);

    frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &frag_src, NULL);
    glCompileShader(frag);
    check_shader_compilation(frag);

    shader = glCreateProgram();
    glAttachShader(shader, vert);
    glAttachShader(shader, frag);
    glLinkProgram(shader);

    check_program_compilation(shader);

    glDeleteShader(vert);
    glDeleteShader(frag);
}

static void render_mesh(Mesh *mesh, GLuint shader)
{
    glBindVertexArray(mesh->config);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->id);
    glUseProgram(shader);
    glDrawArrays(GL_TRIANGLES, 0, mesh->vert_num);
    printf("%d\n\n", glGetError());
}

static void init_scene(void)
{
    flat_shader = compile_shader(vertex_shader, fragment_shader);

    tri.config = create_mesh_configuration();
    upload_mesh(&tri);
}

static void cleanup(void)
{
    glDeleteProgram(flat_shader);

    cleanup_mesh(&tri);
}

int main(void)
{
    int is_running = 1;
    SDL_Event event;
    
    setbuf(stdout, 0);

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        panic_sdl("Could not initialize SDL2");

    SDL_GL_LoadLibrary(NULL);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    window = SDL_CreateWindow("AkariGL Test", 
                              SDL_WINDOWPOS_UNDEFINED, 
                              SDL_WINDOWPOS_UNDEFINED,
                              WIDTH, HEIGHT,
                              SDL_WINDOW_OPENGL);
    
    if(!window)
        panic_sdl("Could not create window:");
    
    gl_context = SDL_GL_CreateContext(window);
    if(!gl_context)
        panic_sdl("Could not create OpenGL context");

    gladLoadGLLoader(SDL_GL_GetProcAddress);
    SDL_GL_SetSwapInterval(1); /* use v-sync */

    glViewport(0,0, WIDTH, HEIGHT);
    glClearColor(0.2f, 0.3f, 0.3f, 0.0f);

    init_scene();

    while(is_running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_QUIT:
                is_running = 0;
            }
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        render_mesh(&tri, flat_shader);

        SDL_GL_SwapWindow(window);
    }

    cleanup();

    SDL_GL_DeleteContext(window);
    SDL_DestroyWindow(window);
    
    return 1;
}
