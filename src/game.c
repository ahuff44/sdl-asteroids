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
  assert(memcmp(&player, &(Ship){0}, sizeof(player)) != 0); // assert(player is initialized)
  Asteroid *out = &asteroids[asteroid_count++];
  do {
    out->x = randInt(SCREEN_WIDTH);
    out->y = randInt(SCREEN_HEIGHT);
  } while (
    taxicabDist((SDL_Point){.x=out->x, .y=out->y}, (SDL_Point){.x=player.x, .y=player.y}) < 300
  );
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
    TextureRenderEx(asteroidTex, a.x-asteroidTex.w/2, a.y-asteroidTex.h/2, NULL, a.t, NULL, SDL_FLIP_NONE);
  }
}



void BulletRender() {
  for (int i = 0; i < bullet_count; ++i) {
    Bullet a = bullets[i];
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawLine(renderer, a.x-a.dx, a.y-a.dy, a.x, a.y);
  }
}



SDL_Rect CollisionMask(Texture tex, float x, float y, int border) {
  return (SDL_Rect){.x=x+border, .y=y+border, .w=tex.w-border*2, .h=tex.h-border*2};
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
    SDL_Rect pMask = CollisionMask(shipTex, player.x, player.y, 12);
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
  PlayerCreate();
  asteroid_count = 0;
  for (int i = 0; i < 10; ++i) {
    AsteroidCreate();
  }
  bullet_count = 0;
}

/*
entities
  player
    position
    velocity
    display
    collide
    recvmove
    recvshoot
  asteroid
    position
    velocity
    display
    collide
  bullet
    position
    velocity
    displaybullet (0-length "flag component"?)
    collide (figure out rect collision bounds on creation)
processes
  displaydebug
    position
    collide
  display
    position
    display
  displaybullet
    position
    velocity
    displaybullet
  move
    position
    velocity
  inputmove
    recvmove
    velocity
  inputshoot
    recvshoot
    position
  collide
    position
    collide
*/

typedef int Entity;

const int MAX_ENTITIES = 256;
#define ASSERT_VALID_ENTITIY(e) (assert(0 < (e) && (e) < MAX_ENTITIES))

int num_entities;

//
// Components
//

enum ComponentTypes {
  C_COLLIDE,
  C_DISPLAY,
  C_DISPLAYBULLET,
  C_POSITION,
  C_RECVMOVE,
  C_RECVSHOOT,
  C_VELOCITY,

  NUM_COMPONENTS,
};

// TODO macro this boilerplate up?:

typedef struct {
  bool initd;
  SDL_Rect rect;
} CollideC;

typedef struct {
  bool initd;
  Texture tex;
} DisplayC;

typedef struct {
  bool initd;
} DisplayBulletC;

typedef struct {
  bool initd;
  int x;
  int y;
  int t; // angle
} PositionC;

typedef struct {
  bool initd;
} RecvMoveC;

typedef struct {
  bool initd;
} RecvShootC;

typedef struct {
  bool initd;
  int dx;
  int dy;
  int dt;
} VelocityC;

CollideC collideCData[MAX_ENTITIES];
DisplayC displayCData[MAX_ENTITIES];
DisplayBulletC displayBulletCData[MAX_ENTITIES];
PositionC positionCData[MAX_ENTITIES];
RecvMoveC recvMoveCData[MAX_ENTITIES];
RecvShootC recvShootCData[MAX_ENTITIES];
VelocityC velocityCData[MAX_ENTITIES];

void attachCollideC(Entity e, CollideC c) {
  ASSERT_VALID_ENTITIY(e);
  collideCData[e] = c;
}

void attachDisplayC(Entity e, DisplayC c) {
  ASSERT_VALID_ENTITIY(e);
  displayCData[e] = c;
}

void attachDisplayBulletC(Entity e, DisplayBulletC c) {
  ASSERT_VALID_ENTITIY(e);
  displayBulletCData[e] = c;
}

void attachPositionC(Entity e, PositionC c) {
  ASSERT_VALID_ENTITIY(e);
  positionCData[e] = c;
}

void attachRecvMoveC(Entity e, RecvMoveC c) {
  ASSERT_VALID_ENTITIY(e);
  recvMoveCData[e] = c;
}

void attachRecvShootC(Entity e, RecvShootC c) {
  ASSERT_VALID_ENTITIY(e);
  recvShootCData[e] = c;
}

void attachVelocityC(Entity e, VelocityC c) {
  ASSERT_VALID_ENTITIY(e);
  velocityCData[e] = c;
}

//
// Processes
//

bool DisplayDebugNodes[MAX_ENTITIES];
bool DisplayNodes[MAX_ENTITIES];
bool DisplayBulletNodes[MAX_ENTITIES];
bool MoveNodes[MAX_ENTITIES];
bool InputMoveNodes[MAX_ENTITIES];
bool InputShootNodes[MAX_ENTITIES];
bool CollideNodes[MAX_ENTITIES];

bool checkDisplayDebugNode(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  return (positionCData[e].initd
       && collideCData[e].initd);
}
void processDisplayDebug() {
  for (int i = 0; i < MAX_ENTITIES; ++i) {
    if (!DisplayDebugNodes[i]) continue;
    // @TODO
  }
}

bool checkDisplayNode(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  return (positionCData[e].initd
       && displayCData[e].initd);
}
void processDisplay() {
  for (int i = 0; i < MAX_ENTITIES; ++i) {
    if (!DisplayNodes[i]) continue;
    PositionC positionC = positionCData[i];
    DisplayC displayC = DisplayCData[i];
    TextureRenderEx(displayC.tex, positionC.x-displayC.tex.w/2, positionC.y-displayC.tex.h/2, NULL, positionC.t, NULL, SDL_FLIP_NONE);
  }
}

bool checkDisplayBulletNode(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  return (positionCData[e].initd
       && velocityCData[e].initd
       && displayBulletCData[e].initd);
}
void processDisplayBullet() {
  for (int i = 0; i < MAX_ENTITIES; ++i) {
    if (!DisplayBulletNodes[i]) continue;
    // @TODO
  }
}

bool checkMoveNode(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  return (positionCData[e].initd
       && velocityCData[e].initd);
}
void processMove() {
  for (int i = 0; i < MAX_ENTITIES; ++i) {
    if (!MoveNodes[i]) continue;
    // @TODO
  }
}

bool checkInputMoveNode(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  return (recvMoveCData[e].initd
       && velocityCData[e].initd);
}
void processInputMove() {
  for (int i = 0; i < MAX_ENTITIES; ++i) {
    if (!InputMoveNodes[i]) continue;
    // @TODO
  }
}

bool checkInputShootNode(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  return (recvShootCData[e].initd
       && positionCData[e].initd);
}
void processInputShoot() {
  for (int i = 0; i < MAX_ENTITIES; ++i) {
    if (!InputShootNodes[i]) continue;
    // @TODO
  }
}

bool checkCollideNode(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  return (positionCData[e].initd
       && collideCData[e].initd);
}
void processCollide() {
  for (int i = 0; i < MAX_ENTITIES; ++i) {
    if (!CollideNodes[i]) continue;
    // @TODO
  }
}

//
// Engine
//

Entity preregisterEntity() {
  // returns -1 if no entity slots left
  if (num_entities >= MAX_ENTITIES) {
    return -1;
  } else {
    return num_entities++;
  }
}

void registerEntity(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  if (checkDisplayDebugNode(e)) { DisplayDebugNodes[e] = true; }
  if (checkDisplayNode(e)) { DisplayNodes[e] = true; }
  if (checkDisplayBulletNode(e)) { DisplayBulletNodes[e] = true; }
  if (checkMoveNode(e)) { MoveNodes[e] = true; }
  if (checkInputMoveNode(e)) { InputMoveNodes[e] = true; }
  if (checkInputShootNode(e)) { InputShootNodes[e] = true; }
  if (checkCollideNode(e)) { CollideNodes[e] = true; }
}

void deregisterEntity(Entity e) {
  ASSERT_VALID_ENTITIY(e);

  // zero out components
  collideCData[e] = (CollideC){0};
  displayCData[e] = (DisplayC){0};
  displayBulletCData[e] = (DisplayBulletC){0};
  positionCData[e] = (PositionC){0};
  recvMoveCData[e] = (RecvMoveC){0};
  recvShootCData[e] = (RecvShootC){0};
  velocityCData[e] = (VelocityC){0};

  // zero out nodes
  DisplayDebugNodes[e] = false;
  DisplayNodes[e] = false;
  DisplayBulletNodes[e] = false;
  MoveNodes[e] = false;
  InputMoveNodes[e] = false;
  InputShootNodes[e] = false;
  CollideNodes[e] = false;
}

void processAll() {
  processDisplayDebug();
  processDisplay();
  processDisplayBullet();
  processMove();
  processInputMove();
  processInputShoot();
  processCollide();
}
