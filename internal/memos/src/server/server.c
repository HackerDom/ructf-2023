#include "server/server.h"

#include <stdbool.h>

#include "context/context.h"
#include "utils/reader.h"
#include "utils/writer.h"
#include "http/http.h"

const size_t default_headers_size = 8;

void server_serve(int in_fd, int out_fd) {
    context_t ctx = context_new();

    reader_t reader = reader_new(in_fd);
    writer_t writer = writer_new(out_fd);

    struct http_request request = {
        .body = NULL,
        .query = NULL,
        .headers = map_new(default_headers_size),
    };
    struct http_response response = {
        .body = NULL,
        .headers = map_new(default_headers_size),
        .filename = NULL,
        .size = 0,
    };

    while (true) {
        bool success = http_read(reader, &request);

        if (!success) {
            break;
        }

        http_process(ctx, &request, &response);
        http_write(writer, &response);

        map_clear(request.headers);

        string_free(request.query);
        request.query = NULL;

        string_free(request.body);
        request.body = NULL;

        if (ctx->fatal) {
            break;
        }

        context_clear(ctx);
    }

    map_free(response.headers);
    map_free(request.headers);

    writer_free(writer);
    reader_free(reader);

    context_free(ctx);

    return;
}
