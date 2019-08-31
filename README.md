# SDL / Emscripten sandbox

This is a toy project to learn about SDL / Emscripten / WebAssembly / ECS

Shoutouts to http://lazyfoo.net/tutorials/SDL/index.php, which has been a very
helpful guide while learning SDL.

## External Dependencies

* makeheaders
* clang
* emscripten
* python2.7

## Local (C) Dev instructions

```
./build compile && ./out/a  # compile and run
./build  # shortcut for the above command
```

## Emscripten Dev instructions

I followed https://webassembly.org/getting-started/developers-guide/

tl;dr:

### Setup (once per day)

1. (in a separate shell) Run webserver: `./server`
2. Get cmdline aliases:
```
source ~/projects/emsdk/emsdk_env.sh
```

### Dev loop (once per change)

1. Compile the code: `./build emscripten`
2. Refresh http://localhost:8080/



## Todo

* Read ECS articles
  https://www.richardlord.net/blog/ecs/
* Read skipped tutorials
  * 17 (mouse events)
  * 19, 20 (gamepads)
  * 21 (music, current)

## misc notes

From one the SDL.dmg README (iirc):
 - Screencast tutorials for getting started with OpenSceneGraph/Mac OS X are
  available at:
  http://www.openscenegraph.org/projects/osg/wiki/Support/Tutorials/MacOSXTips
  Though these are OpenSceneGraph centric, the same exact concepts apply to
  SDL, thus the videos are recommended for everybody getting started with
  developing on Mac OS X. (You can skim over the PlugIns stuff since SDL
  doesn't have any PlugIns to worry about.)


windows issues:
  main.c:
    -#include <SDL2/SDL.h>
    +#include <SDL.h>

  $ clang src/main.c -I/c/SDL2-2.0.9/i686-w64-mingw32/include/SDL2 -I/c/SDL2_image-2.0.5/i686-w64-mingw32/include/SDL2 -L/c/SDL2-2.0.9/i686-w64-mingw32/lib -L/c/SDL2_image-2.0.5/i686-w64-mingw32/lib -lSDL2main -lSDL2 -lSDL2_image -o a.out
  ...
  81 warnings generated.
  LINK : fatal error LNK1181: cannot open input file 'SDL2main.lib'

  i dunno i give up. maybe i need mingw?

  to try:
    I saw this in a lazyfoo tutorial; maybe I need SDL2main?
    ```
    This project is linked against:
    ----------------------------------------
    Windows:
    SDL2
    SDL2main
    SDL2_image
    SDL2_ttf

    *nix:
    SDL2
    SDL2_image
    SDL2_ttf
    ```