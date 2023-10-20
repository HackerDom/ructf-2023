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
}
