#include <time.h>

void InitRNG() {
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
