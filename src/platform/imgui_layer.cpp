#include "imgui_layer.hpp"

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ImguiLayer* ImguiLayer::s_active = nullptr;

ImguiLayer* ImguiLayer::active() {
    return s_active;
}

bool ImguiLayer::initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context) {
    if (s_active)
        return false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    if (!ImGui_ImplWin32_Init(hwnd))
        return false;

    if (!ImGui_ImplDX11_Init(device, context))
        return false;

    m_hwnd = hwnd;
    s_active = this;
    return true;
}

void ImguiLayer::shutdown() {
    if (!s_active)
        return;

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    s_active = nullptr;
    m_hwnd = nullptr;
}

void ImguiLayer::begin_frame() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImguiLayer::end_frame() {
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool ImguiLayer::handle_win32_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (!s_active || hwnd != m_hwnd)
        return false;

    return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
}
