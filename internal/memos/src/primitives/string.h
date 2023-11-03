#ifndef _MY_STRING_H
#define _MY_STRING_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "primitives/array.h"
#include "primitives/iterator.h"

#define STRING_MAGIC 0xbbcfb24300000000
#define STRING_INPLACE_SIZE 63

#define IS_STRING(str) ((str->magic & 0xFFFFFFFF00000000ULL) == STRING_MAGIC)
#define STRING_SIZE(str) (str->magic & 0x00000000FFFFFFFFULL)
#define STRING_ALLOCATED(str) (STRING_SIZE(str) > STRING_INPLACE_SIZE)
#define STRING_DATA(str) (STRING_ALLOCATED(str) ? str->data_ptr : str->data_inplace)

struct _string {
    uint64_t magic;
    union {
        char *data_ptr;
        char data_inplace[STRING_INPLACE_SIZE + 1];
    };
};

typedef struct _string *string_t;

static __always_inline size_t string_size(string_t str) {
    if (str == NULL) {
        return 0;
    }

    if (IS_STRING(str)) {
        return STRING_SIZE(str);
    }

    return strlen((char *)str);
}

static __always_inline char *string_data(string_t str) {
    if (str == NULL) {
        return NULL;
    }

    if (IS_STRING(str)) {
        return STRING_DATA(str);
    }

    return (char *)str;
}

static __always_inline iterator_t string_iterator(string_t str) {
    if (str == NULL) {
        return iterator_new(NULL, 0, 0);
    }

    return iterator_new(string_data(str), string_size(str), sizeof(char));
}

string_t string_new(size_t size);
void string_free(string_t);

string_t string_from(char *);
string_t string_buffer(char *, size_t);
string_t string_concat(string_t, ...);
string_t string_join(string_t separator, array_t strings);
array_t string_split(string_t, string_t separator);
string_t string_substring(string_t, size_t start, size_t length);

bool string_equal(string_t, string_t);
bool string_startswith(string_t, string_t);

#endif /* _MY_STRING_H */
