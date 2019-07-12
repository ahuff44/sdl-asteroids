#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

bool init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL failed to initialize: SDL_Error %s\n", SDL_GetError());
    return false;
  }

  window = SDL_CreateWindow("SDL Tutorial",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
    SDL_WINDOW_SHOWN);
  if (window == NULL) {
    printf("Could not create window: SDL_Error %s\n", SDL_GetError());
    return false;
  }

  screenSurface = SDL_GetWindowSurface(window);
  return true;
}

SDL_Surface* loadSurface(char* path) {
  SDL_Surface* out;
  out = SDL_LoadBMP(path);
  if (out == NULL) {
    printf("Could not load image %s: SDL_Error: %s\n", path, SDL_GetError());
    return NULL;
  }
  return out;
}

SDL_Surface* helloSurface = NULL;
bool loadMedia() {
  helloSurface = loadSurface("hello_world.bmp");
  if (helloSurface == NULL) {
    printf("Could not load media");
    return false;
  }
  return true;
}

void close() {
  SDL_DestroyWindow(window);
  window = NULL;

  SDL_Quit();
}

void game() {
  bool quit = false;
  SDL_Event e;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      printf("Event code: %d\n", e.type);
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    // all events for this pseudo-frame have now been processed
    SDL_BlitSurface(helloSurface, NULL, screenSurface, NULL);
    SDL_UpdateWindowSurface(window);
  }
}

// void game() {
//   SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0x00, 0xFF));
//   SDL_UpdateWindowSurface(window);

//   SDL_Delay(3000);
// }

int main(int argc, char** argv) {
  if (!init()) {
    printf("Failed to init\n");
    return 1;
  }

  if (!loadMedia()) {
    printf("Failed to load media\n");
    return 1;
  }

  game();

  close();
}
