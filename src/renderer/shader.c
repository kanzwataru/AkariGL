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

ShaderID agl_load_compile_shader(const char *vert_path, const char *frag_path, const char *geom_path)
{
    char *vert_src = agl_filesys_load(vert_path, NULL);
    assert(vert_src);

    char *frag_src = agl_filesys_load(frag_path, NULL);
    assert(frag_src);

    char *geom_src = agl_filesys_load(geom_path, NULL); // can be null

    ShaderID shader = agl_compile_shader(vert_src, frag_src, geom_src);
    free(vert_src);
    free(frag_src);
    if(geom_src)
        free(geom_src);

    return shader;
}

ShaderID agl_compile_shader(const char *vert_src, const char *frag_src, const char *geom_src)
{
    GLuint vert, frag, geom, shader;

    vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vert_src, NULL);
    glCompileShader(vert);
    check_shader_compilation(vert);

    frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &frag_src, NULL);
    glCompileShader(frag);
    check_shader_compilation(frag);

    if(geom_src) {
        geom = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geom, 1, &geom_src, NULL);
        glCompileShader(geom);
        check_shader_compilation(geom);
    }

    shader = glCreateProgram();
    glAttachShader(shader, vert);
    glAttachShader(shader, frag);
    if(geom_src) {
        glAttachShader(shader, geom);
    }

    glLinkProgram(shader);

    check_program_compilation(shader);

    glDeleteShader(vert);
    glDeleteShader(frag);
    if(geom_src) {
        glDeleteShader(geom);
    }

    check_gl_error();

    return shader;
}
