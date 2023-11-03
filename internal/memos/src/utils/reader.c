#include "utils/reader.h"

#include <unistd.h>
#include <string.h>

extern FILE *fdopen(int __fd, const char *__modes);

reader_t reader_new(int fd) {
    struct _reader *reader = (struct _reader *)malloc(sizeof(struct _reader));

    if (reader == NULL) {
        return NULL;
    }

    reader->fd = dup(fd);
    reader->file = fdopen(reader->fd, "r");

    if (reader->file == NULL) {
        close(reader->fd);
        free(reader);

        return NULL;
    }

    setvbuf(reader->file, NULL, _IOLBF, 4 * 4096);

    return reader;
}

void reader_free(reader_t reader) {
    if (reader == NULL) {
        return;
    }

    fclose(reader->file);
    reader->file = NULL;

    free(reader);

    return;
}

string_t reader_read(reader_t reader, size_t size) {
    char buffer[size];

    size_t length = fread(buffer, sizeof(char), size, reader->file);

    if (length == 0) {
        return (string_t)"";
    }

    return string_buffer(buffer, length);
}

string_t reader_readline(reader_t reader, size_t limit) {
    char buffer[limit + 1];

    char *ptr = fgets(buffer, limit, reader->file);

    if (ptr == NULL || ptr[0] == '\0' || ptr[0] == '\r' || ptr[0] == '\n') {
        return (string_t)"";
    }

    ptr = memchr(buffer, '\n', limit + 1);

    if (ptr > 0 && *(ptr - 1) == '\r') {
        ptr -= 1;
    }

    size_t length = (size_t)(ptr - buffer);

    return string_buffer(buffer, length);
}
