#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include <stdlib.h>
#include <stdbool.h>

#include "context/context.h"
#include "primitives/string.h"
#include "primitives/array.h"

bool file_exists(context_t, string_t filename);
size_t file_size(context_t, string_t filename);
string_t file_read(context_t, string_t filename);
void file_write(context_t, string_t filename, string_t content);
void file_delete(context_t, string_t filename);
void file_move(context_t, string_t filename_old, string_t filename_new);

bool directory_exists(context_t, string_t directory);
array_t directory_list(context_t, string_t directory, size_t limit);

string_t path_combine(string_t directory, string_t filename);

#endif /* _FILESYSTEM_H */
