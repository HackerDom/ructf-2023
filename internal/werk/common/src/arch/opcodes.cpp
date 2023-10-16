#include <vector>
#include <string_view>

#include <arch/opcodes.hpp>

namespace werk::arch::opcodes {
    constexpr std::string_view kMov = "mov";
    constexpr std::string_view kLoad = "load";
    constexpr std::string_view kStore = "store";
    constexpr std::string_view kAdd = "add";
    constexpr std::string_view kSub = "sub";
    constexpr std::string_view kMul = "mul";
    constexpr std::string_view kSetPixel = "setpixel";
    constexpr std::string_view kGetPixel = "getpixel";
    constexpr std::string_view kClearScreen = "clearscreen";
    constexpr std::string_view kOut = "out";
    constexpr std::string_view kHexOut = "hexout";
    constexpr std::string_view kCmp = "cmp";
    constexpr std::string_view kJmp = "jmp";
    constexpr std::string_view kJe = "je";
    constexpr std::string_view kJne = "jne";
    constexpr std::string_view kJl = "jl";
    constexpr std::string_view kJg = "jg";
    constexpr std::string_view kJle = "jle";
    constexpr std::string_view kJge = "jge";
    constexpr std::string_view kCall = "call";
    constexpr std::string_view kRet = "ret";
    constexpr std::string_view kAnd = "and";
    constexpr std::string_view kOr = "or";
    constexpr std::string_view kShl = "shl";
    constexpr std::string_view kShr = "shr";

    OpcodesManager::OpcodesManager() {
        allOpcodes.insert(kMov);
        allOpcodes.insert(kLoad);
        allOpcodes.insert(kStore);
        allOpcodes.insert(kAdd);
        allOpcodes.insert(kSub);
        allOpcodes.insert(kMul);
        allOpcodes.insert(kSetPixel);
        allOpcodes.insert(kGetPixel);
        allOpcodes.insert(kClearScreen);
        allOpcodes.insert(kOut);
        allOpcodes.insert(kHexOut);
        allOpcodes.insert(kCmp);
        allOpcodes.insert(kJmp);
        allOpcodes.insert(kJe);
        allOpcodes.insert(kJne);
        allOpcodes.insert(kJl);
        allOpcodes.insert(kJg);
        allOpcodes.insert(kJle);
        allOpcodes.insert(kJge);
        allOpcodes.insert(kCall);
        allOpcodes.insert(kRet);
        allOpcodes.insert(kAnd);
        allOpcodes.insert(kOr);
        allOpcodes.insert(kShl);
        allOpcodes.insert(kShr);
    }

    bool OpcodesManager::IsValidOpcodeName(std::string_view name) {
        auto it = allOpcodes.find(name);

        return it != allOpcodes.end();
    }
}