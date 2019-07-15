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


typedef struct {
  float x;
  float y;
  float dx;
  float dy;
  int t; // angle
} Ship;

Ship player;
void ShipCreate(int x, int y, Ship *out) {
  out->x = x;
  out->y = y;
  out->dx = 0;
  out->dy = 0;
  out->t = 0;
}

void PlayerCreate() {
  ShipCreate(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, &player);
}

void PlayerUpdate() {
  player.x = saneMod(player.x + player.dx, SCREEN_WIDTH);
  player.y = saneMod(player.y + player.dy, SCREEN_HEIGHT);
}

void PlayerInput(SDL_Keycode code) {
  double pi = acos(-1);
  if (code == SDLK_UP) {
    player.dx += 2*cos(player.t * (pi/180));
    player.dy += 2*sin(player.t * (pi/180));
  } else if (code == SDLK_DOWN) {
    player.dx -= 2*cos(player.t * (pi/180));
    player.dy -= 2*sin(player.t * (pi/180));
  } else if (code == SDLK_RIGHT) {
    player.t += 5;
  } else if (code == SDLK_LEFT) {
    player.t -= 5;
  }
}

void PlayerRender() {
  TextureRenderEx(shipTex, player.x, player.y, NULL, player.t, NULL, SDL_FLIP_NONE);
}
