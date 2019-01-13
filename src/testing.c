#include <stdio.h>
#include <SDL2/SDL.h>

static SDL_Window   *window;
static SDL_Renderer *renderer;

static SDL_Rect blah = {32, 64, 128, 256};

static int panic_sdl(const char *err) {
    fprintf(stderr, "%s: %s\n", err, SDL_GetError());
    exit(-1);
}

int main(void)
{
    int is_running = 1;
    SDL_Event event;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        panic_sdl("Could not initialize SDL2");
    }
    
    window = SDL_CreateWindow("AkariGL Test", 
                              SDL_WINDOWPOS_UNDEFINED, 
                              SDL_WINDOWPOS_UNDEFINED,
                              640, 480,
                              SDL_WINDOW_SHOWN);
    
    if(!window) {
        panic_sdl("Could not create window:");
    }

    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer) {
        panic_sdl(stderr, "Could not create renderer:");
    }
    
    while(is_running) {
        SDL_SetRenderDrawColor(renderer, 50, 15, 45, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 20, 50, 60, 255);
        SDL_RenderFillRect(renderer, &blah);
        SDL_RenderPresent(renderer);

        while(SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_QUIT:
                is_running = 0;
            }
        }
    }
    
    SDL_DestroyWindow(window);
    
    return 1;
}
