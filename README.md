# WebAssembly Hello World

## Dev instructions

I followed https://webassembly.org/getting-started/developers-guide/

tl;dr:

Setup:
1. `source ./emsdk_env.sh`
2. Run webserver `emrun --no_browser --port 8080 .`

Dev loop:
1. edit hello.c
2. `emcc hello.c -o hello.html -s WASM=1 -s EXIT_RUNTIME=1 -s USE_SDL=2`
3. Refresh http://localhost:8080/hello.html

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



