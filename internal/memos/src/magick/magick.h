#ifndef _MAGICK_H
#define _MAGICK_H

#include "context/context.h"
#include "primitives/string.h"
#include "primitives/array.h"

void magick_create_text(context_t, string_t uuid, string_t font, string_t color, string_t size, string_t text);
void magick_compose(context_t, string_t uuid, array_t layer_uuids, array_t positions_x, array_t positions_y);

#endif /* _MAGICK_H */
