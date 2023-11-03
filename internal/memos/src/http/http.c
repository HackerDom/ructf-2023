#include "http/http.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "models/models.h"
#include "handlers/handlers.h"
#include "utils/common.h"
#include "utils/filesystem.h"

const size_t max_body_length = 512 * 1024;
const size_t max_query_length = 1024;
const size_t max_header_length = 1024;
const size_t max_headers_count = 32;

const string_t methods[] = {
    (string_t)"UNDEFINED", /* HTTP_METHOD_UNDEFINED */
    (string_t)"GET",       /* HTTP_METHOD_GET */
    (string_t)"POST",      /* HTTP_METHOD_POST */
};

const string_t versions[] = {
    (string_t)"HTTP/x.x", /* HTTP_VERSION_UNDEFINED */
    (string_t)"HTTP/0.9", /* HTTP_VERSION_0_9 */
    (string_t)"HTTP/1.0", /* HTTP_VERSION_1_0 */
    (string_t)"HTTP/1.1", /* HTTP_VERSION_1_1 */
};

const string_t codes[] = {
    (string_t)"000 Undefined",             /* HTTP_CODE_UNDEFINED */
    (string_t)"200 OK",                    /* HTTP_CODE_200 */
    (string_t)"400 Bad Request",           /* HTTP_CODE_400 */
    (string_t)"403 Forbidden",             /* HTTP_CODE_403 */
    (string_t)"404 Not Found",             /* HTTP_CODE_404 */
    (string_t)"500 Internal Server Error", /* HTTP_CODE_500 */
};

const string_t server_name = (string_t)"memos/0.0.1";

const string_t header_content_type = (string_t)"content-type";
const string_t header_content_length = (string_t)"content-length";
const string_t header_cache_control = (string_t)"cache-control";
const string_t header_expires = (string_t)"expires";

const string_t content_type_html = (string_t)"text/html";
const string_t content_type_plain = (string_t)"text/plain";
const string_t content_type_image = (string_t)"image/jpeg";

const string_t default_cache_control = (string_t)"public, immutable, max-age=3600";
const string_t default_expires = (string_t)"Mon, 01 Jan 2024 00:00:01 GMT";

void http_middleware(context_t ctx, struct http_response *res) {
    if (!ctx->failed) {
        return;
    }

    res->code = HTTP_CODE_400;
    res->version = HTTP_VERSION_1_1;

    string_free(res->body);
    res->body = string_join((string_t)": ", ctx->messages);

    switch (ctx->state) {
        case CONTEXT_STATE_ERROR:
            res->code = HTTP_CODE_400;
            break;

        case CONTEXT_STATE_INTERNAL:
            res->code = HTTP_CODE_500;
            break;

        case CONTEXT_STATE_NOT_FOUND:
            res->code = HTTP_CODE_404;
            break;

        case CONTEXT_STATE_FORBIDDEN:
            res->code = HTTP_CODE_403;
            break;
    }

    return;
}

string_t http_current_date() {
    char buffer[128];

    time_t now = time(NULL);
    struct tm *tm = gmtime(&now);

    size_t length = strftime(buffer, 128, "%a, %d %b %Y %H:%M:%S %Z", tm);

    return string_buffer(buffer, length);
}

void http_parse_query(string_t query, struct http_request *req) {
    req->query = NULL;
    req->method = HTTP_METHOD_UNDEFINED;
    req->version = HTTP_VERSION_UNDEFINED;

    size_t query_length = string_size(query);

    if (query_length == 0) {
        return;
    }

    char *query_ptr = string_data(query);
    char *end_ptr = string_data(query) + query_length - sizeof(uint64_t);

    uint32_t method = *(uint32_t *)query_ptr;
    uint64_t version = *(uint64_t *)end_ptr;

    query_length -= sizeof(uint64_t);

    switch (method) {
        case 0x20544547: /* "GET " */
            req->method = HTTP_METHOD_GET;
            query_ptr += 4;
            query_length -= 4;
            break;

        case 0x54534f50: /* "POST" */
            req->method = HTTP_METHOD_POST;
            query_ptr += 5;
            query_length -= 5;
            break;
    }

    switch (version) {
        case 0x392e302f50545448: /* "HTTP/0.9" */
            req->version = HTTP_VERSION_0_9;
            break;

        case 0x302e312f50545448: /* "HTTP/1.0" */
            req->version = HTTP_VERSION_1_0;
            break;

        case 0x312e312f50545448: /* "HTTP/1.1" */
            req->version = HTTP_VERSION_1_1;
            break;
    }

    query_length -= 1;

    if (query_length <= max_query_length) {
        req->query = string_buffer(query_ptr, query_length);
    } else {
        req->query = (string_t)"";
    }

    return;
}

void http_parse_header(string_t header, struct http_request *req) {
    size_t header_length = string_size(header);

    if (header_length == 0) {
        return;
    }

    char *header_ptr = string_data(header);
    char *delimiter = strstr(header_ptr, ": ");

    if (delimiter == NULL) {
        return;
    }

    size_t key_size = (size_t)(delimiter - header_ptr);
    char key_buffer[key_size];

    for (size_t i = 0; i < key_size; i += 1) {
        key_buffer[i] = common_to_lower(*header_ptr);
        header_ptr += 1;
    }

    string_t key = string_buffer(key_buffer, key_size);
    string_t value = string_buffer(delimiter + 2, header_length - key_size - 2);

    if (string_size(key) == 0 || string_size(value) == 0) {
        string_free(key);
        string_free(value);

        return;
    }

    map_set(req->headers, key, value);

    return;
}

bool http_read(reader_t reader, struct http_request *req) {
    string_t query = reader_readline(reader, max_query_length);

    if (string_size(query) == 0) {
        string_free(query);

        return false;
    }

    http_parse_query(query, req);
    string_free(query);

    if (req->method == HTTP_METHOD_UNDEFINED || req->version == HTTP_VERSION_UNDEFINED) {
        return true;
    }

    req->body = NULL;

    while (true) {
        if (map_size(req->headers) >= max_headers_count) {
            break;
        }

        string_t header = reader_readline(reader, max_header_length);

        if (string_size(header) == 0) {
            break;
        }

        http_parse_header(header, req);
        string_free(header);
    }

    if (req->method == HTTP_METHOD_POST) {
        string_t content_length = map_get(req->headers, header_content_length);

        if (content_length == NULL) {
            return true;
        }

        size_t length = strtoul(string_data(content_length), NULL, 10);

        if (length > max_body_length) {
            length = max_body_length;
        }

        req->body = reader_read(reader, length);
    }

    return true;
}

void http_write(writer_t writer, struct http_response *res) {
    writer_write(writer, versions[res->version]);
    writer_write(writer, (string_t)" ");
    writer_write(writer, codes[res->code]);
    writer_write(writer, (string_t)"\n");

    map_set(res->headers, (string_t)"date", http_current_date());
    map_set(res->headers, (string_t)"server", server_name);

    size_t content_length = 0;

    if (res->body != NULL) {
        content_length = string_size(res->body);
    } else if (res->filename != NULL) {
        content_length = res->size;
    }

    if (content_length > 0) {
        char buffer[16];
        sprintf(buffer, "%lu", content_length);

        string_t content_length = string_from(buffer);
        map_set(res->headers, header_content_length, content_length);
    } else {
        map_set(res->headers, header_content_length, (string_t)"0");
    }

    iterator_t keys = map_iterator_keys(res->headers);
    iterator_t values = map_iterator_values(res->headers);

    while (!iterator_stopped(&keys) && !iterator_stopped(&values)) {
        string_t key = *(string_t *)(keys.current);
        string_t value = *(string_t *)(values.current);

        if (key != NULL && value != NULL) {
            writer_write(writer, key);
            writer_write(writer, (string_t)": ");
            writer_write(writer, value);
            writer_write(writer, (string_t)"\n");
        }

        string_free(key);
        *(string_t *)(keys.current) = NULL;

        string_free(value);
        *(string_t *)(values.current) = NULL;

        iterator_next(&keys);
        iterator_next(&values);
    }

    res->headers->size = 0;

    writer_write(writer, (string_t)"\n");

    if (content_length > 0) {
        if (res->body != NULL) {
            writer_write(writer, res->body);

            string_free(res->body);
            res->body = NULL;
        } else if (res->filename != NULL) {
            writer_write_file(writer, res->filename, res->size);

            string_free(res->filename);
            res->filename = NULL;
            res->size = 0;
        }
    }

    return;
}

void http_validate(context_t ctx, struct http_request *req) {
    if (req->method == HTTP_METHOD_UNDEFINED) {
        ctx->fatal = true;
        context_error(ctx, (string_t)"unsupported method");

        return;
    }
    
    if (req->version == HTTP_VERSION_UNDEFINED) {
        ctx->fatal = true;
        context_error(ctx, (string_t)"unsupported version");

        return;
    }

    if (!map_contains(req->headers, (string_t)"host")) {
        context_error(ctx, (string_t)"host header is empty");

        return;
    }

    if (req->body != NULL && req->method != HTTP_METHOD_POST) {
        context_error(ctx, (string_t)"unexpected body");

        return;
    }

    if (req->query == NULL || string_size(req->query) == 0) {
        context_error(ctx, (string_t)"empty query string");

        return;
    }

    return;
}

void http_index(context_t ctx, struct http_request *req, struct http_response *res) {
    /* GET / */

    string_t filename = (string_t)"index.html";

    size_t size = file_size(ctx, filename);

    if (!ctx->failed) {
        res->code = HTTP_CODE_200;
        res->body = NULL;

        map_set(res->headers, header_content_type, content_type_html);
        map_set(res->headers, header_cache_control, default_cache_control);
        map_set(res->headers, header_expires, default_expires);

        res->filename = filename;
        res->size = size;
    }

    return;
}

void http_upload_image(context_t ctx, struct http_request *req, struct http_response *res) {
    /* POST /images/upload */

    string_t content_type = map_get(req->headers, header_content_type);

    if (content_type != NULL && !string_equal(content_type, content_type_image)) {
        context_error(ctx, (string_t)"unsupported image type");

        return;
    }

    struct UploadImageInput input;

    input.data = req->body;

    struct UploadImageOutput output = handle_upload_image(ctx, input);

    if (!ctx->failed) {
        res->code = HTTP_CODE_200;
        res->body = output.uuid;

        map_set(res->headers, header_content_type, content_type_plain);
    } else {
        string_free(output.uuid);
    }

    return;
}

void http_get_image(context_t ctx, struct http_request *req, struct http_response *res) {
    /* GET /images/get/<uuid>[?<password>] */

    struct GetImageInput input;

    string_t payload = string_substring(req->query, 12, string_size(req->query) - 12);
    array_t parts = string_split(payload, (string_t)"?");
    string_free(payload);

    if (array_size(parts) == 0) {
        array_free(parts);

        context_error(ctx, (string_t)"invalid query");

        return;
    }

    input.uuid = array_get(parts, 0);

    if (array_size(parts) == 1) {
        input.password = NULL;
        array_free(parts);
    } else if (array_size(parts) == 2) {
        input.password = array_get(parts, 1);
        array_free(parts);
    } else {
        while (array_size(parts) > 0) {
            string_free(array_pop(parts));
        }

        array_free(parts);

        context_error(ctx, (string_t)"invalid query");

        return;
    }

    struct GetImageOutput output = handle_get_image(ctx, input);

    string_free(input.uuid);

    if (!ctx->failed) {
        size_t size = file_size(ctx, output.filename);

        if (!ctx->failed) {
            res->code = HTTP_CODE_200;
            res->body = NULL;

            map_set(res->headers, header_content_type, content_type_image);
            map_set(res->headers, header_cache_control, default_cache_control);
            map_set(res->headers, header_expires, default_expires);

            res->filename = output.filename;
            res->size = size;
        } else {
            string_free(output.filename);
        }
    } else {
        string_free(output.filename);
    }

    return;
}

void http_list_images(context_t ctx, struct http_request *req, struct http_response *res) {
    /* GET /images/list */

    struct ListImagesInput input;
    struct ListImagesOutput output = handle_list_images(ctx, input);

    if (!ctx->failed) {
        res->code = HTTP_CODE_200;
        res->body = string_join((string_t)", ", output.uuids);

        map_set(res->headers, header_content_type, content_type_plain);
        map_set(res->headers, header_cache_control, (string_t)"no-store");
    }

    while (array_size(output.uuids) > 0) {
        string_free(array_pop(output.uuids));
    }

    array_free(output.uuids);

    return;
}

void http_create_draft(context_t ctx, struct http_request *req, struct http_response *res) {
    /* POST /drafts/create */

    struct CreateDraftInput input;

    input.uuid = req->body;

    struct CreateDraftOutput output = handle_create_draft(ctx, input);

    if (!ctx->failed) {
        res->code = HTTP_CODE_200;
        res->body = output.session;

        map_set(res->headers, header_content_type, content_type_plain);
    } else {
        string_free(output.session);
    }

    return;
}

void http_view_draft(context_t ctx, struct http_request *req, struct http_response *res) {
    /* GET /drafts/view/<session> */

    struct ViewDraftInput input;

    input.session = string_substring(req->query, 13, string_size(req->query) - 13);

    struct ViewDraftOutput output = handle_view_draft(ctx, input);

    string_free(input.session);

    if (!ctx->failed) {
        size_t size = file_size(ctx, output.filename);

        if (!ctx->failed) {
            res->code = HTTP_CODE_200;
            res->body = NULL;

            map_set(res->headers, header_content_type, content_type_image);
            map_set(res->headers, header_cache_control, (string_t)"no-store");

            res->filename = output.filename;
            res->size = size;
        } else {
            string_free(output.filename);
        }
    } else {
        string_free(output.filename);
    }

    return;
}

void http_change_draft_background(context_t ctx, struct http_request *req, struct http_response *res) {
    /* POST /drafts/background/<session> */

    struct ChangeDraftBackgroundInput input;

    input.session = string_substring(req->query, 19, string_size(req->query) - 19);
    input.uuid = req->body;

    struct ChangeDraftBackgroundOutput output = handle_change_draft_background(ctx, input);

    string_free(input.session);

    if (!ctx->failed) {
        res->code = HTTP_CODE_200;
        res->body = NULL;
    }

    return;
}

void http_update_draft_text(context_t ctx, struct http_request *req, struct http_response *res) {
    /* POST /drafts/text/<session> */

    struct UpdateDraftTextInput input;

    input.session = string_substring(req->query, 13, string_size(req->query) - 13);

    array_t large_parts = string_split(req->body, (string_t)"||");

    if (array_size(large_parts) == 0) {
        array_free(large_parts);
        string_free(input.session);

        context_error(ctx, (string_t)"failed to parse labels");

        return;
    }

    input.labels = array_new(8);

    for (size_t i = 0; i < array_size(large_parts); i += 1) {
        array_t small_parts = string_split(array_get(large_parts, i), (string_t)"|");

        if (array_size(small_parts) == 6) {
            struct Label *label = (struct Label *)malloc(sizeof(struct Label));

            if (label == NULL) {
                continue;
            }

            label->font = array_get(small_parts, 0);
            label->color = array_get(small_parts, 1);
            label->position_x = array_get(small_parts, 2);
            label->position_y = array_get(small_parts, 3);
            label->size = array_get(small_parts, 4);
            label->text = array_get(small_parts, 5);
            
            array_push(input.labels, label);
        } else {
            while (array_size(small_parts) > 0) {
                string_free(array_pop(small_parts));
            }
        }

        array_free(small_parts);
    }

    while (array_size(large_parts) > 0) {
        string_free(array_pop(large_parts));
    }

    array_free(large_parts);

    struct UpdateDraftTextOutput output = handle_update_draft_text(ctx, input);

    while (array_size(input.labels) > 0) {
        struct Label *label = array_pop(input.labels);

        string_free(label->font);
        string_free(label->color);
        string_free(label->position_x);
        string_free(label->position_y);
        string_free(label->size);
        string_free(label->text);

        free(label);
    }

    array_free(input.labels);
    string_free(input.session);

    if (!ctx->failed) {
        res->code = HTTP_CODE_200;
        res->body = NULL;
    }

    return;
}

void http_release_draft(context_t ctx, struct http_request *req, struct http_response *res) {
    /* POST /drafts/release/<session> */

    struct ReleaseDraftInput input;

    input.session = string_substring(req->query, 16, string_size(req->query) - 16);
    input.password = req->body;

    struct ReleaseDraftOutput output = handle_release_draft(ctx, input);

    string_free(input.session);

    if (!ctx->failed) {
        res->code = HTTP_CODE_200;
        res->body = output.uuid;

        map_set(res->headers, header_content_type, content_type_plain);
    } else {
        string_free(output.uuid);
    }

    return;
}

void http_process(context_t ctx, struct http_request *req, struct http_response *res) {
    context_push(ctx, (string_t)"HTTP");

    res->code = HTTP_CODE_404;
    res->version = req->version;
    res->body = (string_t)"not found";

    http_validate(ctx, req);

    if (ctx->failed) {
        goto finish;
    }

    switch (req->method) {
        case HTTP_METHOD_GET:
            if (string_equal(req->query, (string_t)"/")) {
                http_index(ctx, req, res);
                break;
            }
            
            if (string_startswith(req->query, (string_t)"/images/get/")) {
                http_get_image(ctx, req, res);
                break;
            }

            if (string_equal(req->query, (string_t)"/images/list")) {
                http_list_images(ctx, req, res);
                break;
            }

            if (string_startswith(req->query, (string_t)"/drafts/view/")) {
                http_view_draft(ctx, req, res);
                break;
            }

            break;

        case HTTP_METHOD_POST:
            if (string_equal(req->query, (string_t)"/images/upload")) {
                http_upload_image(ctx, req, res);
                break;
            }

            if (string_equal(req->query, (string_t)"/drafts/create")) {
                http_create_draft(ctx, req, res);
                break;
            }

            if (string_startswith(req->query, (string_t)"/drafts/background/")) {
                http_change_draft_background(ctx, req, res);
                break;
            }

            if (string_startswith(req->query, (string_t)"/drafts/text/")) {
                http_update_draft_text(ctx, req, res);
                break;
            }
            
            if (string_startswith(req->query, (string_t)"/drafts/release/")) {
                http_release_draft(ctx, req, res);
                break;
            }

            break;
    }

finish:
    http_middleware(ctx, res);

    context_pop(ctx);

    return;
}
