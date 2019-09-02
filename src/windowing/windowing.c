#include "agl_windowing.h"
#include <glad/glad.h>
#include <SDL2/SDL_opengl.h>

static SDL_Window    *window;
static SDL_GLContext gl_context;

static int panic_sdl(const char *err)
{
    fprintf(stderr, "%s: %s\n", err, SDL_GetError());
    exit(-1);
}

int agl_window_should_quit(void)
{
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_QUIT:
            return 1;
        }
    }

    return 0;
}

void agl_window_init(const char *title, int width, int height)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        panic_sdl("Could not initialize SDL2");
    }

    SDL_GL_LoadLibrary(NULL);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

    window = SDL_CreateWindow(title,
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              width, height,
                              SDL_WINDOW_OPENGL);

    if(!window)
      panic_sdl("Could not create window:");

    gl_context = SDL_GL_CreateContext(window);
    if(!gl_context)
      panic_sdl("Could not create OpenGL context");

    gladLoadGLLoader(SDL_GL_GetProcAddress);
    SDL_GL_SetSwapInterval(1); /* use v-sync */
}

void agl_window_swap_buffers(void)
{
    SDL_GL_SwapWindow(window);
}

void agl_window_quit(void)
{
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
