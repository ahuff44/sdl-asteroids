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
} Bullet;

Bullet bullets[128];
int bullet_count;
void BulletCreate(float x, float y, float dx, float dy) {
  Bullet *out = &bullets[bullet_count++];
  out->x = x;
  out->y = y;
  out->dx = dx;
  out->dy = dy;
}

void BulletUpdate() {
  for (int i = 0; i < bullet_count; ++i) {
    Bullet* b = &bullets[i];
    b->x += b->dx;
    b->y += b->dy;
  }
}

void BulletRender() {
  for (int i = 0; i < bullet_count; ++i) {
    Bullet a = bullets[i];
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawLine(renderer, a.x-a.dx, a.y-a.dy, a.x, a.y);
  }
}



typedef struct {
  float x;
  float y;
  float dx;
  float dy;
  float t; // angle
  int cooldown; // shoot cooldown
  bool dead;
} Ship;
const int SHOOT_COOLDOWN = 10; // frames

Ship player;
void ShipCreate(float x, float y, Ship *out) {
  out->x = x;
  out->y = y;
  out->dx = 0;
  out->dy = 0;
  out->t = 0;
  out->cooldown = 0;
  out->dead = false;
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
  }
  if (state[SDL_SCANCODE_DOWN]) {
    double pi = acos(-1);
    player.dx -= 0.4*cos(player.t * (pi/180));
    player.dy -= 0.4*sin(player.t * (pi/180));
  }
  if (state[SDL_SCANCODE_RIGHT]) {
    player.t += 5;
  }
  if (state[SDL_SCANCODE_LEFT]) {
    player.t -= 5;
  }
  if (state[SDL_SCANCODE_SPACE]) {
    if (player.cooldown == 0) {
      double pi = acos(-1);
      float dx = 10*cos(player.t * (pi/180));
      float dy = 10*sin(player.t * (pi/180));
      BulletCreate(player.x, player.y, dx, dy);
      player.cooldown = SHOOT_COOLDOWN;
    }
  }
}

void PlayerUpdate() {
  if (player.dead) return;
  if (player.cooldown > 0) player.cooldown--;
  PlayerInput();
  player.x = saneMod(player.x + player.dx, SCREEN_WIDTH);
  player.y = saneMod(player.y + player.dy, SCREEN_HEIGHT);
}

void PlayerRender() {
  if (player.dead) return;
  TextureRenderEx(shipTex, player.x-shipTex.w/2.0, player.y-shipTex.h/2.0, NULL, player.t, NULL, SDL_FLIP_NONE);
}



SDL_Rect CollisionMask(Texture tex, float x, float y, int border) {
  return (SDL_Rect){.x=x+border, .y=y+border, .w=tex.w-border*2, .h=tex.h-border*2};
}

bool arrayHas(int* arr, int len, int target) {
  for (int i = 0; i < len; ++i) {
    if (arr[i] == target) return true;
  }
  return false;
}

void HandleCollisions() {
  int bToKill[128]; // dead list for bullets
  int bToKill_count = 0;
  int aToKill[128]; // dead list for asteroids
  int aToKill_count = 0;

  // check oob asteroids
  for (int i = 0; i < bullet_count; ++i) {
    Bullet b = bullets[i];
    if (b.x < 0 || SCREEN_WIDTH <= b.x || b.y < 0 || SCREEN_HEIGHT <= b.y) {
      if (!arrayHas(bToKill, bToKill_count, i)) bToKill[bToKill_count++] = i;
    }
  }

  // check asteroid collisions
  for (int j = 0; j < asteroid_count; ++j) {
    Asteroid a = asteroids[j];
    SDL_Rect aMask = CollisionMask(asteroidTex, a.x, a.y, 2);

    // with player
    SDL_Rect pMask = CollisionMask(shipTex, player.x, player.y, 4);
    if (SDL_HasIntersection(&aMask, &pMask) == SDL_TRUE) {
      if (!arrayHas(aToKill, aToKill_count, j)) aToKill[aToKill_count++] = j;
      player.dead = true;
    }

    // with bullets
    for (int i = 0; i < bullet_count; ++i) {
      Bullet b = bullets[i];
      int x1 = (int)(b.x);
      int x2 = (int)(b.x-b.dx);
      int y1 = (int)(b.y);
      int y2 = (int)(b.y-b.dy);
      if (SDL_IntersectRectAndLine(&aMask, &x1, &y1, &x2, &y2) == SDL_TRUE) {
        if (!arrayHas(aToKill, aToKill_count, j)) aToKill[aToKill_count++] = j;
        if (!arrayHas(bToKill, bToKill_count, i)) bToKill[bToKill_count++] = i;
      }
    }
  }

  // kill toKill queues
  for (int i = bToKill_count-1; i >= 0; --i) {
    int index = bToKill[i];
    bullets[index] = bullets[--bullet_count];
  }
  for (int i = aToKill_count-1; i >= 0; --i) {
    int index = aToKill[i];
    asteroids[index] = asteroids[--asteroid_count];
  }
}

void InitGame() {
  asteroid_count = 0;
  for (int i = 0; i < 10; ++i) {
    AsteroidCreate();
  }
  PlayerCreate();
  bullet_count = 0;
}
