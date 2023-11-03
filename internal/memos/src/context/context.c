#include "context/context.h"

#include <stdlib.h>

context_t context_new() {
    struct _context *ctx = (struct _context *)malloc(sizeof(struct _context));

    if (ctx == NULL) {
        return NULL;
    }

    ctx->failed = false;
    ctx->state = CONTEXT_STATE_OK;
    ctx->messages = array_new(32);
    ctx->fatal = false;

    if (ctx->messages == NULL) {
        free(ctx);

        return NULL;
    }

    return ctx;
}

void context_free(context_t ctx) {
    if (ctx == NULL) {
        return;
    }

    while (array_size(ctx->messages) > 0) {
        string_free(array_pop(ctx->messages));
    }

    array_free(ctx->messages);
    free(ctx);

    return;
}

void context_push(context_t ctx, string_t message) {
    array_push(ctx->messages, message);

    return;
}

void context_pop(context_t ctx) {
    string_free(array_pop(ctx->messages));

    return;
}

void context_clear(context_t ctx) {
    ctx->failed = false;
    ctx->state = CONTEXT_STATE_OK;
    ctx->fatal = false;

    while (array_size(ctx->messages) > 0) {
        string_free(array_pop(ctx->messages));
    }

    return;
}

void context_error(context_t ctx, string_t message) {
    ctx->failed = true;

    ctx->state = CONTEXT_STATE_ERROR;
    context_push(ctx, message);

    return;
}

void context_internal(context_t ctx, string_t message) {
    ctx->failed = true;

    ctx->state = CONTEXT_STATE_INTERNAL;
    context_push(ctx, message);
}

void context_not_found(context_t ctx, string_t message) {
    ctx->failed = true;

    ctx->state = CONTEXT_STATE_NOT_FOUND;
    context_push(ctx, message);

    return;
}

void context_forbidden(context_t ctx, string_t message) {
    ctx->failed = true;

    ctx->state = CONTEXT_STATE_FORBIDDEN;
    context_push(ctx, message);

    return;
}
