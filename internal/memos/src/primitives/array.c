#include "primitives/array.h"

#include <string.h>

array_t array_new(size_t capacity) {
    struct _array *arr = (struct _array *)malloc(sizeof(struct _array));

    if (arr == NULL) {
        return NULL;
    }

    arr->size = 0;
    arr->capacity = capacity;

    if (capacity > ARRAY_INPLACE_SIZE) {
        arr->items_ptr = (array_item_t *)malloc(capacity * sizeof(array_item_t));

        if (arr->items_ptr == NULL) {
            free(arr);

            return NULL;
        }
    }

    return arr;
}

void array_free(array_t arr) {
    if (arr == NULL) {
        return;
    }

    if (ARRAY_ALLOCATED(arr)) {
        free(arr->items_ptr);
        arr->items_ptr = NULL;
    }

    free(arr);

    return;
}

void array_push(array_t arr, array_item_t item) {
    if (arr->size == arr->capacity) {
        size_t new_capacity = arr->capacity * 2;

        if (ARRAY_ALLOCATED(arr)) {
            arr->items_ptr = (array_item_t *)realloc(arr->items_ptr, new_capacity * sizeof(array_item_t));
        } else if (new_capacity > ARRAY_INPLACE_SIZE) {
            array_item_t *items_ptr = (array_item_t *)malloc(new_capacity * sizeof(array_item_t));
            memcpy(items_ptr, arr->items_inplace, arr->size * sizeof(array_item_t));
            arr->items_ptr = items_ptr;
        }

        arr->capacity = new_capacity;
    }

    ARRAY_DATA(arr)[arr->size] = item;
    arr->size += 1;
}

array_item_t array_pop(array_t arr) {
    if (arr->size == 0) {
        return NULL;
    }

    array_item_t item = ARRAY_DATA(arr)[arr->size - 1];
    arr->size -= 1;

    return item;
}

array_item_t array_get(array_t arr, size_t index) {
    if (index > array_size(arr)) {
        return NULL;
    }

    array_item_t *items = array_data(arr);

    return items[index];
}

void array_set(array_t arr, size_t index, array_item_t item) {
    if (index > array_size(arr)) {
        return;
    }

    array_item_t *items = array_data(arr);

    items[index] = item;

    return;
}
