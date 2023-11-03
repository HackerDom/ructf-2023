#include "utils/filesystem.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

const size_t chunk_size = 16 * 1024;

bool file_exists(context_t ctx, string_t filename) {
    int result = access(string_data(filename), F_OK);

    if (result < 0 && errno != ENOENT) {
        context_internal(ctx, string_from(strerror(errno)));

        return false;
    }

    return (result == 0);
}

size_t file_size(context_t ctx, string_t filename) {
    struct stat statbuf;
    int result = stat(string_data(filename), &statbuf);

    if (result < 0) {
        context_internal(ctx, string_from(strerror(errno)));

        return 0;
    }

    return (size_t)statbuf.st_size;
}

string_t file_read(context_t ctx, string_t filename) {
    size_t size = file_size(ctx, filename);

    if (ctx->failed) {
        return NULL;
    }

    if (size == 0) {
        return (string_t)"";
    }

    string_t content = string_new(size);

    if (content == NULL) {
        context_internal(ctx, (string_t)"failed to allocate string");

        return NULL;
    }

    char *content_ptr = string_data(content);

    FILE *file = fopen(string_data(filename), "r");

    if (file == NULL) {
        string_free(content);

        context_internal(ctx, (string_t)"failed to open file");

        return NULL;
    }

    while (size > 0) {
        size_t n = fread(content_ptr, sizeof(char), chunk_size, file);

        if (n == 0) {
            fclose(file);
            string_free(content);

            context_internal(ctx, (string_t)"failed to read file");

            return NULL;
        }

        content_ptr += n;
        size -= n;
    }

    fclose(file);

    return content;
}

void file_write(context_t ctx, string_t filename, string_t content) {
    FILE *file = fopen(string_data(filename), "w");

    if (file == NULL) {
        context_internal(ctx, (string_t)"failed to open file");

        return;
    }

    char *content_ptr = string_data(content);
    size_t size = string_size(content);

    while (size > 0) {
        size_t current_size = chunk_size;

        if (size < chunk_size) {
            current_size = size;
        }

        size_t n = fwrite(content_ptr, sizeof(char), current_size, file);

        if (n == 0) {
            fclose(file);

            context_internal(ctx, (string_t)"failed to write file");

            return;
        }

        content_ptr += n;
        size -= n;
    }

    fclose(file);

    return;
}

void file_delete(context_t ctx, string_t filename) {
    int result = unlink(string_data(filename));

    if (result < 0 && errno != ENOENT) {
        context_internal(ctx, string_from(strerror(errno)));

        return;
    }

    return;
}

void file_move(context_t ctx, string_t filename_old, string_t filename_new) {
    int result = rename(string_data(filename_old), string_data(filename_new));

    if (result < 0) {
        context_internal(ctx, string_from(strerror(errno)));

        return;
    }

    return;
}

bool directory_exists(context_t ctx, string_t directory) {
    struct stat statbuffer;

    int result = stat(string_data(directory), &statbuffer);

    if (result < 0 && errno != ENOENT) {
        context_internal(ctx, string_from(strerror(errno)));

        return false;
    }

    return (result == 0 && S_ISDIR(statbuffer.st_mode));
}

array_t directory_list(context_t ctx, string_t directory, size_t limit) {
    DIR *dir = opendir(string_data(directory));

    if (dir == NULL) {
        context_internal(ctx, (string_t)"failed to open directory");

        return NULL;
    }

    array_t result = array_new(limit);

    if (result == NULL) {
        closedir(dir);

        context_internal(ctx, (string_t)"failed to allocate array");

        return NULL;
    }

    for (size_t i = 0; i < limit; i += 1) {
        struct dirent *entry = readdir(dir);

        if (entry == NULL) {
            break;
        }

        if (entry->d_name[0] == '.' && (entry->d_name[1] == '\0' || (entry->d_name[1] == '.' && entry->d_name[2] == '\0'))) {
            continue;
        }

        string_t name = string_from(entry->d_name);
        array_push(result, name);
    }

    closedir(dir);

    return result;
}

string_t path_combine(string_t directory, string_t filename) {
    return string_concat(directory, (string_t)"/", filename, NULL);
}
