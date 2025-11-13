#include "win32_window.hpp"

#include <dwmapi.h>

#include "imgui_layer.hpp"

#pragma comment(lib, "dwmapi.lib")

Win32Window& Win32Window::instance() {
    static Win32Window g_instance;
    return g_instance;
}

bool Win32Window::create(const wchar_t* overlay_title, const wchar_t* overlay_class, const wchar_t* target_window_title) {
    m_target_title = target_window_title;
    m_target_window = find_target_window(target_window_title);
    if (!m_target_window)
        return false;

    if (!register_class(overlay_class))
        return false;

    RECT rect{};
    GetWindowRect(m_target_window, &rect);

    m_hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
                             overlay_class,
                             overlay_title,
                             WS_POPUP,
                             rect.left,
                             rect.top,
                             rect.right - rect.left,
                             rect.bottom - rect.top,
                             nullptr,
                             nullptr,
                             m_wc.hInstance,
                             nullptr);
    if (!m_hwnd)
        return false;

    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(m_hwnd, &margins);
    SetLayeredWindowAttributes(m_hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);

    return true;
}

void Win32Window::destroy() {
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }

    if (m_wc.lpszClassName) {
        UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
        m_wc = {};
    }
}

void Win32Window::pump_messages() {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
        if (msg.message == WM_QUIT)
            m_is_running = false;
    }
}

void Win32Window::sync_with_target() {
    if (!m_hwnd)
        return;

    if (!IsWindow(m_target_window)) {
        m_target_window = find_target_window(m_target_title);
        if (!m_target_window) {
            m_should_render = false;
            return;
        }
    }

    RECT rect;
    GetWindowRect(m_target_window, &rect);
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    MoveWindow(m_hwnd, rect.left, rect.top, width, height, TRUE);

    if (GetForegroundWindow() != m_target_window || !IsWindowVisible(m_target_window)) {
        ShowWindow(m_hwnd, SW_HIDE);
        m_should_render = false;
    } else {
        ShowWindow(m_hwnd, SW_SHOW);
        m_should_render = true;
    }
}

void Win32Window::toggle_input_passthrough(bool enable) {
    LONG_PTR style = GetWindowLongPtrW(m_hwnd, GWL_EXSTYLE);
    if (enable)
        SetWindowLongPtrW(m_hwnd, GWL_EXSTYLE, style | WS_EX_TRANSPARENT);
    else
        SetWindowLongPtrW(m_hwnd, GWL_EXSTYLE, style & ~WS_EX_TRANSPARENT);

    m_cursor_passthrough = enable;
    if (enable) {
        SetCursor(nullptr);
    } else {
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
    }
}

bool Win32Window::register_class(const wchar_t* overlay_class) {
    m_wc = { sizeof(WNDCLASSEXW) };
    m_wc.style = CS_CLASSDC;
    m_wc.lpfnWndProc = wnd_proc;
    m_wc.hInstance = GetModuleHandleW(nullptr);
    m_wc.lpszClassName = overlay_class;
    return RegisterClassExW(&m_wc) != 0;
}

HWND Win32Window::find_target_window(const wchar_t* title) {
    return FindWindowW(nullptr, title);
}

LRESULT CALLBACK Win32Window::wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_SETCURSOR && Win32Window::instance().cursor_passthrough()) {
        SetCursor(nullptr);
        return TRUE;
    }

    if (ImguiLayer::active() && ImguiLayer::active()->handle_win32_message(hwnd, msg, wparam, lparam))
        return TRUE;

    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            break;
    }

    return DefWindowProcW(hwnd, msg, wparam, lparam);
}
