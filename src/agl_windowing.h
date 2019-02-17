#ifndef AKARI_WINDOWING_H
#define AKARI_WINDOWING_H
#include <SDL2/SDL.h>

void agl_window_init(const char *title, int width, int height);
int  agl_window_should_quit(void);
void agl_window_swap_buffers(void);
void agl_window_quit(void);

#endif /*AKARI_WINDOWING_H*/
