#pragma once

#include <windows.h>

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

#include "process.h"

struct PatternByte {
    unsigned char value;
    bool wildcard;
};

class Memory {
public:
    ~Memory();

    bool attach(const wchar_t* process_name);
    void detach();

    bool valid() const { return m_process_handle != nullptr; }
    HANDLE handle() const { return m_process_handle; }
    DWORD process_id() const { return m_process_id; }

    uintptr_t module_base(const wchar_t* module_name) const;

    template <typename T>
    T read(uintptr_t address) const {
        T buffer{};
        ReadProcessMemory(m_process_handle, reinterpret_cast<LPCVOID>(address), &buffer, sizeof(T), nullptr);
        return buffer;
    }

    template <typename T>
    bool write(uintptr_t address, const T& value) const {
        return WriteProcessMemory(m_process_handle, reinterpret_cast<LPVOID>(address), &value, sizeof(T), nullptr) != FALSE;
    }

    uintptr_t find_pattern(uintptr_t base, size_t size, const char* pattern) const;
    uintptr_t resolve_rip(uintptr_t address, int offset, int instruction_length) const;

private:
    static std::vector<PatternByte> parse_pattern(const char* pattern_str);

    HANDLE m_process_handle = nullptr;
    DWORD m_process_id = 0;
};

extern Memory g_memory;

inline Memory::~Memory() {
    detach();
}

inline bool Memory::attach(const wchar_t* process_name) {
    detach();

    m_process_id = sdk::process::FindProcessId(process_name);
    if (!m_process_id)
        return false;

    m_process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_process_id);
    return m_process_handle != nullptr;
}

inline void Memory::detach() {
    if (m_process_handle) {
        CloseHandle(m_process_handle);
        m_process_handle = nullptr;
        m_process_id = 0;
    }
}

inline uintptr_t Memory::module_base(const wchar_t* module_name) const {
    return sdk::process::GetModuleBaseAddress(m_process_id, module_name);
}

inline std::vector<PatternByte> Memory::parse_pattern(const char* pattern_str) {
    std::vector<PatternByte> pattern;
    const char* current = pattern_str;

    while (*current) {
        if (*current == ' ') {
            ++current;
            continue;
        }

        if (*current == '?') {
            pattern.push_back({ 0, true });
            ++current;
            if (*current == '?')
                ++current;
        } else {
            char buffer[3] = { current[0], current[1], 0 };
            pattern.push_back({ static_cast<unsigned char>(strtoul(buffer, nullptr, 16)), false });
            current += 2;
        }
    }

    return pattern;
}

inline uintptr_t Memory::find_pattern(uintptr_t base, size_t size, const char* pattern) const {
    if (!m_process_handle)
        return 0;

    std::vector<char> buffer(size);
    SIZE_T bytes_read = 0;
    if (!ReadProcessMemory(m_process_handle, reinterpret_cast<LPCVOID>(base), buffer.data(), size, &bytes_read)) {
        if (bytes_read == 0)
            return 0;
    }

    const auto parsed_pattern = parse_pattern(pattern);
    if (parsed_pattern.empty() || bytes_read < parsed_pattern.size())
        return 0;

    for (size_t i = 0; i <= bytes_read - parsed_pattern.size(); ++i) {
        bool found = true;
        for (size_t j = 0; j < parsed_pattern.size(); ++j) {
            if (!parsed_pattern[j].wildcard && parsed_pattern[j].value != static_cast<unsigned char>(buffer[i + j])) {
                found = false;
                break;
            }
        }

        if (found)
            return base + i;
    }

    return 0;
}

inline uintptr_t Memory::resolve_rip(uintptr_t address, int offset, int instruction_length) const {
    const int rel = read<int>(address + offset);
    return address + rel + instruction_length;
}
