#ifndef RENDERER_DEBUG_H
#define RENDERER_DEBUG_H

#include "agl_renderer.h"
#include <stdio.h>

#ifdef DEBUG
    #define check_gl_error()    _check_gl_error(__FILE__, __LINE__)
#else
    #define check_gl_error() /* disabled */
#endif


static int _check_gl_error(const char *file, int line)
{
    GLenum err = 0;
    while((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "%s:%d -> OpenGL Error: %04x\n", file, line, err);
    }

    return err == 0;
}

#endif /*RENDERER_DEBUG_H*/
