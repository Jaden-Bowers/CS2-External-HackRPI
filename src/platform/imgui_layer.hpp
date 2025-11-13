#pragma once

#include <d3d11.h>
#include <windows.h>

#include <imgui.h>

class ImguiLayer {
public:
    static ImguiLayer* active();

    bool initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);
    void shutdown();

    void begin_frame();
    void end_frame();

    bool handle_win32_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
    static ImguiLayer* s_active;

    HWND m_hwnd = nullptr;
};