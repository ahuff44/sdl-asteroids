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
2. Compile the code: `./build emscripten`
3. Refresh:
http://localhost:8080/out/hello.html

## Local (C) Dev instructions

```
./build  # this compiles and runs
./build compile  # this only compiles
```



## Todo

* Set up proper makefile; shouldn't be too hard
* Do GPU sections of tutorial: (skipped)
  http://lazyfoo.net/tutorials/SDL/07_texture_loading_and_rendering/index.php
  http://lazyfoo.net/tutorials/SDL/08_geometry_rendering/index.php
  http://lazyfoo.net/tutorials/SDL/09_the_viewport/index.php
* read ECS articles
  https://www.richardlord.net/blog/ecs/

## misc notes

From one the SDL.dmg README (iirc):
 - Screencast tutorials for getting started with OpenSceneGraph/Mac OS X are
  available at:
  http://www.openscenegraph.org/projects/osg/wiki/Support/Tutorials/MacOSXTips
  Though these are OpenSceneGraph centric, the same exact concepts apply to
  SDL, thus the videos are recommended for everybody getting started with
  developing on Mac OS X. (You can skim over the PlugIns stuff since SDL
  doesn't have any PlugIns to worry about.)

`emcc --help | bat` has some interesting profiling option (search "profil")
