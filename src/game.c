typedef struct {
  float x;
  float y;
  float dx;
  float dy;
  float t; // angle
  float dt;
} Asteroid;

Asteroid asteroids[128] = {};
int asteroid_count = 0;
void AsteroidCreate() {
  Asteroid *out = &asteroids[asteroid_count++];
  out->x = randInt(SCREEN_WIDTH);
  out->y = randInt(SCREEN_HEIGHT);
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
  float t; // angle
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

void PlayerInput() {
  const Uint8* state = SDL_GetKeyboardState(NULL);
  if (state[SDL_SCANCODE_UP]) {
    double pi = acos(-1);
    player.dx += 0.4*cos(player.t * (pi/180));
    player.dy += 0.4*sin(player.t * (pi/180));
  } else if (state[SDL_SCANCODE_DOWN]) {
    double pi = acos(-1);
    player.dx -= 0.4*cos(player.t * (pi/180));
    player.dy -= 0.4*sin(player.t * (pi/180));
  } else if (state[SDL_SCANCODE_RIGHT]) {
    player.t += 5;
  } else if (state[SDL_SCANCODE_LEFT]) {
    player.t -= 5;
  }
}

void PlayerUpdate() {
  PlayerInput();
  player.x = saneMod(player.x + player.dx, SCREEN_WIDTH);
  player.y = saneMod(player.y + player.dy, SCREEN_HEIGHT);
}

void PlayerRender() {
  TextureRenderEx(shipTex, player.x, player.y, NULL, player.t, NULL, SDL_FLIP_NONE);
}


void InitGame() {
  for (int i = 0; i < 10; ++i) {
    AsteroidCreate();
  }
  PlayerCreate();
}
