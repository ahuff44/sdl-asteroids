#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#ifdef __EMSCRIPTEN__
  #include <emscripten/emscripten.h>
#endif

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TARGET_FPS = 30;
const int TARGET_TICK_INTERVAL = 1000 / TARGET_FPS;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

#include "util.c"
#include "buf.c"
#include "map.c"
#include "media.c"
#include "game.c"

bool initSDL() {
  // init SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL error: %s\n", SDL_GetError());
    printf("Could not initialize SDL\n");
    return false;
  }

  // make window
  window = SDL_CreateWindow("Asteroids",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
    SDL_WINDOW_SHOWN);
  if (window == NULL) {
    printf("SDL error: %s\n", SDL_GetError());
    printf("Could not create window\n");
    return false;
  }
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("SDL error: %s\n", SDL_GetError());
    printf("Could not create renderer\n");
    return false;
  }

  #ifdef __EMSCRIPTEN__
    // skip IMG_Init; see https://github.com/emscripten-ports/SDL2_image/issues/3
  #else
    // init png plugin
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
      printf("SDL_image error: %s\n", IMG_GetError());
      printf("Could not initialize SDL_image\n");
    }
  #endif

  return true;
}

void closeGame() {
  FreeMedia();

  SDL_DestroyRenderer(renderer);
  renderer = NULL;
  SDL_DestroyWindow(window);
  window = NULL;

  SDL_Quit();
}

Uint32 timeUntil(Uint32 target) {
  Uint32 now = SDL_GetTicks();
  if (target <= now) {
    printf("frame took %dms too long\n", now - target);
    return 0;
  } else {
    // printf("idle time: %d\n", target - now);
    return target - now;
  }
}

bool _quitGame = false;
#ifdef __EMSCRIPTEN__
  void setQuitGame() {
    emscripten_cancel_main_loop();
  }
#else
  void setQuitGame() {
    _quitGame = true;
  }
#endif

void runForOneFrame() {
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    // printf("Event code: %d\n", e.type);
    if (e.type == SDL_QUIT) {
      setQuitGame();
      return;
    } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q) {
      setQuitGame();
      return;
    } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r) {
      InitGame();
    }
  }

  PlayerUpdate();
  BulletUpdate();
  AsteroidUpdate();
  HandleCollisions();

  // all events for this frame have now been processed

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
  SDL_RenderClear(renderer);
  PlayerRender();
  BulletRender();
  AsteroidRender();
  SDL_RenderPresent(renderer);
}

#ifdef __EMSCRIPTEN__
  void runGame_fork() {
    emscripten_set_main_loop(runForOneFrame, TARGET_FPS, 1);
  }
#else
  void runGame_fork() {
    Uint32 frameStartTime;
    while (1) {
      frameStartTime = SDL_GetTicks();
      runForOneFrame();
      if (_quitGame) break;
      // Delay to keep frame rate constant
      SDL_Delay(timeUntil(frameStartTime + TARGET_TICK_INTERVAL));
    }
  }
#endif

void runGame() {
  InitRNG();
  InitGame();
  runGame_fork();
}

int main(int argc, char** argv) {
  if (!initSDL()) {
    printf("Failed to init\n");
    return 1;
  }

  if (!LoadMedia()) {
    printf("Failed to load media\n");
    return 1;
  }

  runGame();

  closeGame();
}
