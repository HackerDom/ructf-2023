#include <glog/logging.h>

#include <pages_pool.hpp>

int main(int argc, char **argv) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    FLAGS_logtostderr = true;
    FLAGS_minloglevel = 0;

    if (argc < 2) {
        LOG(ERROR) << "expected one positional argument: path to the unix socket to be bound to";
        return 1;
    }

    auto path = argv[1];

    LOG(INFO) << "Starting server at path '" << path << "'...";

    werk::server::Page p{};
    LOG(INFO) << sizeof(p.memory);

    return 0;
}
