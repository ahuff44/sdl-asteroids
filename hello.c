#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TARGET_TICK_INTERVAL = 1000 / 30;

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

bool initGame() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL_Error: %s\n", SDL_GetError());
    printf("Could not initialize SDL\n");
    return false;
  }

  window = SDL_CreateWindow("SDL Tutorial",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
    SDL_WINDOW_SHOWN);
  if (window == NULL) {
    printf("SDL_Error: %s\n", SDL_GetError());
    printf("Could not create window\n");
    return false;
  }

  screenSurface = SDL_GetWindowSurface(window);
  return true;
}

SDL_Surface* loadSurface(char* path) {
  SDL_Surface* loadedSfc = SDL_LoadBMP(path);
  if (loadedSfc == NULL) {
    printf("SDL_Error: %s\n", SDL_GetError());
    printf("Could not load surface %s\n", path);
    return NULL;
  }
  SDL_Surface* optimizedSfc = SDL_ConvertSurface(loadedSfc, screenSurface->format, 0);
  if (optimizedSfc == NULL) {
    printf("SDL_Error: %s\n", SDL_GetError());
    printf("Could not optimize surface %s\n", path);
    printf("Recovering...\n");
    return loadedSfc;
  }

  SDL_FreeSurface(loadedSfc);
  return optimizedSfc;
}

SDL_Surface* helloSurface = NULL;
bool loadMedia() {
  if (!(helloSurface = loadSurface("data/hello_world.bmp"))) return false;
  return true;
}

void closeGame() {
  SDL_DestroyWindow(window);
  window = NULL;

  SDL_Quit();
}

Uint32 timeUntil(Uint32 target) {
  Uint32 now = SDL_GetTicks();
  if (target <= now) {
    printf("idle time: 0\n");
    return 0;
  } else {
    printf("idle time: %d\n", target - now);
    return target - now;
  }
}

bool _quitGame = false;
void setQuitGame() {
#ifdef __EMSCRIPTEN__
  emscripten_cancel_main_loop();
#else
  _quitGame = true;
#endif
}

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

  // SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0x00, 0xFF));

  SDL_Rect stretchRect;
  stretchRect.x = 0;
  stretchRect.y = 0;
  stretchRect.w = SCREEN_WIDTH/2;
  stretchRect.h = SCREEN_HEIGHT/2;
  SDL_BlitScaled(helloSurface, NULL, screenSurface, &stretchRect);

  SDL_UpdateWindowSurface(window);
}

void runGame() {
#ifdef __EMSCRIPTEN__
  // void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
  emscripten_set_main_loop(runForOneFrame, 0, 1);
#else
  Uint32 frameStartTime;
  while (1) {
    frameStartTime = SDL_GetTicks();
    runForOneFrame();
    if (_quitGame) break;
    // Delay to keep frame rate constant (using SDL)
    SDL_Delay(timeUntil(frameStartTime + TARGET_TICK_INTERVAL));
  }
#endif
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
