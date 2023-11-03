#ifndef _ITERATOR_H
#define _ITERATOR_H

#include <stdlib.h>
#include <stdbool.h>

typedef void *iterator_item_t;

struct _iterator {
    size_t size;
    size_t index;
    size_t element_size;
    iterator_item_t current;
};

typedef struct _iterator iterator_t;

static __always_inline iterator_t iterator_new(void *data, size_t size, size_t element_size) {
    struct _iterator it;

    it.size = size;
    it.element_size = element_size;
    it.index = 0;
    it.current = (iterator_item_t)data;

    return it;
}

static __always_inline bool iterator_stopped(iterator_t *it) {
    if (it == NULL) {
        return true;
    }

    return (it->index >= it->size);
}

static __always_inline void iterator_next(iterator_t *it) {
    if (it == NULL) {
        return;
    }

    if (iterator_stopped(it)) {
        return;
    }

    it->index += 1;

    char *ptr = (char *)(it->current);
    ptr += it->element_size;
    it->current = (iterator_item_t)ptr;

    return;
}

#endif /* _ITERATOR_H */
