#pragma once

#include <windows.h>

namespace sdk::process {
    DWORD FindProcessId(const wchar_t* process_name);
    uintptr_t GetModuleBaseAddress(DWORD process_id, const wchar_t* module_name);
}