#include "utils/uuid.h"

#include <stdint.h>
#include <sys/random.h>

#include "utils/common.h"

const char hexdigits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
const size_t groups[] = { 8, 4, 4, 4, 12 };

const size_t uuid_length = 36;

string_t uuid_generate() {
    uint8_t buffer[16];
    getrandom(buffer, 16, 0);

    string_t uuid = string_new(uuid_length);

    if (uuid == NULL) {
        return NULL;
    }

    iterator_t it = string_iterator(uuid);

    size_t index = 0;

    for (size_t i = 0; i < 5; i += 1) {
        for (size_t j = 0; j < groups[i]; j += 2) {
            uint8_t byte = buffer[index];
            index += 1;

            *(char *)(it.current) = hexdigits[byte >> 4];
            iterator_next(&it);

            *(char *)(it.current) = hexdigits[byte & 0xf];
            iterator_next(&it);
        }

        if (i < 4) {
            *(char *)(it.current) = '-';
            iterator_next(&it);
        }
    }

    return uuid;
}

bool uuid_validate(string_t uuid) {
    if (uuid == NULL) {
        return false;
    }

    if (string_size(uuid) != uuid_length) {
        return false;
    }

    iterator_t it = string_iterator(uuid);

    for (size_t i = 0; i < 5; i += 1) {
        size_t group = groups[i];

        for (size_t j = 0; j < group; j += 1) {
            char symbol = *(char *)(it.current);
            iterator_next(&it);

            if (!common_is_hexdigit(symbol)) {
                return false;
            }
        }

        if (i < 4 && *(char *)(it.current) != '-') {
            return false;
        }

        iterator_next(&it);
    }

    return true;
}
