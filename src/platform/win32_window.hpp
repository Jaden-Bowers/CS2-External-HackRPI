#pragma once

#include <windows.h>

class Win32Window {
public:
    static Win32Window& instance();

    bool create(const wchar_t* overlay_title, const wchar_t* overlay_class, const wchar_t* target_window_title);
    void destroy();

    void pump_messages();
    void sync_with_target();

    bool is_running() const { return m_is_running; }
    bool should_render() const { return m_should_render; }
    bool cursor_passthrough() const { return m_cursor_passthrough; }

    HWND handle() const { return m_hwnd; }
    HWND target_handle() const { return m_target_window; }

    void toggle_input_passthrough(bool enable);

private:
    Win32Window() = default;

    static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    bool register_class(const wchar_t* overlay_class);
    HWND find_target_window(const wchar_t* title);

    HWND m_hwnd = nullptr;
    HWND m_target_window = nullptr;
    WNDCLASSEXW m_wc{};
    const wchar_t* m_target_title = nullptr;
    bool m_is_running = true;
    bool m_should_render = false;
    bool m_cursor_passthrough = false;
};
