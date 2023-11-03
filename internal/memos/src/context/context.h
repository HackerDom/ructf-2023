#ifndef _CONTEXT_H
#define _CONTEXT_H

#include <stdbool.h>

#include "primitives/array.h"
#include "primitives/string.h"

enum CONTEXT_STATE {
    CONTEXT_STATE_OK = 0,

    CONTEXT_STATE_ERROR = 1,
    CONTEXT_STATE_INTERNAL = 2,
    CONTEXT_STATE_NOT_FOUND = 3,
    CONTEXT_STATE_FORBIDDEN = 4,
};

struct _context {
    bool failed;
    enum CONTEXT_STATE state;
    array_t messages;
    bool fatal;
};

typedef struct _context *context_t;

context_t context_new();
void context_free(context_t);

void context_push(context_t, string_t);
void context_pop(context_t);
void context_clear(context_t);

void context_error(context_t, string_t);
void context_internal(context_t, string_t);
void context_not_found(context_t, string_t);
void context_forbidden(context_t, string_t);

#endif /* _CONTEXT_H */
