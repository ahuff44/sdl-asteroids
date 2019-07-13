# WebAssembly Hello World

## Emscripten Dev instructions

I followed https://webassembly.org/getting-started/developers-guide/

tl;dr:

### Setup (once per day)

1. Get cmdline aliases:
```
source ~/projects/emsdk/emsdk_env.sh
```
2. Run webserver:
```
emrun --no_browser --port 8080 .
```

### Dev loop (once per change)

1. Edit hello.c
2. Compile the code:
```
emcc hello.c -o hello.html --preload-file data --use-preload-plugins -s WASM=1 -s EXIT_RUNTIME=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1
```
3. Refresh:
http://localhost:8080/hello.html

## Local (C) Dev instructions

```
gcc hello.c -I ~/Library/Frameworks/SDL2.framework/Headers -I ~/Library/Frameworks/SDL2_image.framework/Headers -F ~/Library/Frameworks -framework SDL2 -framework SDL2_Image && ./a.out
```



## Todo

1. Get basic sdl2 app up (draw a square onscreen?), working on mac only
2. Then, switch out compiler for webasm and hope that wokrs

## misc notes

 - Screencast tutorials for getting started with OpenSceneGraph/Mac OS X are
  available at:
  http://www.openscenegraph.org/projects/osg/wiki/Support/Tutorials/MacOSXTips
  Though these are OpenSceneGraph centric, the same exact concepts apply to
  SDL, thus the videos are recommended for everybody getting started with
  developing on Mac OS X. (You can skim over the PlugIns stuff since SDL
  doesn't have any PlugIns to worry about.)


works to compile sdl (no webasm yet)
  gcc hello.c -o a.out -I ~/Library/Frameworks/SDL2.framework/Headers -F ~/Library/Frameworks -framework SDL2

`emcc --help | bat` has some interesting profiling option (search "profil")
