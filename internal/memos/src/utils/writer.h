#ifndef _WRITER_H
#define _WRITER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "primitives/string.h"

struct _writer {
    int fd;
    FILE *file;
};

typedef struct _writer *writer_t;

writer_t writer_new(int fd);
void writer_free(writer_t);

void writer_write(writer_t, string_t data);
void writer_write_file(writer_t, string_t filename, size_t size);

#endif /* _WRITER_H */
