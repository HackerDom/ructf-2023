#include <csignal>

#include <glog/logging.h>

#include <server.hpp>

using namespace werk::server;
using namespace werk::utils;

std::shared_ptr<Server> server;

static void finishRequestedHandler(int) {
    if (server) {
        LOG(INFO) << "stopping server...";
        server->StopListen();
    }
}

int main(int argc, char **argv) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    FLAGS_logtostderr = true;
    FLAGS_minloglevel = 0;

    if (argc < 2) {
        LOG(ERROR) << "expected one positional argument: path to the unix socket to be bound to";
        return 1;
    }

    auto path = std::filesystem::path(std::string(argv[1]));

    auto threadPool = std::make_shared<ThreadPool>(5);
    server = std::make_shared<Server>(threadPool, path);

    std::signal(SIGCHLD, SIG_IGN);
    std::signal(SIGPIPE, SIG_IGN);
    std::signal(SIGINT, finishRequestedHandler);
    std::signal(SIGTERM, finishRequestedHandler);

    auto listenResult = server->Listen();
    if (!listenResult.success) {
        LOG(ERROR) << "failed to listen: " << listenResult.errorMessage;
        return 1;
    }

    return 0;
}
