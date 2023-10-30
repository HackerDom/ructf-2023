#include <sys/random.h>

#include <extern_api.hpp>

extern "C" {
    ssize_t Translate(const char *text, void *out, size_t maxOut) {
        return getrandom(out, maxOut, 0);
    }
}