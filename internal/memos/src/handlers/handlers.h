#ifndef _HANDLERS_H
#define _HANDLERS_H

#include "context/context.h"
#include "models/models.h"

struct UploadImageOutput handle_upload_image(context_t, struct UploadImageInput);
struct GetImageOutput handle_get_image(context_t, struct GetImageInput);
struct ListImagesOutput handle_list_images(context_t, struct ListImagesInput);

struct CreateDraftOutput handle_create_draft(context_t, struct CreateDraftInput);
struct ViewDraftOutput handle_view_draft(context_t, struct ViewDraftInput);
struct ChangeDraftBackgroundOutput handle_change_draft_background(context_t, struct ChangeDraftBackgroundInput);
struct UpdateDraftTextOutput handle_update_draft_text(context_t, struct UpdateDraftTextInput);
struct ReleaseDraftOutput handle_release_draft(context_t, struct ReleaseDraftInput);

#endif /* _HANDLERS_H */
