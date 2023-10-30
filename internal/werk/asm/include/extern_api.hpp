#pragma once

#include <sys/types.h>

extern "C" {
    ssize_t Translate(const char *text, void *out, size_t maxOut);
}
