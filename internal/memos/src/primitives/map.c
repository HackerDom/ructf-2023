#include "primitives/map.h"

map_t map_new(size_t capacity) {
    struct _map *map = (struct _map *)malloc(sizeof(struct _map));

    if (map == NULL) {
        return NULL;
    }

    map->size = 0;

    map->keys = array_new(capacity);
    map->values = array_new(capacity);

    if (map->keys == NULL || map->values == NULL) {
        array_free(map->keys);
        array_free(map->values);
        free(map);

        return NULL;
    }

    return map;
}

void map_free(map_t map) {
    if (map == NULL) {
        return;
    }

    array_free(map->keys);
    map->keys = NULL;

    array_free(map->values);
    map->values = NULL;

    free(map);

    return;
}

void map_set(map_t map, map_key_t key, map_value_t value) {
    if (key == NULL) {
        return;
    }

    map->size += 1;

    for (iterator_t it = map_iterator_keys(map); !iterator_stopped(&it); iterator_next(&it)) {
        map_key_t current = *(map_key_t *)(it.current);

        if (current == NULL || string_equal(key, current)) {
            if (current == NULL) {
                array_set(map->keys, it.index, key);
            } else {
                string_t old_value = array_get(map->values, it.index);
                string_free(old_value);
            }

            array_set(map->values, it.index, value);

            return;
        }
    }

    array_push(map->keys, key);
    array_push(map->values, value);
}

map_value_t map_get(map_t map, map_key_t key) {
    if (key == NULL) {
        return NULL;
    }

    for (iterator_t it = map_iterator_keys(map); !iterator_stopped(&it); iterator_next(&it)) {
        map_key_t current = *(map_key_t *)(it.current);

        if (string_equal(key, current)) {
            return array_get(map->values, it.index);
        }
    }

    return NULL;
}

void map_delete(map_t map, map_key_t key) {
    if (key == NULL) {
        return;
    }

    for (iterator_t it = map_iterator_keys(map); !iterator_stopped(&it); iterator_next(&it)) {
        map_key_t current = *(map_key_t *)(it.current);

        if (string_equal(key, current)) {
            string_t old_key = array_get(map->keys, it.index);
            string_free(old_key);
            array_set(map->keys, it.index, NULL);

            string_t old_value = array_get(map->keys, it.index);
            string_free(old_value);
            array_set(map->values, it.index, NULL);

            map->size -= 1;

            return;
        }
    }

    return;
}

bool map_contains(map_t map, map_key_t key) {
    if (key == NULL) {
        return false;
    }

    for (iterator_t it = map_iterator_keys(map); !iterator_stopped(&it); iterator_next(&it)) {
        map_key_t current = *(map_key_t *)(it.current);

        if (string_equal(key, current)) {
            return true;
        }
    }

    return false;
}

void map_clear(map_t map) {
    for (size_t i = 0; i < map->size; i += 1) {
        string_free(array_get(map->keys, i));
        array_set(map->keys, i, NULL);

        string_free(array_get(map->values, i));
        array_set(map->values, i, NULL);

        map->size -= 1;
    }

    return;
}
