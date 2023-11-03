#include <iostream>
#include <filesystem>
#include <cstring>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <utils/defer.hpp>

#include <vm.hpp>

using namespace werk;

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "expected one positional argument: path to the binary to run to" << std::endl;
        return 1;
    }

    auto path = std::filesystem::path(argv[1]);

    auto fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        std::cout << utils::PError("open") << std::endl;
        return 1;
    }
    utils::Defer fileClose(close, fd);

    struct stat statResult{};
    if (fstat(fd, &statResult) == -1) {
        std::cout << utils::PError("fstat") << std::endl;
        return 1;
    }

    if (static_cast<std::size_t>(statResult.st_size) > (vm::kMemorySize - vm::kProgramLoadOffset)) {
        std::cout << "program file is too big" << std::endl;
        return 1;
    }

    auto mapped = mmap(nullptr, statResult.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        std::cout << utils::PError("mmap") << std::endl;
        return 1;
    }
    utils::Defer um(munmap, mapped, statResult.st_size);

    uint8_t *memory = new uint8_t[vm::kMemorySize];
    utils::Defer deleter([memory] { delete []memory; });

    std::memcpy(memory + vm::kProgramLoadOffset, mapped, statResult.st_size);

    auto vm = std::make_shared<vm::Vm>(memory);

    std::size_t printedIdx = 0;

    while (vm->GetStatus() == vm::Vm::Status::Running) {
        vm->Tick(100);

        while (printedIdx < vm->GetSerial().size()) {
            std::cout << vm->GetSerial()[printedIdx++];
        }
    }

    while (printedIdx < vm->GetSerial().size()) {
        std::cout << vm->GetSerial()[printedIdx++];
    }

    if (vm->GetStatus() == vm::Vm::Status::Running) {
        std::cout << "!!! Crashed !!!" << std::endl;
        return 1;
    }

    return 0;
}
