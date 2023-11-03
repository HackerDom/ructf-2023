#ifndef _READER_H
#define _READER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "primitives/string.h"

struct _reader {
    int fd;
    FILE *file;
};

typedef struct _reader *reader_t;

reader_t reader_new(int fd);
void reader_free(reader_t);

string_t reader_read(reader_t, size_t size);
string_t reader_readline(reader_t, size_t limit);

#endif /* _READER_H */
