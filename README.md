I followed https://webassembly.org/getting-started/developers-guide/

tl;dr:

Setup:
1. `source ./emsdk_env.sh`
2. Run webserver `emrun --no_browser --port 8080 .`

Dev loop:
1. edit hello.c
2. `emcc hello.c -s WASM=1 -o hello.html`
3. Refresh http://localhost:8080/hello.html
