#include "map.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAP_CAPACITY 100003 

struct Map {
  int (*lower_than)(void *key1, void *key2);
  int (*is_equal)(void *key1, void *key2);
  List **buckets;       
  int current_bucket;   
};

typedef Map Map;

int (*current_lt)(void *, void *) = NULL;

int pair_lt(void *pair1, void *pair2) {
  return (current_lt(((MapPair *)pair1)->key, ((MapPair *)pair2)->key));
}


unsigned long hash_key(void *key) {
    unsigned long hash = 5381;
    char *str = (char *)key;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; 
    }
    return hash;
}

unsigned long get_bucket_index(void *key) {
    return hash_key(key) % MAP_CAPACITY;
}

Map *map_create(int (*is_equal)(void *key1, void *key2)) {
  Map *newMap = (Map *)malloc(sizeof(Map));
  newMap->lower_than = NULL;
  newMap->is_equal = is_equal;
  
 
  newMap->buckets = (List **)calloc(MAP_CAPACITY, sizeof(List *));
  newMap->current_bucket = -1;
  return newMap;
}

Map *sorted_map_create(int (*lower_than)(void *key1, void *key2)) {
  Map *newMap = (Map *)malloc(sizeof(Map));
  newMap->lower_than = lower_than;
  newMap->is_equal = NULL;
  newMap->buckets = (List **)calloc(MAP_CAPACITY, sizeof(List *));
  newMap->current_bucket = -1;
  return newMap;
}

int _is_equal(Map *map, MapPair *pair, void *key) {
  return ((map->is_equal && map->is_equal(pair->key, key)) ||
          (map->lower_than && !map->lower_than(pair->key, key) &&
           !map->lower_than(key, pair->key)));
}


void map_insert(Map *map, void *key, void *value) {
  unsigned long idx = get_bucket_index(key);
  
  if (map->buckets[idx] == NULL) {
      map->buckets[idx] = list_create();
  }


  for (MapPair *pair = list_first(map->buckets[idx]); pair != NULL; pair = list_next(map->buckets[idx])) {
      if (_is_equal(map, pair, key)) return;
  }

  MapPair *newPair = (MapPair *)malloc(sizeof(MapPair));
  newPair->key = key;
  newPair->value = value;
  list_pushBack(map->buckets[idx], newPair);
}

void multimap_insert(Map *map, void *key, void *value) {
  unsigned long idx = get_bucket_index(key);
  
  if (map->buckets[idx] == NULL) {
      map->buckets[idx] = list_create();
  }

  MapPair *newPair = (MapPair *)malloc(sizeof(MapPair));
  newPair->key = key;
  newPair->value = value;
  
  if (map->lower_than) {
    current_lt = map->lower_than;
    list_sortedInsert(map->buckets[idx], newPair, pair_lt);
  } else {
    list_pushBack(map->buckets[idx], newPair);
  }
}


MapPair *map_search(Map *map, void *key) {
  unsigned long idx = get_bucket_index(key);
  if (map->buckets[idx] == NULL) return NULL; 


  for (MapPair *pair = list_first(map->buckets[idx]); pair != NULL; pair = list_next(map->buckets[idx])) {
    if (_is_equal(map, pair, key)) {
      return pair;
    }
  }
  return NULL;
}

MapPair *map_remove(Map *map, void *key) {
  unsigned long idx = get_bucket_index(key);
  if (map->buckets[idx] == NULL) return NULL;

  for (MapPair *pair = list_first(map->buckets[idx]); pair != NULL; pair = list_next(map->buckets[idx])) {
    if (_is_equal(map, pair, key)) {
      list_popCurrent(map->buckets[idx]);
      return pair;
    }
  }
  return NULL;
}

MapPair *map_first(Map *map) {
    for (int i = 0; i < MAP_CAPACITY; i++) {
        if (map->buckets[i] != NULL && list_first(map->buckets[i]) != NULL) {
            map->current_bucket = i;
            return list_first(map->buckets[i]);
        }
    }
    map->current_bucket = -1;
    return NULL;
}

MapPair *map_next(Map *map) {
    if (map->current_bucket == -1) return NULL;
    
    MapPair *nextPair = list_next(map->buckets[map->current_bucket]);
    if (nextPair != NULL) return nextPair;

   
    for (int i = map->current_bucket + 1; i < MAP_CAPACITY; i++) {
        if (map->buckets[i] != NULL && list_first(map->buckets[i]) != NULL) {
            map->current_bucket = i;
            return list_first(map->buckets[i]);
        }
    }
    
    map->current_bucket = -1;
    return NULL;
}

void map_clean(Map *map) {
    for (int i = 0; i < MAP_CAPACITY; i++) {
        if (map->buckets[i] != NULL) {
            list_clean(map->buckets[i]);
            free(map->buckets[i]);
        }
    }
    free(map->buckets);
    free(map);
}