#include <glog/logging.h>

int main([[maybe_unused]] int argc, char **argv) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    FLAGS_logtostderr = true;
    FLAGS_minloglevel = 0;

    LOG(INFO) << "Hello from compiler!";

    return 0;
}
