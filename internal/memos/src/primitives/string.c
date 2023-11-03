#include "primitives/string.h"

#include <stdarg.h>

extern void *memmem (const void *__haystack, size_t __haystacklen,
		     const void *__needle, size_t __needlelen);

string_t string_new(size_t size) {
    struct _string *str = (struct _string *)malloc(sizeof(struct _string));

    if (str == NULL) {
        return NULL;
    }

    str->magic = STRING_MAGIC | size;

    if (size > STRING_INPLACE_SIZE) {
        str->data_ptr = (char *)malloc((size + 1) * sizeof(char));

        if (str->data_ptr == NULL) {
            free(str);

            return NULL;
        }
    }

    memset(string_data(str), '\0', (string_size(str) + 1) * sizeof(char));

    return str;
}

void string_free(string_t str) {
    if (str == NULL) {
        return;
    }

    if (!IS_STRING(str)) {
        return;
    }

    if (STRING_ALLOCATED(str)) {
        free(str->data_ptr);
        str->data_ptr = NULL;
    }

    free(str);

    return;
}

string_t string_from(char *data) {
    return string_buffer(data, strlen(data));
}

string_t string_buffer(char *data, size_t size) {
    if (data == NULL) {
        return NULL;
    }

    if (size == 0) {
        return (string_t)"";
    }

    string_t str = string_new(size);

    if (str == NULL) {
        return NULL;
    }

    memcpy(string_data(str), data, size);

    return str;
}

string_t string_concat(string_t first, ...) {
    size_t first_size = string_size(first);

    va_list args;
    va_start(args, first);

    size_t items_count = 0;

    while (true) {
        string_t arg = va_arg(args, string_t);

        if (arg == NULL) {
            break;
        }

        items_count += 1;
    }

    va_end(args);

    size_t length = first_size;
    size_t items_sizes[items_count];

    va_start(args, first);

    for (size_t i = 0; i < items_count; i += 1) {
        string_t arg = va_arg(args, string_t);

        items_sizes[i] = string_size(arg);
        length += items_sizes[i];
    }

    va_end(args);

    string_t result = string_new(length);
    char *result_ptr = string_data(result);

    memcpy(result_ptr, string_data(first), first_size);
    result_ptr += first_size;

    va_start(args, first);

    for (size_t i = 0; i < items_count; i += 1) {
        string_t arg = va_arg(args, string_t);

        memcpy(result_ptr, string_data(arg), items_sizes[i]);
        result_ptr += items_sizes[i];
    }

    va_end(args);

    return result;
}

string_t string_join(string_t separator, array_t strings) {
    iterator_t it = array_iterator(strings);

    if (it.size == 0) {
        return (string_t)"";
    }

    size_t separator_size = string_size(separator);

    size_t items_sizes[it.size];
    size_t length = (it.size - 1) * separator_size;

    for (; !iterator_stopped(&it); iterator_next(&it)) {
        string_t item = *(string_t *)(it.current);

        items_sizes[it.index] = string_size(item);
        length += items_sizes[it.index];
    }

    string_t result = string_new(length);
    char *result_ptr = string_data(result);

    for (it = array_iterator(strings); !iterator_stopped(&it); iterator_next(&it)) {
        string_t item = *(string_t *)(it.current);

        memcpy(result_ptr, string_data(item), items_sizes[it.index]);
        result_ptr += items_sizes[it.index];

        if (it.index < it.size - 1) {
            memcpy(result_ptr, string_data(separator), separator_size);
            result_ptr += separator_size;
        }
    }

    return result;
}

array_t string_split(string_t str, string_t separator) {
    if (string_size(str) == 0 || string_size(separator) == 0) {
        return array_new(0);
    }

    char *data = string_data(str);
    size_t length = string_size(str);

    char *sep_data = string_data(separator);
    size_t sep_length = string_size(separator);

    array_t result = array_new(16);

    while (length > 0) {
        char *ptr = memmem(data, length, sep_data, sep_length);

        if (ptr == NULL) {
            string_t part = string_buffer(data, length);
            array_push(result, part);

            return result;
        }

        size_t size = (size_t)(ptr - data);

        string_t part = string_buffer(data, size);
        array_push(result, part);

        data = ptr + sep_length;
        length -= size + sep_length;
    }

    return result;
}

string_t string_substring(string_t str, size_t start, size_t length) {
    size_t size = string_size(str);

    if (start >= size) {
        return (string_t)"";
    }

    if (length > size - start) {
        length = size - start;
    }

    return string_buffer(string_data(str) + start, length);
}

bool string_equal(string_t str1, string_t str2) {
    if (str1 == NULL || str2 == NULL) {
        return (str1 == NULL && str2 == NULL);
    }

    size_t size1 = string_size(str1);
    size_t size2 = string_size(str2);

    if (size1 != size2) {
        return false;
    }

    char *ptr1 = string_data(str1);
    char *ptr2 = string_data(str2);

    return (memcmp(ptr1, ptr2, size1) == 0);
}

bool string_startswith(string_t str1, string_t str2) {
    if (str1 == NULL || str2 == NULL) {
        return false;
    }

    size_t size1 = string_size(str1);
    size_t size2 = string_size(str2);

    if (size2 > size1) {
        return false;
    }

    char *ptr1 = string_data(str1);
    char *ptr2 = string_data(str2);

    return (memcmp(ptr1, ptr2, size2) == 0);
}
