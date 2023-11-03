#include "utils/writer.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <errno.h>

#include "utils/filesystem.h"

extern FILE *fdopen(int __fd, const char *__modes);

writer_t writer_new(int fd) {
    struct _writer *writer = (struct _writer *)malloc(sizeof(struct _writer));

    if (writer == NULL) {
        return NULL;
    }

    writer->fd = dup(fd);
    writer->file = fdopen(writer->fd, "w");

    if (writer->file == NULL) {
        close(writer->fd);
        free(writer);

        return NULL;
    }

    setvbuf(writer->file, NULL, _IONBF, 0);

    return writer;
}

void writer_free(writer_t writer) {
    if (writer == NULL) {
        return;
    }

    fclose(writer->file);
    writer->file = NULL;

    free(writer);

    return;
}

void writer_write(writer_t writer, string_t str) {
    size_t size = string_size(str);

    if (size == 0) {
        return;
    }

    fwrite(string_data(str), sizeof(char), size, writer->file);

    return;
}

void writer_write_file(writer_t writer, string_t filename, size_t size) {
    int fd = open(string_data(filename), O_RDONLY);

    if (fd < 0) {
        return;
    }

    sendfile(writer->fd, fd, NULL, size);
    close(fd);

    return;
}
