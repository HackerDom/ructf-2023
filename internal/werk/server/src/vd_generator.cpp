#include <sys/random.h>

#include <utils/strings.hpp>

#include <vd_generator.hpp>

namespace werk::server {
    utils::result<vd_t> VdGenerator::Generate() const {
        vd_t id;
        auto ngenerate = getrandom(&id, sizeof(id), 0);
        if (ngenerate < 0) {
            return utils::result<vd_t>::of_error(utils::PError("getrandom"));
        }
        if (ngenerate != sizeof(id)) {
            return utils::result<vd_t>::of_error("too little bytes generated (out of entropy?)");
        }

        return utils::result<vd_t>::of_success(id);
    }
}
