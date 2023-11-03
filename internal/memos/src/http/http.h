#ifndef _HTTP_H
#define _HTTP_H

#include <stdbool.h>

#include "context/context.h"
#include "primitives/string.h"
#include "primitives/map.h"
#include "utils/reader.h"
#include "utils/writer.h"

enum HTTP_METHOD {
    HTTP_METHOD_UNDEFINED = 0,

    HTTP_METHOD_GET = 1,
    HTTP_METHOD_POST = 2,
};

enum HTTP_VERSION {
    HTTP_VERSION_UNDEFINED = 0,

    HTTP_VERSION_0_9 = 1,
    HTTP_VERSION_1_0 = 2,
    HTTP_VERSION_1_1 = 3,
};

enum HTTP_CODE {
    HTTP_CODE_UNDEFINED = 0,

    HTTP_CODE_200 = 1,
    HTTP_CODE_400 = 2,
    HTTP_CODE_403 = 3,
    HTTP_CODE_404 = 4,
    HTTP_CODE_500 = 5,
};

struct http_request {
    enum HTTP_VERSION version;
    enum HTTP_METHOD method;

    string_t query;
    map_t headers;
    string_t body;
};

struct http_response {
    enum HTTP_VERSION version;
    enum HTTP_CODE code;

    map_t headers;
    string_t body;

    string_t filename;
    size_t size;
};

bool http_read(reader_t, struct http_request *);
void http_write(writer_t, struct http_response *);

void http_process(context_t, struct http_request *, struct http_response *);

#endif /* _HTTP_H */
