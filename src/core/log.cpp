#include "log.hpp"

Log& Log::instance() {
    static Log g_instance;
    return g_instance;
}

void Log::push(std::string_view message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_entries.emplace_back(message);
    trim_if_needed();
}

std::deque<std::string> Log::snapshot() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_entries;
}

void Log::trim_if_needed() {
    while (m_entries.size() > kMaxEntries) {
        m_entries.pop_front();
    }
}