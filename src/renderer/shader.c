#include "agl_renderer.h"
#include "agl_platform.h"
#include "renderer/debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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

ShaderID agl_load_compile_shader(const char *vert_path, const char *frag_path)
{
    char *vert_src = agl_filesys_load(vert_path, NULL);
    assert(vert_src);

    char *frag_src = agl_filesys_load(frag_path, NULL);
    assert(frag_src);

    ShaderID shader = agl_compile_shader(vert_src, frag_src);
    free(vert_src);
    free(frag_src);

    return shader;
}

ShaderID agl_compile_shader(const char *vert_src, const char *frag_src)
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

    check_gl_error();

    return shader;
}
