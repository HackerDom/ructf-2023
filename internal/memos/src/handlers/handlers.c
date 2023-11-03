#include "handlers/handlers.h"

#include <stdio.h>

#include "utils/uuid.h"
#include "utils/filesystem.h"
#include "storage/storage.h"
#include "magick/magick.h"

const size_t max_list_count = 100;
const size_t max_labels_count = 4;

struct UploadImageOutput handle_upload_image(context_t ctx, struct UploadImageInput input) {
    context_push(ctx, (string_t)"UploadImage handler");

    struct UploadImageOutput output = {
        .uuid = NULL,
    };

    string_t image_uuid = uuid_generate();
    string_t image_path = path_combine(storage_images, image_uuid);

    file_write(ctx, image_path, input.data);
    string_free(image_path);

    if (ctx->failed) {
        string_free(image_uuid);

        return output;
    }

    output.uuid = image_uuid;

    context_pop(ctx);

    return output;
}

struct GetImageOutput handle_get_image(context_t ctx, struct GetImageInput input) {
    context_push(ctx, (string_t)"GetImage handler");

    struct GetImageOutput output = {
        .filename = NULL,
    };

    if (!uuid_validate(input.uuid)) {
        context_error(ctx, (string_t)"invalid uuid");

        return output;
    }

    string_t image_path = path_combine(storage_images, input.uuid);
    bool image_exists = file_exists(ctx, image_path);

    if (ctx->failed) {
        string_free(image_path);

        return output;
    }

    if (!image_exists) {
        string_free(image_path);

        context_not_found(ctx, (string_t)"image does not exist");

        return output;
    }

    string_t password_path = path_combine(storage_passwords, input.uuid);
    bool password_exists = file_exists(ctx, password_path);

    if (ctx->failed) {
        string_free(password_path);
        string_free(image_path);

        return output;
    }

    if (password_exists) {
        string_t password = file_read(ctx, password_path);
        string_free(password_path);

        if (ctx->failed) {
            string_free(password);
            string_free(image_path);

            return output;
        }

        bool password_equal = string_equal(password, input.password);
        string_free(password);

        if (!password_equal) {
            string_free(image_path);

            context_forbidden(ctx, (string_t)"invalid password");

            return output;
        }
    } else {
        string_free(password_path);
    }

    output.filename = image_path;

    context_pop(ctx);

    return output;
}

struct ListImagesOutput handle_list_images(context_t ctx, struct ListImagesInput input) {
    context_push(ctx, (string_t)"ListImages handler");

    struct ListImagesOutput output = {
        .uuids = NULL,
    };

    array_t image_uuids = directory_list(ctx, storage_images, max_list_count);

    if (ctx->failed) {
        array_free(image_uuids);

        return output;
    }

    output.uuids = image_uuids;

    context_pop(ctx);

    return output;
}

struct CreateDraftOutput handle_create_draft(context_t ctx, struct CreateDraftInput input) {
    context_push(ctx, (string_t)"CreateDraft handler");

    struct CreateDraftOutput output = {
        .session = NULL,
    };

    if (!uuid_validate(input.uuid)) {
        context_error(ctx, (string_t)"invalid uuid");

        return output;
    }

    string_t image_path = path_combine(storage_images, input.uuid);
    bool image_exists = file_exists(ctx, image_path);
    string_free(image_path);

    if (ctx->failed) {
        return output;
    }

    if (!image_exists) {
        context_not_found(ctx, (string_t)"image does not exist");

        return output;
    }

    string_t session = uuid_generate();

    string_t draft_path = path_combine(storage_drafts, session);
    file_write(ctx, draft_path, input.uuid);
    string_free(draft_path);

    if (ctx->failed) {
        string_free(session);

        return output;
    }

    output.session = session;

    context_pop(ctx);

    return output;
}

struct ViewDraftOutput handle_view_draft(context_t ctx, struct ViewDraftInput input) {
    context_push(ctx, (string_t)"ViewDraft handler");

    struct ViewDraftOutput output = {
        .filename = NULL,
    };

    if (!uuid_validate(input.session)) {
        context_error(ctx, (string_t)"invalid session");

        return output;
    }

    string_t draft_path = path_combine(storage_drafts, input.session);
    bool draft_exists = file_exists(ctx, draft_path);

    if (ctx->failed) {
        string_free(draft_path);

        return output;
    }

    if (!draft_exists) {
        string_free(draft_path);

        context_not_found(ctx, (string_t)"draft does not exist");

        return output;
    }

    string_t temp_path = path_combine(storage_temp, input.session);
    bool temp_exists = file_exists(ctx, temp_path);

    if (ctx->failed) {
        string_free(draft_path);
        string_free(temp_path);

        return output;
    }

    if (temp_exists) {
        output.filename = temp_path;

        string_free(draft_path);

        context_pop(ctx);

        return output;
    }
    
    string_free(temp_path);

    string_t image_uuid = file_read(ctx, draft_path);
    string_free(draft_path);

    if (ctx->failed) {
        string_free(image_uuid);

        return output;
    }

    string_t password_path = path_combine(storage_passwords, image_uuid);
    bool password_exists = file_exists(ctx, password_path);

    string_free(password_path);

    if (ctx->failed) {
        string_free(image_uuid);

        return output;
    }

    if (password_exists) {
        string_free(image_uuid);

        context_forbidden(ctx, (string_t)"image is private");

        return output;
    }

    string_t image_path = path_combine(storage_images, image_uuid);
    string_free(image_uuid);

    bool image_exists = file_exists(ctx, image_path);

    if (ctx->failed) {
        string_free(image_path);

        return output;
    }

    if (!image_exists) {
        string_free(image_path);

        context_internal(ctx, (string_t)"draft is expired");

        return output;
    }

    output.filename = image_path;

    context_pop(ctx);

    return output;
}

struct ChangeDraftBackgroundOutput handle_change_draft_background(context_t ctx, struct ChangeDraftBackgroundInput input) {
    context_push(ctx, (string_t)"UpdateDraftBackground handler");

    struct ChangeDraftBackgroundOutput output = {
    };

    if (!uuid_validate(input.session)) {
        context_error(ctx, (string_t)"invalid session");

        return output;
    }

    if (!uuid_validate(input.uuid)) {
        context_error(ctx, (string_t)"invalid uuid");

        return output;
    }

    string_t draft_path = path_combine(storage_drafts, input.session);
    bool draft_exists = file_exists(ctx, draft_path);

    if (ctx->failed) {
        string_free(draft_path);

        return output;
    }

    if (!draft_exists) {
        string_free(draft_path);

        context_not_found(ctx, (string_t)"draft does not exist");

        return output;
    }

    string_t image_path = path_combine(storage_images, input.uuid);
    bool image_exists = file_exists(ctx, image_path);
    string_free(image_path);

    if (ctx->failed) {
        string_free(draft_path);

        return output;
    }

    if (!image_exists) {
        string_free(draft_path);

        context_not_found(ctx, (string_t)"image does not exist");

        return output;
    }

    file_write(ctx, draft_path, input.uuid);
    string_free(draft_path);

    context_pop(ctx);

    return output;
}

struct UpdateDraftTextOutput handle_update_draft_text(context_t ctx, struct UpdateDraftTextInput input) {
    context_push(ctx, (string_t)"UpdateDraftText handler");

    struct UpdateDraftTextOutput output = {
    };

    if (!uuid_validate(input.session)) {
        context_error(ctx, (string_t)"invalid session");

        return output;
    }

    string_t draft_path = path_combine(storage_drafts, input.session);
    bool draft_exists = file_exists(ctx, draft_path);

    if (ctx->failed) {
        string_free(draft_path);

        return output;
    }

    if (!draft_exists) {
        string_free(draft_path);

        context_not_found(ctx, (string_t)"draft does not exist");

        return output;
    }

    string_t image_uuid = file_read(ctx, draft_path);
    string_free(draft_path);

    if (ctx->failed) {
        string_free(image_uuid);

        return output;
    }

    string_t image_path = path_combine(storage_images, image_uuid);
    bool image_exists = file_exists(ctx, image_path);
    string_free(image_path);

    if (ctx->failed) {
        string_free(image_uuid);

        return output;
    }

    if (!image_exists) {
        string_free(image_uuid);

        context_internal(ctx, (string_t)"draft is expired");

        return output;
    }

    string_t password_path = path_combine(storage_passwords, image_uuid);
    string_free(image_uuid);

    bool password_exists = file_exists(ctx, password_path);
    string_free(password_path);

    if (ctx->failed) {
        return output;
    }

    if (password_exists) {
        context_forbidden(ctx, (string_t)"image is private");

        return output;
    }

    size_t labels_count = array_size(input.labels);

    if (labels_count == 0) {
        context_error(ctx, (string_t)"expected at least one label");

        return output;
    }

    if (labels_count > max_labels_count) {
        context_error(ctx, (string_t)"too many labels");

        return output;
    }

    array_t label_uuids = array_new(labels_count);

    for (size_t i = 0; i < labels_count; i += 1) {
        struct Label *label = array_get(input.labels, i);

        string_t label_uuid = uuid_generate();

        magick_create_text(ctx, label_uuid, label->font, label->color, label->size, label->text);

        if (ctx->failed) {
            string_free(label_uuid);

            break;
        }

        array_push(label_uuids, label_uuid);
    }

    if (!ctx->failed) {
        array_t positions_x = array_new(labels_count);
        array_t positions_y = array_new(labels_count);

        for (iterator_t it = array_iterator(input.labels); !iterator_stopped(&it); iterator_next(&it)) {
            struct Label *label = *(struct Label **)(it.current);

            array_push(positions_x, label->position_x);
            array_push(positions_y, label->position_y);
        }

        magick_compose(ctx, input.session, label_uuids, positions_x, positions_y);

        array_free(positions_y);
        array_free(positions_x);
    }

    while (array_size(label_uuids) > 0) {
        string_free(array_pop(label_uuids));
    }

    array_free(label_uuids);

    if (!ctx->failed) {
        context_pop(ctx);
    }

    return output;
}

struct ReleaseDraftOutput handle_release_draft(context_t ctx, struct ReleaseDraftInput input) {
    context_push(ctx, (string_t)"ReleaseDraft handler");

    struct ReleaseDraftOutput output = {
        .uuid = NULL,
    };

    if (!uuid_validate(input.session)) {
        context_error(ctx, (string_t)"invalid session");

        return output;
    }

    string_t draft_path = path_combine(storage_drafts, input.session);
    bool draft_exists = file_exists(ctx, draft_path);

    if (ctx->failed) {
        string_free(draft_path);

        return output;
    }

    if (!draft_exists) {
        string_free(draft_path);

        context_not_found(ctx, (string_t)"draft does not exist");

        return output;
    }

    string_t image_uuid = uuid_generate();

    if (string_size(input.password) > 0) {
        string_t password_path = path_combine(storage_passwords, image_uuid);

        file_write(ctx, password_path, input.password);
        string_free(password_path);

        if (ctx->failed) {
            string_free(image_uuid);
            string_free(draft_path);

            return output;
        }
    }

    string_t temp_path = path_combine(storage_temp, input.session);
    string_t image_path = path_combine(storage_images, image_uuid);

    file_move(ctx, temp_path, image_path);
    string_free(image_path);
    string_free(temp_path);

    if (ctx->failed) {
        string_free(image_uuid);
        string_free(draft_path);

        return output;
    }

    file_delete(ctx, draft_path);
    string_free(draft_path);

    if (ctx->failed) {
        string_free(image_uuid);

        return output;
    }

    output.uuid = image_uuid;

    context_pop(ctx);

    return output;
}
