// pass this around by value; it's small
typedef struct {
  SDL_Texture* tex;
  int w;
  int h;
} Texture;

// Texture* _TextureAlloc() {
//   size_t size = sizeof(Texture);
//   void* ptr = malloc(size);
//   return ptr;
// }

const SDL_Color ColorKey = { .r=0xFF, .g=0, .b=0 }; // red
// const SDL_Color ColorKey = { .r=0, .g=0xFF, .b=0xFF }; // cyan

bool TextureLoad(char* path, Texture *out) {
  SDL_Surface* surface = IMG_Load(path);
  if (surface == NULL) {
    printf("SDL_image error: %s\n", IMG_GetError());
    printf("Could not load surface\n");
    return false;
  }
  if (SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, ColorKey.r, ColorKey.g, ColorKey.b)) < 0) {
    printf("SDL error: %s\n", SDL_GetError());
    printf("Could not apply color key\n");
    return false;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == NULL) {
    printf("SDL error: %s\n", SDL_GetError());
    printf("Could not create texture\n");
    return false;
  }

  *out = (Texture){ .tex=texture, .w=surface->w, .h=surface->h };
  SDL_FreeSurface(surface);
  return true;
}

void TextureFree(Texture tex) {
  SDL_DestroyTexture(tex.tex);
}

void TextureRender(Texture tex, int x, int y) {
  SDL_RenderCopy(renderer, tex.tex, NULL, &(SDL_Rect){ .x=x, .y=y, .w=tex.w, .h=tex.h });
}

Texture helloTex;
bool LoadMedia() {
  bool success = true;
  if (!TextureLoad("data/asteroid.png", &helloTex)) {
    printf("Could not load image %s\n", "data/asteroid.png");
    success = false;
  }
  return success;
}
