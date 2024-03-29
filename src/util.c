#include <time.h>

#define ABS(x) ((x) < 0 ? -(x) : (x))

void InitRNG(void) {
  srand(time(NULL));
}

int randInt(int max) {
  // returns an int in [0, max)
  // not totally uniform (esp for large values of `max`)
  // see https://stackoverflow.com/a/822361
  return rand() % max;
}

int randIntIn(int min, int max) {
  // returns an int in [min, max)
  assert(min < max);
  return min+randInt(max-min);
}

int saneMod(int x, int m) {
  // bad performance maybe? not really if you're within [-m, 2m] (which is most common)
  // more worrying is if these loops are infinite b/c of c math differences...
  while (x < 0) { x += m; }
  while (x >= m) { x -= m; }
  return x;
}
float saneModF(float x, float m) {
  while (x < 0) { x += m; }
  while (x >= m) { x -= m; }
  return x;
}

int taxicabDist(int x1, int y1, int x2, int y2) {
  return ABS(x1 - x2) + ABS(y1 - y2);
}

bool arrayHas(int* arr, int len, int target) {
  for (int i = 0; i < len; ++i) {
    if (arr[i] == target) return true;
  }
  return false;
}

void memprint(size_t start, size_t len) {
  // e.g. memprint(&myvar, sizeof(myvar));
  for (int i = 0; i < len; ++i) {
    Uint8 data = *((char *)(start + i));
    printf("%02x ", data);
  }
  printf("\n");
}

Arena TempStorage;

void* temp_alloc(size_t n) {
  void* res = arena_alloc(&TempStorage, n);
  // printf("TempStorage->size=%zu\n", TempStorage.size);
  return res;
}

char* strf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  size_t n = 1 + vsnprintf(NULL, 0, fmt, args);
  va_end(args);
  char* str = (char *)temp_alloc(n);
  va_start(args, fmt);
  vsnprintf(str, n, fmt, args);
  va_end(args);
  return str;
}
