/*
modified from https://stackoverflow.com/questions/29335510/using-sdl2-renderdraw-functions-with-emscripten#29339618
working example:

This works locally:
  gcc test.cpp -o a.out -I ~/Library/Frameworks/SDL2.framework/Headers -F ~/Library/Frameworks -framework SDL2 && ./a.out

And this works in-browser:
  emcc test.cpp -o test.html -s USE_SDL=2 -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1
  (then go to localhost:8080/test.html)

update: the DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR is unnecessary
update:
  this command (very very closely mirrors hello.c) works
    emcc test.cpp -o test.html --preload-file data --use-preload-plugins -s WASM=1 -s EXIT_RUNTIME=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1
  so the problem must be in the code/asset bundling?

*/

#include <stdio.h>
#include <SDL2/SDL.h>
#include <assert.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

SDL_Window * window;
SDL_Renderer * renderer;

const int LENGTH = 250;

void render_func(void) {
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    SDL_RenderClear(renderer);

    static int rr;
    rr = (rr + 1) % 255;
    SDL_SetRenderDrawColor(renderer, 0, rr, (128+rr*2)%255, 255-rr);
    SDL_RenderDrawLine(renderer, 0, 0, 640, 320);
    SDL_RenderPresent(renderer);

#ifdef __EMSCRIPTEN__
    static int i;
    i += 1;
    if (i >= LENGTH) {
      emscripten_cancel_main_loop();
      printf("emscripten done\n");
    }
#endif
}

int main(int argc, char* argv[])
{
    assert(SDL_Init(SDL_INIT_VIDEO) == 0);
    SDL_CreateWindowAndRenderer(640, 320, 0, &window, &renderer);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(render_func, 0, 1);
#else
    int i = 0;
    while (i < LENGTH) {
      i += 1;
      render_func();
      SDL_Delay(10);
    }
#endif
    printf("no-emscripten done\n");

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
