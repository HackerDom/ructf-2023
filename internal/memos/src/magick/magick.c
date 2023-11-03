#include "magick/magick.h"

#include <unistd.h>
#include <sys/wait.h>

#include "utils/uuid.h"
#include "utils/filesystem.h"
#include "utils/common.h"
#include "storage/storage.h"

const string_t magick_executable = (string_t)"./magick/magick";
const string_t magick_directory_fonts = (string_t)"./magick/fonts";

const size_t magick_max_layers_count = 8;

const string_t fonts[] = {
    (string_t)"extralight",
    (string_t)"extralight-italic",
    (string_t)"light",
    (string_t)"light-italic",
    (string_t)"thin",
    (string_t)"thin-italic",
    (string_t)"regular",
    (string_t)"regular-italic",
    (string_t)"medium",
    (string_t)"medium-italic",
    (string_t)"semibold",
    (string_t)"semibold-italic",
    (string_t)"bold",
    (string_t)"bold-italic",
};

bool magick_validate_color(string_t color) {
    if (string_size(color) != 7) {
        return false;
    }

    char *ptr = string_data(color);

    if (ptr[0] != '#' || !common_is_hexdigit(ptr[1]) || !common_is_hexdigit(ptr[2]) || !common_is_hexdigit(ptr[3]) || !common_is_hexdigit(ptr[4]) || !common_is_hexdigit(ptr[5]) || !common_is_hexdigit(ptr[6])) {
        return false;
    }

    return true;
}

bool magick_validate_font(string_t font) {
    if (string_size(font) == 0) {
        return false;
    }

    for (size_t i = 0; i < 14; i += 1) {
        if (string_equal(font, fonts[i])) {
            return true;
        }
    }

    return false;
}

bool magick_validate_number(string_t number) {
    char *ptr = string_data(number);

    if (string_size(number) == 1 && common_is_digit(ptr[0]) && ptr[0] != '0') {
        return true;
    }
    
    if (string_size(number) == 2 && common_is_digit(ptr[0]) && common_is_digit(ptr[1]) && ptr[0] != '0') {
        return true;
    }
    
    if (string_size(number) == 3 && common_is_digit(ptr[0]) && common_is_digit(ptr[1]) && common_is_digit(ptr[2]) && ptr[0] != '0') {
        return true;
    }

    return false;
}

bool magick_validate_text(string_t text) {
    if (string_size(text) == 0) {
        return false;
    }

    for (iterator_t it = string_iterator(text); !iterator_stopped(&it); iterator_next(&it)) {
        char first = *(char *)(it.current);
        char second = *(char *)(it.current + 1);

        if (common_is_alphanumeric(first)) {
            continue;
        }

        if (common_is_russian_letter(first, second)) {
            iterator_next(&it);
            continue;
        }

        if (first == ' ' || first == '.' || first == ',' || first == '!' || first == '=') {
            continue;
        }

        return false;
    }

    return true;
}

void magick_validate(context_t ctx, string_t font, string_t color, string_t size, string_t text) {
    if (!magick_validate_font(font)) {
        context_error(ctx, (string_t)"unknown font");

        return;
    }

    if (!magick_validate_color(color)) {
        context_error(ctx, (string_t)"invalid color format");

        return;
    }

    if (!magick_validate_number(size)) {
        context_error(ctx, (string_t)"invalid size");

        return;
    }

    if (!magick_validate_text(text)) {
        context_error(ctx, (string_t)"invalid text");

        return;
    }

    return;
}

void magick_create_text(context_t ctx, string_t uuid, string_t font, string_t color, string_t size, string_t text) {
    context_push(ctx, (string_t)"magick");

    magick_validate(ctx, font, color, size, text);

    if (ctx->failed) {
        return;
    }

    string_t font_name = string_concat(font, (string_t)".ttf", NULL);
    string_t font_path = path_combine(magick_directory_fonts, font_name);
    string_free(font_name);

    string_t result_name = path_combine(storage_temp, uuid);
    string_t result_path = string_concat((string_t)"png:", result_name, NULL);
    string_free(result_name);

    string_t label = string_concat((string_t)"label:", text, NULL);

    int pid = fork();
    int status;

    if (pid == 0) {
        execl(
            (char *)magick_executable,
            (char *)magick_executable,
            "-background", "transparent",
            "-fill", string_data(color),
            "-font", string_data(font_path),
            "-pointsize", string_data(size),
            string_data(label),
            string_data(result_path),
            NULL
        );
    } else {
        waitpid(pid, &status, 0);
    }

    string_free(label);
    string_free(result_path);
    string_free(font_path);

    if (status != 0) {
        context_internal(ctx, (string_t)"failed to generate text");

        return;
    }

    context_pop(ctx);

    return;
}

void magick_compose(context_t ctx, string_t uuid, array_t layer_uuids, array_t positions_x, array_t positions_y) {
    context_push(ctx, (string_t)"magick");

    size_t layers_count = array_size(layer_uuids);

    if (array_size(positions_x) != layers_count || array_size(positions_y) != layers_count) {
        context_internal(ctx, (string_t)"different size of arrays");

        return;
    }

    if (layers_count > magick_max_layers_count) {
        context_error(ctx, (string_t)"too many layers");

        return;
    }

    for (size_t i = 0; i < layers_count; i += 1) {
        string_t position_x = (string_t)array_get(positions_x, i);
        string_t position_y = (string_t)array_get(positions_y, i);

        if (!magick_validate_number(position_x) || !magick_validate_number(position_y)) {
            context_error(ctx, (string_t)"invalid position");

            return;
        }
    }

    array_t layer_targets = array_new(layers_count);
    array_t geometries = array_new(layers_count);

    for (size_t i = 0; i < layers_count; i += 1) {
        string_t layer_uuid = (string_t)array_get(layer_uuids, i);
        string_t layer_path = path_combine(storage_temp, layer_uuid);

        if (!file_exists(ctx, layer_path)) {
            string_free(layer_path);

            continue;
        }

        string_t layer_target = string_concat((string_t)"png:", layer_path, NULL);
        array_push(layer_targets, layer_target);
        string_free(layer_path);

        string_t position_x = (string_t)array_get(positions_x, i);
        string_t position_y = (string_t)array_get(positions_y, i);

        string_t geometry = string_concat((string_t)"+", position_x, (string_t)"+", position_y, NULL);
        array_push(geometries, geometry);
    }

    layers_count = array_size(layer_targets);

    string_t result_path = path_combine(storage_temp, uuid);
    string_t result_target = string_concat((string_t)"jpg:", result_path, NULL);
    string_free(result_path);

    size_t argv_count = 1 + 1 + 4 * layers_count + 1 + 1;
    array_t argv = array_new(argv_count);

    string_t draft_path = path_combine(storage_drafts, uuid);
    string_t image_uuid = file_read(ctx, draft_path);
    string_free(draft_path);

    string_t image_path = path_combine(storage_images, image_uuid);
    string_free(image_uuid);

    string_t background = string_concat((string_t)"jpg:", image_path, NULL);
    string_free(image_path);

    array_push(argv, (char *)magick_executable);
    array_push(argv, string_data(background));

    for (size_t i = 0; i < layers_count; i += 1) {
        string_t layer_target = array_get(layer_targets, i);
        string_t geometry = array_get(geometries, i);

        array_push(argv, string_data(layer_target));
        array_push(argv, "-geometry");
        array_push(argv, string_data(geometry));
        array_push(argv, "-composite");
    }

    array_push(argv, string_data(result_target));
    array_push(argv, NULL);

    int pid = fork();
    int status;

    if (pid == 0) {
        execv((char *)magick_executable, (char **)array_data(argv));
    } else {
        waitpid(pid, &status, 0);
    }

    array_free(argv);

    string_free(result_target);

    for (size_t i = 0; i < layers_count; i += 1) {
        string_t geometry = array_get(geometries, i);
        string_t layer_target = array_get(layer_targets, i);

        string_free(geometry);
        string_free(layer_target);
    }

    array_free(geometries);
    array_free(layer_targets);

    string_free(background);

    if (status != 0) {
        context_internal(ctx, (string_t)"failed to compose images");

        return;
    }

    context_pop(ctx);

    return;
}
