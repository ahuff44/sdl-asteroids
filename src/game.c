typedef struct {
  int x;
  int y;
  int dx;
  int dy;
  int t; // angle
  int dt;
} Asteroid;


Asteroid asteroids[128] = {};
int asteroid_count = 0;
void AsteroidCreate(int x, int y) {
  Asteroid *out = &asteroids[asteroid_count++];
  out->x = x;
  out->y = y;
  out->dx = randIntIn(-5, 6);
  out->dy = randIntIn(-5, 6);
  out->t = randInt(360);
  out->dt = randIntIn(-5, 6);
}

void AsteroidUpdate() {
  for (int i = 0; i < asteroid_count; ++i) {
    Asteroid* a = &asteroids[i];
    a->x = saneMod(a->x + a->dx, SCREEN_WIDTH);
    a->y = saneMod(a->y + a->dy, SCREEN_HEIGHT);
    a->t = saneMod(a->t + a->dt, 360);
  }
}

void AsteroidRender() {
  for (int i = 0; i < asteroid_count; ++i) {
    Asteroid a = asteroids[i];
    TextureRenderEx(asteroidTex, a.x, a.y, NULL, a.t, NULL, SDL_FLIP_NONE);
  }
}
