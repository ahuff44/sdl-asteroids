// typedef struct {
//   float x;
//   float y;
//   float dx;
//   float dy;
//   float t; // angle
//   int cooldown; // shoot cooldown
//   bool dead;
// } Ship;
// const int SHOOT_COOLDOWN = 10; // frames

// Ship player;
// void ShipCreate(float x, float y, Ship *out) {
//   out->x = x;
//   out->y = y;
//   out->dx = 0;
//   out->dy = 0;
//   out->t = 0;
//   out->cooldown = 0;
//   out->dead = false;
// }

// void PlayerUpdate() {
//   if (player.dead) return;
//   if (player.cooldown > 0) player.cooldown--;
//   PlayerInput();
//   player.x = saneMod(player.x + player.dx, SCREEN_WIDTH);
//   player.y = saneMod(player.y + player.dy, SCREEN_HEIGHT);
// }

// void PlayerRender() {
//   if (player.dead) return;
//   TextureRenderEx(shipTex, player.x-shipTex.w/2.0, player.y-shipTex.h/2.0, NULL, player.t, NULL, SDL_FLIP_NONE);
// }



// void AsteroidUpdate() {
//   for (int i = 0; i < asteroid_count; ++i) {
//     Asteroid* a = &asteroids[i];
//     a->x = saneMod(a->x + a->dx, SCREEN_WIDTH);
//     a->y = saneMod(a->y + a->dy, SCREEN_HEIGHT);
//     a->t = saneMod(a->t + a->dt, 360);
//   }
// }






SDL_Rect CollisionMask2(Texture tex, int border) {
  int hw = tex.w/2 - border;
  int hh = tex.h/2 - border;
  return (SDL_Rect){.x=-hw, .y=-hh, .w=2*hw, .h=2*hh};
}

// void HandleCollisions() {
//   int bToKill[128]; // dead list for bullets
//   int bToKill_count = 0;
//   int aToKill[128]; // dead list for asteroids
//   int aToKill_count = 0;

//   // check oob asteroids
//   for (int i = 0; i < bullet_count; ++i) {
//     Bullet b = bullets[i];
//     if (b.x < 0 || SCREEN_WIDTH <= b.x || b.y < 0 || SCREEN_HEIGHT <= b.y) {
//       if (!arrayHas(bToKill, bToKill_count, i)) bToKill[bToKill_count++] = i;
//     }
//   }

//   // check asteroid collisions
//   for (int j = 0; j < asteroid_count; ++j) {
//     Asteroid a = asteroids[j];
//     SDL_Rect aMask = CollisionMask(asteroidTex, a.x, a.y, 2);

//     // with player
//     SDL_Rect pMask = CollisionMask(shipTex, player.x, player.y, 12);
//     if (SDL_HasIntersection(&aMask, &pMask) == SDL_TRUE) {
//       if (!arrayHas(aToKill, aToKill_count, j)) aToKill[aToKill_count++] = j;
//       player.dead = true;
//     }

//     // with bullets
//     for (int i = 0; i < bullet_count; ++i) {
//       Bullet b = bullets[i];
//       int x1 = (int)(b.x);
//       int x2 = (int)(b.x-b.dx);
//       int y1 = (int)(b.y);
//       int y2 = (int)(b.y-b.dy);
//       if (SDL_IntersectRectAndLine(&aMask, &x1, &y1, &x2, &y2) == SDL_TRUE) {
//         if (!arrayHas(aToKill, aToKill_count, j)) aToKill[aToKill_count++] = j;
//         if (!arrayHas(bToKill, bToKill_count, i)) bToKill[bToKill_count++] = i;
//       }
//     }
//   }

//   // kill toKill queues
//   for (int i = bToKill_count-1; i >= 0; --i) {
//     int index = bToKill[i];
//     bullets[index] = bullets[--bullet_count];
//   }
//   for (int i = aToKill_count-1; i >= 0; --i) {
//     int index = aToKill[i];
//     asteroids[index] = asteroids[--asteroid_count];
//   }
// }


//
// old code end
//
// new ECS code start
//

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
#define ASSERT_VALID_ENTITIY(e) (assert(0 <= (e) && (e) < MAX_ENTITIES))

int num_entities;

//
// forward declares
//
Entity ECSBulletCreate(float x, float y, float dx, float dy);
Entity ECSAsteroidCreate();
Entity ECSPlayerCreate();

//
// Components
//

// enum ComponentTypes {
//   C_COLLIDE,
//   C_DISPLAY,
//   C_DISPLAYBULLET,
//   C_POSITION,
//   C_RECVMOVE,
//   C_RECVSHOOT,
//   C_VELOCITY,

//   NUM_COMPONENTS,
// };

// TODO macro this boilerplate up?:

typedef enum CollisionType {
  COLLIDE_KILL,
  COLLIDE_DIE,
} CollisionType;
typedef struct {
  bool initd;
  SDL_Rect rect;
  CollisionType type;
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
  float x;
  float y;
  float t; // angle
} PositionC;

typedef struct {
  bool initd;
} RecvMoveC;

typedef struct {
  bool initd;
  int cooldown;
} RecvShootC;

typedef struct {
  bool initd;
  float dx;
  float dy;
  float dt;
  bool wrap;
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
  c.initd = true;
  collideCData[e] = c;
}

void attachDisplayC(Entity e, DisplayC c) {
  ASSERT_VALID_ENTITIY(e);
  c.initd = true;
  displayCData[e] = c;
}

void attachDisplayBulletC(Entity e, DisplayBulletC c) {
  ASSERT_VALID_ENTITIY(e);
  c.initd = true;
  displayBulletCData[e] = c;
}

void attachPositionC(Entity e, PositionC c) {
  ASSERT_VALID_ENTITIY(e);
  c.initd = true;
  positionCData[e] = c;
}

void attachRecvMoveC(Entity e, RecvMoveC c) {
  ASSERT_VALID_ENTITIY(e);
  c.initd = true;
  recvMoveCData[e] = c;
}

void attachRecvShootC(Entity e, RecvShootC c) {
  ASSERT_VALID_ENTITIY(e);
  c.initd = true;
  recvShootCData[e] = c;
}

void attachVelocityC(Entity e, VelocityC c) {
  ASSERT_VALID_ENTITIY(e);
  c.initd = true;
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
  SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);

  for (int i = 0; i < MAX_ENTITIES; ++i) {
    if (!DisplayDebugNodes[i]) continue;
    PositionC* positionC = &positionCData[i];
    CollideC* collideC = &collideCData[i];

    SDL_Rect rect = collideC->rect;
    rect.x += positionC->x;
    rect.y += positionC->y;
    SDL_RenderDrawRect(renderer, &rect);
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
    PositionC* positionC = &positionCData[i];
    DisplayC* displayC = &displayCData[i];

    int x = positionC->x-displayC->tex.w/2;
    int y = positionC->y-displayC->tex.h/2;
    TextureRenderEx(displayC->tex, x, y, NULL, positionC->t, NULL, SDL_FLIP_NONE);
  }
}

bool checkDisplayBulletNode(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  return (positionCData[e].initd
       && velocityCData[e].initd
       && displayBulletCData[e].initd);
}
void processDisplayBullet() {
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

  for (int i = 0; i < MAX_ENTITIES; ++i) {
    if (!DisplayBulletNodes[i]) continue;
    PositionC* positionC = &positionCData[i];
    VelocityC* velocityC = &velocityCData[i];
    DisplayBulletC* displayBulletC = &displayBulletCData[i];

    SDL_RenderDrawLine(renderer, positionC->x-velocityC->dx, positionC->y-velocityC->dy, positionC->x, positionC->y);
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
    PositionC* positionC = &positionCData[i];
    VelocityC* velocityC = &velocityCData[i];

    positionC->x += velocityC->dx;
    positionC->y += velocityC->dy;
    if (velocityC->wrap) positionC->x = saneMod(positionC->x, SCREEN_WIDTH);
    if (velocityC->wrap) positionC->y = saneMod(positionC->y, SCREEN_HEIGHT);

    positionC->t = saneMod(positionC->t + velocityC->dt, 360);
  }
}

bool checkInputMoveNode(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  return (recvMoveCData[e].initd
       && positionCData[e].initd
       && velocityCData[e].initd);
}
void processInputMove(const Uint8* state) {
  for (int i = 0; i < MAX_ENTITIES; ++i) {
    if (!InputMoveNodes[i]) continue;
    RecvMoveC* recvMoveC = &recvMoveCData[i];
    PositionC* positionC = &positionCData[i];
    VelocityC* velocityC = &velocityCData[i];

    if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W]) {
      double pi = acos(-1);
      velocityC->dx += 0.4*cos(positionC->t * (pi/180));
      velocityC->dy += 0.4*sin(positionC->t * (pi/180));
    }
    if (state[SDL_SCANCODE_DOWN]) {
      double pi = acos(-1);
      velocityC->dx -= 0.4*cos(positionC->t * (pi/180));
      velocityC->dy -= 0.4*sin(positionC->t * (pi/180));
    }
    if (state[SDL_SCANCODE_RIGHT]) {
      positionC->t += 5;
    }
    if (state[SDL_SCANCODE_LEFT]) {
      positionC->t -= 5;
    }
  }
}

bool checkInputShootNode(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  return (recvShootCData[e].initd
       && positionCData[e].initd);
}
void processInputShoot(const Uint8* state) {
  const int SHOOT_COOLDOWN = 10; // frames

  for (int i = 0; i < MAX_ENTITIES; ++i) {
    if (!InputShootNodes[i]) continue;
    RecvShootC* recvShootC = &recvShootCData[i];
    PositionC* positionC = &positionCData[i];

    if (recvShootC->cooldown > 0) recvShootC->cooldown--;
    if (state[SDL_SCANCODE_SPACE]) {
      if (recvShootC->cooldown == 0) {
        recvShootC->cooldown = SHOOT_COOLDOWN;
        double pi = acos(-1);
        float dx = 10*cos(positionC->t * (pi/180));
        float dy = 10*sin(positionC->t * (pi/180));
        ECSBulletCreate(positionC->x, positionC->y, dx, dy);
      }
    }
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
    PositionC* iPositionC = &positionCData[i];
    CollideC* iCollideC = &collideCData[i];
    for (int j = i+1; j < MAX_ENTITIES; ++j) {
      if (!CollideNodes[j]) continue;
      PositionC* jPositionC = &positionCData[j];
      CollideC* jCollideC = &collideCData[j];

      // if (SDL_HasIntersection(iCollideC.rect, jCollideC.rect) == SDL_TRUE) {
      //   if (iCollideC->type == COLLIDE_KILL && jCollideC->type == COLLIDE_DIE) {
      //     deregisterEntity(j);
      //   }
      //   if (iCollideC->type == COLLIDE_DIE && jCollideC->type == COLLIDE_KILL) {
      //     deregisterEntity(i);
      //   }
      //   // @TODO too simple; asteroids sometimes kill but sometimes die
      // }
    }
  }
}
// @todo
//   deregister needs to wait until end of frame
//   bullets offscreen need to deregister

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

void ProcessAll() {
  const Uint8* state = SDL_GetKeyboardState(NULL);
  processDisplayDebug();
  processDisplay();
  processDisplayBullet();
  processMove();
  processInputMove(state);
  processInputShoot(state);
  processCollide();
}

void ZeroECS() {
  // zero out components
  memset(&collideCData, 0, sizeof(collideCData));
  memset(&displayCData, 0, sizeof(displayCData));
  memset(&displayBulletCData, 0, sizeof(displayBulletCData));
  memset(&positionCData, 0, sizeof(positionCData));
  memset(&recvMoveCData, 0, sizeof(recvMoveCData));
  memset(&recvShootCData, 0, sizeof(recvShootCData));
  memset(&velocityCData, 0, sizeof(velocityCData));

  // zero out nodes
  memset(&DisplayDebugNodes, 0, sizeof(DisplayDebugNodes));
  memset(&DisplayNodes, 0, sizeof(DisplayNodes));
  memset(&DisplayBulletNodes, 0, sizeof(DisplayBulletNodes));
  memset(&MoveNodes, 0, sizeof(MoveNodes));
  memset(&InputMoveNodes, 0, sizeof(InputMoveNodes));
  memset(&InputShootNodes, 0, sizeof(InputShootNodes));
  memset(&CollideNodes, 0, sizeof(CollideNodes));

  num_entities = 0;
}



//
// Game
//

Entity player;
void InitGame() {
  ZeroECS();

  player = ECSPlayerCreate();
  for (int i = 0; i < 10; ++i) {
    ECSAsteroidCreate();
  }
}

Entity ECSPlayerCreate() {
  Entity e = preregisterEntity();
  attachPositionC(e, (PositionC){.x=SCREEN_WIDTH/2, .y=SCREEN_HEIGHT/2, .t=0});
  attachVelocityC(e, (VelocityC){.dx=0, .dy=0, .dt=0, .wrap=true});
  attachDisplayC(e, (DisplayC){.tex=shipTex});
  attachCollideC(e, (CollideC){.rect=CollisionMask2(shipTex, 12)});
  attachRecvMoveC(e, (RecvMoveC){});
  attachRecvShootC(e, (RecvShootC){});
  registerEntity(e);
  return e;
}

Entity ECSAsteroidCreate() {
  Entity e = preregisterEntity();

  // figure out position
  PositionC playerPositionC = positionCData[player];
  assert(playerPositionC.initd);

  int x;
  int y;
  do {
    x = randInt(SCREEN_WIDTH);
    y = randInt(SCREEN_HEIGHT);
  } while (
    taxicabDist2(x, y, playerPositionC.x, playerPositionC.y) < 300
  );

  // attach components
  attachPositionC(e, (PositionC){.x=x, .y=y, .t=randInt(360)});
  attachVelocityC(e, (VelocityC){.dx=randIntIn(-5, 6), .dy=randIntIn(-5, 6), .dt=randIntIn(-5, 6), .wrap=true});
  attachDisplayC(e, (DisplayC){.tex=asteroidTex});
  attachCollideC(e, (CollideC){.rect=CollisionMask2(asteroidTex, 2)});

  registerEntity(e);
  return e;
}

Entity ECSBulletCreate(float x, float y, float dx, float dy) {
  Entity e = preregisterEntity();
  attachPositionC(e, (PositionC){.x=x, .y=y, .t=0});
  attachVelocityC(e, (VelocityC){.dx=dx, .dy=dy, .dt=0, .wrap=false});
  attachCollideC(e, (CollideC){.rect=(SDL_Rect){.x=-dx, .y=-dy, .w=dx, .h=dy}});
  attachDisplayBulletC(e, (DisplayBulletC){});
  registerEntity(e);
  return e;
}
