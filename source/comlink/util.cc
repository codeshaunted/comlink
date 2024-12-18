#include "util.hh"

#include <Windows.h>

namespace comlink {

void Util::nopFill(uintptr_t dest, size_t len) {
    DWORD old_protect;
    VirtualProtect((LPVOID)(dest), len, PAGE_EXECUTE_READWRITE, &old_protect);

    memset(reinterpret_cast<void*>(dest), 0x90, len);

    DWORD dummy_protect;
    VirtualProtect(reinterpret_cast<void*>(dest), len, old_protect, &dummy_protect);
}

} // namespace comlink