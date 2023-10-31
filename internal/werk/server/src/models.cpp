#include <chrono>
#include <cmath>

#include <sys/types.h>
#include <sys/socket.h>

#include <utils/strings.hpp>

#include <models.hpp>

namespace werk::server {
    utils::result_no_value sendall(int fd, void *data, std::size_t n, std::chrono::milliseconds timeout) {
        auto start = std::chrono::high_resolution_clock::now();
        auto *p = reinterpret_cast<uint8_t*>(data);
        std::size_t total = 0;
        std::size_t left = n;

        while (total != n) {
            auto nsend = send(fd, p, left, 0);
            if (nsend < 0) {
                return utils::result_no_value::of_error(utils::PError("send"));
            }
            if (nsend == 0) {
                auto diff = std::chrono::high_resolution_clock::now() - start;
                auto diffMs = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
                if (diffMs > timeout) {
                    return utils::result_no_value::of_error("timeout");
                }
            }

            total += static_cast<std::size_t>(nsend);
            p += nsend;
        }

        return utils::result_no_value::of_success();
    }

    std::string RunRequest::String() const {
        return utils::Format("RunRequest(binaryPath='%s')", binaryPath.c_str());
    }

    std::string RunResponse::String() const {
        return utils::Format("RunResponse(success=%d, vd=%lx, errorMessage='%s')",
                             success, vd, errorMessage.c_str()
        );
    }

    utils::result<std::shared_ptr<RunRequest>> RunRequest::ReadFromSocket(int fd) {
        struct {
            uint16_t binaryPathLen;
        } header{};

        auto nrecv = recv(fd, &header, sizeof(header), MSG_WAITALL);
        if (nrecv != sizeof(header)) {
            return utils::result<std::shared_ptr<RunRequest>>::of_error(utils::PError("header recv"));
        }

        std::string binaryPathStr;
        binaryPathStr.resize(header.binaryPathLen);
        nrecv = recv(fd, binaryPathStr.data(), binaryPathStr.size(), MSG_WAITALL);
        if (static_cast<std::size_t>(nrecv) != binaryPathStr.size()) {
            return utils::result<std::shared_ptr<RunRequest>>::of_error(utils::PError("binary path recv"));
        }

        auto req = std::make_shared<RunRequest>(std::move(binaryPathStr));
        return utils::result<std::shared_ptr<RunRequest>>::of_success(req);
    }

    utils::result_no_value RunResponse::WriteToSocket(int fd) {
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
            return utils::result_no_value::of_error(utils::PError("header send"));
        }

        if (!errorMessage.empty()) {
            nsend = send(fd, errorMessage.data(), errorMessage.size(), 0);
            if (static_cast<std::size_t>(nsend) != errorMessage.size()) {
                return utils::result_no_value::of_error(utils::PError("error message send"));
            }
        }

        return utils::result_no_value::of_success();
    }

    std::string KillRequest::String() const {
        return utils::Format("KillRequest(vd=%lx)", vd);
    }

    std::string KillResponse::String() const {
        return utils::Format("KillResponse(success=%d)", success);
    }

    utils::result<std::shared_ptr<KillRequest>> KillRequest::ReadFromSocket(int fd) {
        struct {
            uint64_t vd;
        } header;
        static_assert(sizeof(header.vd) == sizeof(vd_t));

        auto nrecv = recv(fd, &header, sizeof(header), MSG_WAITALL);
        if (nrecv != sizeof(header)) {
            return utils::result<std::shared_ptr<KillRequest>>::of_error(utils::PError("header recv"));
        }

        auto req = std::make_shared<KillRequest>(header.vd);
        return utils::result<std::shared_ptr<KillRequest>>::of_success(req);
    }

    utils::result_no_value KillResponse::WriteToSocket(int fd) {
        struct {
            uint8_t success;
        } header;

        header.success = static_cast<uint8_t>(success);

        auto nsend = send(fd, &header, sizeof(header), 0);
        if (nsend != sizeof(header)) {
            return utils::result_no_value::of_error(utils::PError("header send"));
        }

        return utils::result_no_value::of_success();
    }

    std::string StatusRequest::String() const {
        return utils::Format("StatusRequest(vd=%lx)", vd);
    }

    std::string StatusResponse::String() const {
        return utils::Format("StatusResponse(success=%d, state=%d)", success, state);
    }

    utils::result<std::shared_ptr<StatusRequest>> StatusRequest::ReadFromSocket(int fd) {
        struct {
            uint64_t vd;
        } header;
        static_assert(sizeof(header.vd) == sizeof(vd));

        auto nrecv = recv(fd, &header, sizeof(header), MSG_WAITALL);
        if (nrecv != sizeof(header)) {
            return utils::result<std::shared_ptr<StatusRequest>>::of_error(utils::PError("header recv"));
        }

        auto req = std::make_shared<StatusRequest>(header.vd);
        return utils::result<std::shared_ptr<StatusRequest>>::of_success(req);
    }

    utils::result_no_value StatusResponse::WriteToSocket(int fd) {
        struct {
            uint8_t success;
            uint8_t status;
        } header;

        header.success = success;
        header.status = static_cast<uint8_t>(state);

        auto nsend = send(fd, &header, sizeof(header), 0);
        if (nsend != sizeof(header)) {
            return utils::result_no_value::of_error(utils::PError("header send"));
        }

        return utils::result_no_value::of_success();
    }

    std::string DeleteRequest::String() const {
        return utils::Format("DeleteRequest(vd=%lx)", vd);
    }

    std::string DeleteResponse::String() const {
        return utils::Format("DeleteResponse(success=%d)", success);
    }

    utils::result<std::shared_ptr<DeleteRequest>> DeleteRequest::ReadFromSocket(int fd) {
        struct {
            uint64_t vd;
        } header;
        static_assert(sizeof(header.vd) == sizeof(vd));

        auto nrecv = recv(fd, &header, sizeof(header), MSG_WAITALL);
        if (nrecv != sizeof(header)) {
            return utils::result<std::shared_ptr<DeleteRequest>>::of_error(utils::PError("header recv"));
        }

        auto req = std::make_shared<DeleteRequest>(header.vd);
        return utils::result<std::shared_ptr<DeleteRequest>>::of_success(req);
    }

    utils::result_no_value DeleteResponse::WriteToScoket(int fd) {
        struct {
            uint8_t success;
        } header;

        header.success = static_cast<uint8_t>(success);

        auto nsend = send(fd, &header, sizeof(header), 0);
        if (nsend != sizeof(header)) {
            return utils::result_no_value::of_error(utils::PError("header send"));
        }

        return utils::result_no_value::of_success();
    }

    std::string GetSerialRequest::String() const {
        return utils::Format("GetSerialRequest(vd=%lx)", vd);
    }

    std::string GetSerialResponse::String() const {
        return utils::Format("GetSerialResponse(success=%d, serial=%s)", success, serial.c_str());
    }

    utils::result<std::shared_ptr<GetSerialRequest>> GetSerialRequest::ReadFromSocket(int fd) {
        struct {
            uint64_t vd;
        } header;
        static_assert(sizeof(header.vd) == sizeof(vd));

        auto nrecv = recv(fd, &header, sizeof(header), MSG_WAITALL);
        if (nrecv != sizeof(header)) {
            return utils::result<std::shared_ptr<GetSerialRequest>>::of_error(utils::PError("header recv"));
        }

        auto req = std::make_shared<GetSerialRequest>(header.vd);
        return utils::result<std::shared_ptr<GetSerialRequest>>::of_success(req);
    }

    utils::result_no_value GetSerialResponse::WriteToSocket(int fd) {
        struct {
            uint8_t success;
            uint64_t len;
        } header;
        static_assert(sizeof(header.len) >= sizeof(serial.size()));

        header.success = static_cast<uint8_t>(success);
        header.len = success ? static_cast<uint64_t>(serial.size()) : 0;

        auto nsend = send(fd, &header, sizeof(header), 0);
        if (nsend != sizeof(header)) {
            return utils::result_no_value::of_error(utils::PError("header send"));
        }

        if (success && serial.size() > 0) {
            auto r = sendall(fd, serial.data(), serial.size(), std::chrono::milliseconds(300));
            if (!r) {
                return utils::result_no_value::of_error(utils::Format("serial send: %s", r.message));
            }
        }

        return utils::result_no_value::of_success();
    }
}
