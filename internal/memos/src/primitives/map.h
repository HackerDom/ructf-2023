#ifndef _MAP_H
#define _MAP_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "primitives/array.h"
#include "primitives/string.h"
#include "primitives/iterator.h"

typedef string_t map_key_t;
typedef string_t map_value_t;

struct _map {
    size_t size;
    array_t keys;
    array_t values;
};

typedef struct _map *map_t;

static __always_inline size_t map_size(map_t map) {
    if (map == NULL) {
        return 0;
    }

    return map->size;
}

static __always_inline iterator_t map_iterator_keys(map_t map) {
    if (map == NULL) {
        return iterator_new(NULL, 0, 0);
    }

    return array_iterator(map->keys);
}

static __always_inline iterator_t map_iterator_values(map_t map) {
    if (map == NULL) {
        return iterator_new(NULL, 0, 0);
    }

    return array_iterator(map->values);
}

map_t map_new(size_t capacity);
void map_free(map_t);

void map_set(map_t, map_key_t, map_value_t);
map_value_t map_get(map_t, map_key_t);
void map_delete(map_t, map_key_t);
bool map_contains(map_t, map_key_t);

void map_clear(map_t);

#endif /* _MAP_H */
