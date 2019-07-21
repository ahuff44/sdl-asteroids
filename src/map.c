// a janky, unoptimized map (O(n) find instead of O(1))

// untested!!!

#if INTERFACE
struct MapEntry {
  int key;
  int val;
};
#endif

MapEntry* map_new() {
  MapEntry* map = NULL;
  return map;
}

int map__indexof(MapEntry* map, int key) {
  for (int i = 0; i < buf_len(map); ++i) {
    if (map[i].key == key) return i;
  }
  return -1;
}

int map_has(MapEntry* map, int key) {
  return map__indexof(map, key) != -1;
}

void map_set(MapEntry* map, int key, int val) {
  int index = map__indexof(map, key);
  MapEntry entry = {.key=key, .val=val};
  if (index == -1) {
    buf_push(map, entry);
  } else {
    map[index] = entry;
  }
}

int map_get(MapEntry* map, int key) {
  int index = map__indexof(map, key);
  if (index == -1) {
    return 0;
  } else {
    return map[index].val;
  }
}
