#include <glog/logging.h>

#include <server.hpp>

namespace werk::server {
    bool Server::Listen() {
        if (listening) {
            return false;
        }

        if (!listening.exchange(true)) {

        }
    }
}
