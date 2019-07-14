#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#ifdef __EMSCRIPTEN__
  #include <emscripten/emscripten.h>
#endif

// #include "test.c"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TARGET_TICK_INTERVAL = 1000 / 30; // want 30fps

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

bool initGame() {
  // init SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL error: %s\n", SDL_GetError());
    printf("Could not initialize SDL\n");
    return false;
  }

  // make window
  window = SDL_CreateWindow("SDL Tutorial",
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
  SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0xFF, 0xFF); // magenta

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

SDL_Texture* loadTex(char* path) {
  SDL_Surface* loadedSfc = IMG_Load(path);
  if (loadedSfc == NULL) {
    printf("SDL_image error: %s\n", IMG_GetError());
    printf("Could not load surface (%s)\n", path);
    return NULL;
  }
  if (SDL_SetColorKey(loadedSfc, SDL_TRUE, SDL_MapRGB(loadedSfc->format, 0xFF, 0, 0)) < 0) {
    printf("SDL error: %s\n", SDL_GetError());
    printf("Could not apply color key (%s)\n", path);
    return NULL;
  }
  SDL_Texture* loadedTex = SDL_CreateTextureFromSurface(renderer, loadedSfc);
  if (loadedTex == NULL) {
    printf("SDL error: %s\n", SDL_GetError());
    printf("Could not create texture (%s)\n", path);
    return NULL;
  }

  SDL_FreeSurface(loadedSfc);
  return loadedTex;
}

SDL_Texture* helloTex = NULL;
bool loadMedia() {
  // if (!(helloTex = loadTex("data/hello_world.bmp"))) return false;
  // SDL_DestroyTexture(helloTex);
  if (!(helloTex = loadTex("data/asteroid.png"))) return false;
  return true;
}

void closeGame() {
  SDL_DestroyRenderer(renderer);
  renderer = NULL;
  SDL_DestroyWindow(window);
  window = NULL;

  SDL_Quit();
}

Uint32 timeUntil(Uint32 target) {
  Uint32 now = SDL_GetTicks();
  if (target <= now) {
    // printf("idle time: 0\n");
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
    } else if (e.type == SDL_KEYDOWN) {
      if (e.key.keysym.sym == SDLK_UP) {
        printf("Pressed up\n");
      } else if (e.key.keysym.sym == SDLK_LEFT) {
        printf("Pressed left\n");
      } else if (e.key.keysym.sym == SDLK_RIGHT) {
        printf("Pressed right\n");
      } else if (e.key.keysym.sym == SDLK_DOWN) {
        printf("Pressed down\n");
      }
    }
  }

  // all events for this pseudo-frame have now been processed

  SDL_Rect stretchRect = { .x = 0, .y = 0, .w = SCREEN_WIDTH/2, .h = SCREEN_HEIGHT/2 };

  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, helloTex, NULL, &stretchRect);
  SDL_RenderPresent(renderer);
}

#ifdef __EMSCRIPTEN__
  void runGame_fork() {
    // void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
    emscripten_set_main_loop(runForOneFrame, 0, 1);
  }
#else
  void runGame_fork() {
    Uint32 frameStartTime;
    while (1) {
      frameStartTime = SDL_GetTicks();
      runForOneFrame();
      if (_quitGame) break;
      // Delay to keep frame rate constant (using SDL)
      SDL_Delay(timeUntil(frameStartTime + TARGET_TICK_INTERVAL));
    }
  }
#endif

void runGame() {
  runGame_fork();
}

int main(int argc, char** argv) {
  if (!initGame()) {
    printf("Failed to init\n");
    return 1;
  }

  if (!loadMedia()) {
    printf("Failed to load media\n");
    return 1;
  }

  runGame();

  closeGame();
}
