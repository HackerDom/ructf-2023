#ifndef _UUID_H
#define _UUID_H

#include <stdbool.h>

#include "primitives/string.h"

string_t uuid_generate();
bool uuid_validate(string_t uuid);

#endif /* _UUID_H */
