#include "process.h"

#include <tlhelp32.h>
#include <cwchar>

namespace sdk::process {

DWORD FindProcessId(const wchar_t* process_name) {
    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(PROCESSENTRY32W);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;

    DWORD process_id = 0;
    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, process_name) == 0) {
                process_id = entry.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return process_id;
}

uintptr_t GetModuleBaseAddress(DWORD process_id, const wchar_t* module_name) {
    MODULEENTRY32W module_entry{};
    module_entry.dwSize = sizeof(MODULEENTRY32W);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);
    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;

    uintptr_t base = 0;
    if (Module32FirstW(snapshot, &module_entry)) {
        do {
            if (_wcsicmp(module_entry.szModule, module_name) == 0) {
                base = reinterpret_cast<uintptr_t>(module_entry.modBaseAddr);
                break;
            }
        } while (Module32NextW(snapshot, &module_entry));
    }

    CloseHandle(snapshot);
    return base;
}

} // namespace sdk::process
