# WebAssembly Hello World

## Local (C) Dev instructions

```
./build compile && ./out/a  # compile and run
./build  # shortcut for the above command
```

## Emscripten Dev instructions

I followed https://webassembly.org/getting-started/developers-guide/

tl;dr:

### Setup (once per day)

1. (in a separate shell) Run webserver: `./server.sh`
2. Get cmdline aliases:
```
source ~/projects/emsdk/emsdk_env.sh
```

### Dev loop (once per change)

1. Compile the code: `./build emscripten`
2. Refresh:
http://localhost:8080/



## Todo

* Read ECS articles
  https://www.richardlord.net/blog/ecs/
* Read skipped tutorials
  * 16 (fonts)
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

`emcc --help | bat` has some interesting profiling option (search "profil")
