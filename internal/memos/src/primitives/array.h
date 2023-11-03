#ifndef _ARRAY_H
#define _ARRAY_H

#include <stdlib.h>
#include <stdint.h>

#include "primitives/iterator.h"

#define ARRAY_INPLACE_SIZE 16

#define ARRAY_ALLOCATED(arr) (arr->capacity > ARRAY_INPLACE_SIZE)
#define ARRAY_DATA(arr) (ARRAY_ALLOCATED(arr) ? arr->items_ptr : arr->items_inplace)

typedef void *array_item_t;

struct _array {
    size_t size;
    size_t capacity;
    union {
        array_item_t *items_ptr;
        array_item_t items_inplace[ARRAY_INPLACE_SIZE];
    };
};

typedef struct _array *array_t;

static __always_inline size_t array_size(array_t arr) {
    if (arr == NULL) {
        return 0;
    }

    return arr->size;
}

static __always_inline array_item_t *array_data(array_t arr) {
    if (arr == NULL) {
        return NULL;
    }

    return ARRAY_DATA(arr);
}

static __always_inline iterator_t array_iterator(array_t arr) {
    if (arr == NULL) {
        return iterator_new(NULL, 0, 0);
    }

    return iterator_new(array_data(arr), array_size(arr), sizeof(array_item_t));
}

array_t array_new(size_t capacity);
void array_free(array_t);

void array_push(array_t, array_item_t);
array_item_t array_pop(array_t);

array_item_t array_get(array_t, size_t);
void array_set(array_t, size_t, array_item_t);

#endif /* _ARRAY_H */
