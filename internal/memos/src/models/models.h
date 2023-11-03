#ifndef _MODELS_H
#define _MODELS_H

#include "primitives/string.h"
#include "primitives/array.h"

struct UploadImageInput {
    string_t data;
};
struct UploadImageOutput {
    string_t uuid;
};

struct GetImageInput {
    string_t uuid;
    string_t password;
};
struct GetImageOutput {
    string_t filename;
};

struct ListImagesInput {
};
struct ListImagesOutput {
    array_t uuids;
};

struct CreateDraftInput {
    string_t uuid;
};
struct CreateDraftOutput {
    string_t session;
};

struct ViewDraftInput {
    string_t session;
};
struct ViewDraftOutput {
    string_t filename;
};

struct ChangeDraftBackgroundInput {
    string_t session;
    string_t uuid;
};
struct ChangeDraftBackgroundOutput {
};

struct UpdateDraftTextInput {
    string_t session;
    array_t labels;
};
struct UpdateDraftTextOutput {
};

struct ReleaseDraftInput {
    string_t session;
    string_t password;
};
struct ReleaseDraftOutput {
    string_t uuid;
};

struct Label {
    string_t font;
    string_t color;
    string_t position_x;
    string_t position_y;
    string_t size;
    string_t text;
};

#endif /* _MODELS_H */
