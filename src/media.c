typedef struct {
  SDL_Texture tex;
  int w;
  int h;
} Texture;

Texture* _TextureAlloc() {
  size_t size = sizeof(Texture);
  void* ptr = malloc(size);
  return ptr;
}

const SDL_Color ColorKey = { .r=0xFF, .g=0, .b=0 };

Texture* TextureLoad(char* path) {
  SDL_Surface* loadedSfc = IMG_Load(path);
  if (loadedSfc == NULL) {
    printf("SDL_image error: %s\n", IMG_GetError());
    printf("Could not load surface\n", path);
    return NULL;
  }
  if (SDL_SetColorKey(loadedSfc, SDL_TRUE, SDL_MapRGB(loadedSfc->format, ColorKey.r, ColorKey.g, ColorKey.b)) < 0) {
    printf("SDL error: %s\n", SDL_GetError());
    printf("Could not apply color key\n", path);
    return NULL;
  }
  SDL_Texture* loadedTex = SDL_CreateTextureFromSurface(renderer, loadedSfc);
  if (loadedTex == NULL) {
    printf("SDL error: %s\n", SDL_GetError());
    printf("Could not create texture\n", path);
    return NULL;
  }

  Texture* tex = _TextureAlloc();
  *tex = { .tex=loadedTex, .w=loadedSfc.width, .h=loadedSfc.height };

  SDL_FreeSurface(loadedSfc);
  return tex;
}

SDL_Texture* helloTex = NULL;
bool LoadMedia() {
  bool success = true;
  if (!(helloTex = TextureLoad("data/asteroid.png"))) {
    printf("Could not load image %s\n", "data/asteroid.png");
    success = false;
  }
  return success;
}