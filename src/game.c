bool DRAW_DEBUG;

SDL_Rect CollisionMask(Texture tex, int border) {
  int hw = tex.w/2 - border;
  int hh = tex.h/2 - border;
  return (SDL_Rect){.x=-hw, .y=-hh, .w=2*hw, .h=2*hh};
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

#if INTERFACE
typedef int Entity;
#endif

#define ASSERT_VALID_ENTITIY(e) (assert(0 <= (e) && (e) < MAX_ENTITIES))

int num_entities;

//
// Components
//

// TODO macro this boilerplate up?:

#if INTERFACE
enum CollisionType {
  COLLIDE_NONE, // mainly just to avoid 0-initialization bugs
  COLLIDE_SHIP,
  COLLIDE_ASTEROID,
  COLLIDE_BIG_ASTEROID,
  COLLIDE_BULLET,
};
struct CollideC {
  bool initd;
  SDL_Rect rect;
  CollisionType type;
};

struct DisplayC {
  bool initd;
  Texture tex;
};

struct DisplayBulletC {
  bool initd;
};

struct PositionC {
  bool initd;
  float x;
  float y;
  float t; // angle
};

struct RecvMoveC {
  bool initd;
};

struct RecvShootC {
  bool initd;
  int cooldown;
};

struct RecvDebugC {
  bool initd;
};

struct VelocityC {
  bool initd;
  float dx;
  float dy;
  float dt;
  bool wrap;
};
#endif

Arena TempStorage;
char* stringTalloc(char* ptr, int n) {
  // takes ptr and returns a pointer that points to a permanent copy of the string
  // (allocated in TempStorage)
  assert(strlen(ptr) == n);
  void* newptr = arena_alloc(&TempStorage, n+1)
  memcpy(newptr, ptr, n+1);
  return (char *)newptr;
}

const int SPRINT_BUFFER_SIZE = 1024*2;
char sprintBuffer[SPRINT_BUFFER_SIZE];
char* sprintCollisionType(CollisionType x) {
  switch (x) {
    case COLLIDE_NONE: return "COLLIDE_NONE";
    case COLLIDE_SHIP: return "COLLIDE_SHIP";
    case COLLIDE_ASTEROID: return "COLLIDE_ASTEROID";
    case COLLIDE_BIG_ASTEROID: return "COLLIDE_BIG_ASTEROID";
    case COLLIDE_BULLET: return "COLLIDE_BULLET";
    default: return "<unknown>";
  }
}
char* sprintCollideC(CollideC x) {
  char* ptr;
  int n = sprintf(ptr, "(CollideC){ initd: %d, rect: %s }", x.initd, "<SDL_Rect>", sprintCollisionType(x.type));
  char* newptr = stringTalloc(ptr, n);
  return newptr;
}

CollideC collideCData[MAX_ENTITIES];
DisplayC displayCData[MAX_ENTITIES];
DisplayBulletC displayBulletCData[MAX_ENTITIES];
PositionC positionCData[MAX_ENTITIES];
RecvMoveC recvMoveCData[MAX_ENTITIES];
RecvShootC recvShootCData[MAX_ENTITIES];
RecvDebugC recvDebugCData[MAX_ENTITIES];
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

void attachRecvDebugC(Entity e, RecvDebugC c) {
  ASSERT_VALID_ENTITIY(e);
  c.initd = true;
  recvDebugCData[e] = c;
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
bool InputDebugNodes[MAX_ENTITIES];
bool CollideNodes[MAX_ENTITIES];

bool checkDisplayDebugNode(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  return (positionCData[e].initd
       && collideCData[e].initd);
}
void processDisplayDebug(void) {
  if (!DRAW_DEBUG) return;
  SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);

  for (int i = 0; i < num_entities; ++i) {
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
void processDisplay(void) {
  for (int i = 0; i < num_entities; ++i) {
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
void processDisplayBullet(void) {
  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

  for (int i = 0; i < num_entities; ++i) {
    if (!DisplayBulletNodes[i]) continue;
    // printf("processDisplayBullet for %d\n", i);
    PositionC* positionC = &positionCData[i];
    VelocityC* velocityC = &velocityCData[i];
    // DisplayBulletC* displayBulletC = &displayBulletCData[i];

    SDL_RenderDrawLine(renderer, positionC->x-velocityC->dx, positionC->y-velocityC->dy, positionC->x, positionC->y);
  }
}

bool checkMoveNode(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  return (positionCData[e].initd
       && velocityCData[e].initd);
}
void processMove(void) {
  for (int i = 0; i < num_entities; ++i) {
    if (!MoveNodes[i]) continue;
    PositionC* positionC = &positionCData[i];
    VelocityC* velocityC = &velocityCData[i];

    positionC->x += velocityC->dx;
    positionC->y += velocityC->dy;
    float wrapX = saneModF(positionC->x, SCREEN_WIDTH);
    float wrapY = saneModF(positionC->y, SCREEN_HEIGHT);
    if (positionC->x != wrapX) {
      if (velocityC->wrap) {
        positionC->x = wrapX;
      } else {
        deregisterEntity(i);
      }
    }
    if (positionC->y != wrapY) {
      if (velocityC->wrap) {
        positionC->y = wrapY;
      } else {
        deregisterEntity(i);
      }
    }

    positionC->t = saneModF(positionC->t + velocityC->dt, 360);
  }
}

bool checkInputMoveNode(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  printf("%s\n", sprintCollideC(collideCData[e]));
  return (recvMoveCData[e].initd
       && positionCData[e].initd
       && velocityCData[e].initd);
}
void processInputMove(const Uint8* state) {
  for (int i = 0; i < num_entities; ++i) {
    if (!InputMoveNodes[i]) continue;
    // RecvMoveC* recvMoveC = &recvMoveCData[i];
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

  for (int i = 0; i < num_entities; ++i) {
    // if (!InputShootNodes[i]) continue;
    if (!checkInputShootNode((Entity)i)) continue;
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

bool checkInputDebugNode(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  return (recvDebugCData[e].initd);
}
void processInputDebug(const Uint8* state) {
  for (int i = 0; i < num_entities; ++i) {
    if (!InputDebugNodes[i]) continue;
    // RecvDebugC* recvDebugC = &recvDebugCData[i];

    if (state[SDL_SCANCODE_C]) {
      DRAW_DEBUG = true;
      // can't turn it off
    }
  }
}

bool checkCollideNode(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  return (positionCData[e].initd
       && collideCData[e].initd);
}
void processCollide(void) {
  for (int i = 0; i < num_entities; ++i) {
    if (!CollideNodes[i]) continue;
    PositionC* iPositionC = &positionCData[i];
    CollideC* iCollideC = &collideCData[i];
    SDL_Rect iRect = iCollideC->rect;
    iRect.x += iPositionC->x;
    iRect.y += iPositionC->y;
    for (int j = i+1; j < num_entities; ++j) {
      if (!CollideNodes[j]) continue;
      PositionC* jPositionC = &positionCData[j];
      CollideC* jCollideC = &collideCData[j];
      SDL_Rect jRect = jCollideC->rect;
      jRect.x += jPositionC->x;
      jRect.y += jPositionC->y;

      if (SDL_HasIntersection(&iRect, &jRect) != SDL_TRUE) continue;

      int iT = iCollideC->type;
      int jT = jCollideC->type;
      if (iT == COLLIDE_SHIP && (jT == COLLIDE_ASTEROID || jT == COLLIDE_BIG_ASTEROID)) {
        deregisterEntity(i);
      } else if ((iT == COLLIDE_ASTEROID || iT == COLLIDE_BIG_ASTEROID) && jT == COLLIDE_SHIP) {
        deregisterEntity(j);

      } else if (iT == COLLIDE_BULLET && jT == COLLIDE_ASTEROID) {
        deregisterEntity(i);
        deregisterEntity(j);
      } else if (iT == COLLIDE_ASTEROID && jT == COLLIDE_BULLET) {
        deregisterEntity(i);
        deregisterEntity(j);

      } else if (iT == COLLIDE_BULLET && jT == COLLIDE_BIG_ASTEROID) {
        deregisterEntity(i);
        AsteroidSplit(j);
        deregisterEntity(j);
      } else if (iT == COLLIDE_BIG_ASTEROID && jT == COLLIDE_BULLET) {
        AsteroidSplit(i);
        deregisterEntity(i);
        deregisterEntity(j);

      } else if ((iT == COLLIDE_ASTEROID || iT == COLLIDE_BIG_ASTEROID) && (jT == COLLIDE_ASTEROID || jT == COLLIDE_BIG_ASTEROID)) {
        // // hacky
        // VelocityC* iVelocityC = &velocityCData[i];
        // VelocityC* jVelocityC = &velocityCData[j];
        // assert(iVelocityC->initd && jVelocityC->initd);

        // iPositionC->x -= iVelocityC->dx;
        // iPositionC->y -= iVelocityC->dy;
        // float tempX = jVelocityC->dx;
        // float tempY = jVelocityC->dy;
        // jVelocityC->dx = iVelocityC->dx;
        // jVelocityC->dy = iVelocityC->dy;
        // iVelocityC->dx = tempX;
        // iVelocityC->dy = tempY;
      }
    }
  }
}

//
// Engine
//

Entity preregisterEntity(void) {
  printf("preregistering %d\n", num_entities);
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
  if (checkInputDebugNode(e)) { InputDebugNodes[e] = true; }
  if (checkCollideNode(e)) { CollideNodes[e] = true; }
}

Entity* toKill = NULL;
void _frameDeregisterInit(void) {
  buf_clear(toKill);
}
bool alreadyKilled(Entity e) {
  bool res = alreadyKilled_(e);
  printf("alreadyKilled(%d)=%d\n", e, res);
  return res;
}
bool alreadyKilled_(Entity e) {
  for (size_t i = 0; i < buf_len(toKill); ++i) {
    if (toKill[i] == e) return true;
  }
  return false;
}
void deregisterEntity(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  if (!alreadyKilled(e)) buf_push(toKill, e);
}
void _deregisterEntityFRD(Entity e) {
  ASSERT_VALID_ENTITIY(e);
  bool res1 = checkInputShootNode(e);

  // zero out components
  collideCData[e] = (CollideC){0};
  displayCData[e] = (DisplayC){0};
  displayBulletCData[e] = (DisplayBulletC){0};
  positionCData[e] = (PositionC){0};
  recvMoveCData[e] = (RecvMoveC){0};
  recvShootCData[e] = (RecvShootC){0};
  recvDebugCData[e] = (RecvDebugC){0};
  velocityCData[e] = (VelocityC){0};
  bool res2 = checkInputShootNode(e);

  // zero out nodes
  DisplayDebugNodes[e] = false;
  DisplayNodes[e] = false;
  DisplayBulletNodes[e] = false;
  MoveNodes[e] = false;
  InputMoveNodes[e] = false;
  InputShootNodes[e] = false;
  InputDebugNodes[e] = false;
  CollideNodes[e] = false;
  bool res3 = checkInputShootNode(e);
  printf("checkInputShootNode(%d)=%d / %d / %d\n", e, res1, res2, res3);
}
void _deregisterFRD(void) {
  for (size_t i = 0; i < buf_len(toKill); ++i) {
    Entity e = toKill[i];
    ASSERT_VALID_ENTITIY(e);

    // CollideC* collideC = &collideCData[e];
    // assert(collideC->initd);
    // printf("Killing %d (type %d)\n", e, collideC->type);
    _deregisterEntityFRD(e);
  }
}

void ProcessAll(void) {
  _frameDeregisterInit();

  const Uint8* state = SDL_GetKeyboardState(NULL);
  processInputDebug(state);
  processInputMove(state);
  processInputShoot(state);
  processMove();
  processCollide();
  processDisplay();
  processDisplayBullet();
  processDisplayDebug();

  _deregisterFRD();
}

void ZeroECS(void) {
  // zero out components
  memset(&collideCData, 0, sizeof(collideCData));
  memset(&displayCData, 0, sizeof(displayCData));
  memset(&displayBulletCData, 0, sizeof(displayBulletCData));
  memset(&positionCData, 0, sizeof(positionCData));
  memset(&recvMoveCData, 0, sizeof(recvMoveCData));
  memset(&recvShootCData, 0, sizeof(recvShootCData));
  memset(&recvDebugCData, 0, sizeof(recvDebugCData));
  memset(&velocityCData, 0, sizeof(velocityCData));

  // zero out nodes
  memset(&DisplayDebugNodes, 0, sizeof(DisplayDebugNodes));
  memset(&DisplayNodes, 0, sizeof(DisplayNodes));
  memset(&DisplayBulletNodes, 0, sizeof(DisplayBulletNodes));
  memset(&MoveNodes, 0, sizeof(MoveNodes));
  memset(&InputMoveNodes, 0, sizeof(InputMoveNodes));
  memset(&InputShootNodes, 0, sizeof(InputShootNodes));
  memset(&InputDebugNodes, 0, sizeof(InputDebugNodes));
  memset(&CollideNodes, 0, sizeof(CollideNodes));

  num_entities = 0;
}



//
// Game
//

Entity cloneEntity(Entity e) {
  Entity e2 = preregisterEntity();
  attachCollideC(e2, collideCData[e]);
  attachDisplayC(e2, displayCData[e]);
  attachDisplayBulletC(e2, displayBulletCData[e]);
  attachPositionC(e2, positionCData[e]);
  attachRecvMoveC(e2, recvMoveCData[e]);
  // RecvShootC data = recvShootCData[e];
  // memprint(&data, sizeof(data));
  // printf("initd=%d\n", data.initd);
  attachRecvShootC(e2, recvShootCData[e]);
  attachRecvDebugC(e2, recvDebugCData[e]);
  attachVelocityC(e2, velocityCData[e]);
  registerEntity(e2);
  // printf("e2=%d\n", e2);
  return e2;
}

Entity player;
void InitGame(void) {
  ZeroECS();
  ECSDebugCreate();

  player = ECSPlayerCreate();
  for (int i = 0; i < 10; ++i) {
    ECSAsteroidCreate();
  }
}

Entity ECSDebugCreate(void) {
  Entity e = preregisterEntity();
  attachRecvDebugC(e, (RecvDebugC){});
  registerEntity(e);
  return e;
}

Entity ECSPlayerCreate(void) {
  Entity e = preregisterEntity();
  attachPositionC(e, (PositionC){.x=SCREEN_WIDTH/2, .y=SCREEN_HEIGHT/2, .t=0});
  attachVelocityC(e, (VelocityC){.dx=0, .dy=0, .dt=0, .wrap=true});
  attachDisplayC(e, (DisplayC){.tex=shipTex});
  attachCollideC(e, (CollideC){.rect=CollisionMask(shipTex, 12), .type=COLLIDE_SHIP});
  attachRecvMoveC(e, (RecvMoveC){});
  attachRecvShootC(e, (RecvShootC){});
  registerEntity(e);
  return e;
}

Entity ECSAsteroidCreate(void) {
  Entity e = preregisterEntity();

  // figure out position
  PositionC* playerPositionC = &positionCData[player];
  assert(playerPositionC->initd);

  int x;
  int y;
  do {
    x = randInt(SCREEN_WIDTH);
    y = randInt(SCREEN_HEIGHT);
  } while (
    taxicabDist(x, y, playerPositionC->x, playerPositionC->y) < 300
  );

  // attach components
  attachPositionC(e, (PositionC){.x=x, .y=y, .t=randInt(360)});
  if (randInt(100) < 20) {
    initBigAsteroid(e);
  } else {
    initSmallAsteroid(e);
  }

  registerEntity(e);
  return e;
}

void initSmallAsteroid(Entity e) {
  attachVelocityC(e, (VelocityC){.dx=randIntIn(-5, 6), .dy=randIntIn(-5, 6), .dt=randIntIn(-5, 6), .wrap=true});
  attachDisplayC(e, (DisplayC){.tex=asteroidTex});
  attachCollideC(e, (CollideC){.rect=CollisionMask(asteroidTex, 2), .type=COLLIDE_ASTEROID});
}

void initBigAsteroid(Entity e) {
  attachVelocityC(e, (VelocityC){.dx=randIntIn(-2, 3), .dy=randIntIn(-2, 3), .dt=randIntIn(-2, 3), .wrap=true});
  attachDisplayC(e, (DisplayC){.tex=asteroidBigTex});
  attachCollideC(e, (CollideC){.rect=CollisionMask(asteroidBigTex, 2), .type=COLLIDE_BIG_ASTEROID});
}

void AsteroidSplit(Entity e) {
  int num_children = randIntIn(2, 5);
  for (int i = 0; i < num_children; ++i) {
    Entity child = cloneEntity(e);
    initSmallAsteroid(child);
  }
}


Entity ECSBulletCreate(float x, float y, float dx, float dy) {
  Entity e = preregisterEntity();
  attachPositionC(e, (PositionC){.x=x, .y=y, .t=0});
  attachVelocityC(e, (VelocityC){.dx=dx, .dy=dy, .dt=0, .wrap=false});

  float x1 = 0;
  float y1 = 0;
  float x2 = dx;
  float y2 = dy;
  SDL_Rect rect = {.x=MIN(x1, x2)-dx, .y=MIN(y1, y2)-dy, .w=ABS(dx), .h=ABS(dy)};

  attachCollideC(e, (CollideC){.rect=rect, .type=COLLIDE_BULLET});
  attachDisplayBulletC(e, (DisplayBulletC){});
  registerEntity(e);
  return e;
}
