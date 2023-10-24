#include <sys/types.h>
#include <sys/socket.h>

#include <utils/strings.hpp>

#include <models.hpp>

namespace werk::server {
    std::string RunRequest::String() const {
        return utils::Format("RunRequest(binaryPath='%s', serialOutPath='%s')",
                             binaryPath.c_str(),
                             serialOutPath.c_str()
        );
    }

    std::string RunResponse::String() const {
        return utils::Format("RunResponse(success=%d, vd=%lx, errorMessage='%s')",
                             success, vd, errorMessage.c_str()
        );
    }

    std::pair<std::shared_ptr<RunRequest>, std::string> RunRequest::ReadFromSocket(int fd) {
        struct {
            uint16_t binaryPathLen;
            uint16_t serialOutPathLen;
        } header{};

        auto nrecv = recv(fd, &header, sizeof(header), MSG_WAITALL);
        if (nrecv <= 0 || nrecv != sizeof(header)) {
            return std::make_pair(nullptr, utils::PError("header recv"));
        }

        std::string binaryPath;
        binaryPath.resize(header.binaryPathLen);
        nrecv = recv(fd, binaryPath.data(), binaryPath.size(), MSG_WAITALL);
        if (nrecv <= 0 || static_cast<std::size_t>(nrecv) != binaryPath.size()) {
            return std::make_pair(nullptr, utils::PError("binary path recv"));
        }

        std::string serialOutPath;
        serialOutPath.resize(header.serialOutPathLen);
        nrecv = recv(fd, serialOutPath.data(), serialOutPath.size(), MSG_WAITALL);
        if (nrecv <= 0 || static_cast<std::size_t>(nrecv) != serialOutPath.size()) {
            return std::make_pair(nullptr, utils::PError("serial out pah recv"));
        }

        auto req = std::make_shared<RunRequest>(std::move(binaryPath), std::move(serialOutPath));
        return std::make_pair(req, "");
    }

    int RunResponse::WriteToSocket(int fd) {
        struct {
            uint8_t success;
            uint64_t vd;
            uint64_t errorMessageLen;
        } header{};
        static_assert(sizeof(header.vd) == sizeof(vd_t));
        static_assert(sizeof(header.errorMessageLen) >= sizeof(errorMessage.size()));

        header.success = static_cast<uint8_t>(success);
        header.vd = static_cast<uint8_t>(vd);
        header.errorMessageLen = static_cast<uint64_t>(errorMessage.size());

        auto nsend = send(fd, &header, sizeof(header), 0);
        if (nsend != sizeof(header)) {
            return -1;
        }

        if (!errorMessage.empty()) {
            nsend = send(fd, errorMessage.data(), errorMessage.size(), 0);
            if (static_cast<std::size_t>(nsend) != errorMessage.size()) {
                return -1;
            }
        }

        return 0;
    }
}