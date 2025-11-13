#pragma once

#include <deque>
#include <mutex>
#include <string>
#include <string_view>

class Log {
public:
    static Log& instance();

    void push(std::string_view message);
    std::deque<std::string> snapshot() const;

private:
    Log() = default;
    void trim_if_needed();

    static constexpr size_t kMaxEntries = 128;

    mutable std::mutex m_mutex;
    std::deque<std::string> m_entries;
};