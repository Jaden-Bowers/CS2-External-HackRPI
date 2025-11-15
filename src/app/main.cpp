#include <windows.h>

#include "../core/log.hpp"
#include "../features/esp.hpp"
#include "../features/trigger_bot.hpp"
#include "../features/head_overlay.hpp"
#include "../features/aim_bot.hpp"
#include "../platform/d3d11_device.hpp"
#include "../platform/imgui_layer.hpp"
#include "../platform/win32_window.hpp"
#include "../render/overlay_draw.hpp"
#include "../sdk/entity_list.hpp"
#include "../sdk/memory.h"
#include "../sdk/offsets.h"
#include "../ui/menu.hpp"

Memory g_memory;

namespace {
    bool ResolveAddresses(uintptr_t client_base) {
        constexpr size_t kScanSize = 0x5000000;

        const uintptr_t entity_sig = g_memory.find_pattern(client_base, kScanSize, signatures::entity_list);
        const uintptr_t controller_sig = g_memory.find_pattern(client_base, kScanSize, signatures::local_player_controller);
        const uintptr_t view_sig = g_memory.find_pattern(client_base, kScanSize, signatures::view_matrix);

        if (!entity_sig || !controller_sig || !view_sig)
            return false;

        addresses::entity_list = g_memory.resolve_rip(entity_sig, 3, 7);
        addresses::local_player_controller = g_memory.resolve_rip(controller_sig, 3, 7);
        addresses::view_matrix = g_memory.resolve_rip(view_sig, 3, 7);
        return true;
    }
}

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    if (!g_memory.attach(L"cs2.exe")) {
        MessageBoxW(nullptr, L"Failed to attach to cs2.exe", L"cs2-external", MB_ICONERROR);
        return 1;
    }

    Log::instance().push("Attached to cs2.exe");

    const uintptr_t client_base = g_memory.module_base(L"client.dll");
    if (!client_base) {
        MessageBoxW(nullptr, L"Failed to find client.dll", L"cs2-external", MB_ICONERROR);
        return 1;
    }

    if (!ResolveAddresses(client_base)) {
        MessageBoxW(nullptr, L"Signature scan failed", L"cs2-external", MB_ICONERROR);
        return 1;
    }

    auto& window = Win32Window::instance();
    if (!window.create(L"CS2 External", L"CS2_EXTERNAL_CLASS", L"Counter-Strike 2")) {
        MessageBoxW(nullptr, L"Failed to create overlay", L"cs2-external", MB_ICONERROR);
        return 1;
    }

    D3D11Device device;
    if (!device.initialize(window.handle())) {
        MessageBoxW(nullptr, L"Failed to initialize D3D11", L"cs2-external", MB_ICONERROR);
        return 1;
    }

    ImguiLayer imgui;
    if (!imgui.initialize(window.handle(), device.device(), device.context())) {
        MessageBoxW(nullptr, L"Failed to initialize ImGui", L"cs2-external", MB_ICONERROR);
        return 1;
    }

    Menu menu;

    while (window.is_running()) {
        window.pump_messages();
        window.sync_with_target();
        window.toggle_input_passthrough(!menu.is_open());

        if (!window.should_render())
            continue;

        device.begin_frame();
        imgui.begin_frame();

        g_esp.run(menu.esp_enabled());
        g_head_overlay.run(menu.draw_heads_enabled());
        g_aimbot.run(menu.aimbot_enabled(), menu.aimbot_fov());
        g_trigger_bot.update(menu.trigger_bot_enabled(), menu.trigger_bot_delay());

        const ImVec2 display = overlay_draw::DisplaySize();
        const ImVec2 center(display.x * 0.5f, display.y * 0.5f);

        if (menu.crosshair_enabled()) {
            overlay_draw::AddDot(center, 2.0f, IM_COL32(255, 50, 50, 255));
        }

        if (menu.aimbot_enabled()) {
            overlay_draw::AddCircle(center, menu.aimbot_fov(), IM_COL32(255, 50, 50, 100), 1.5f);
        }
        menu.draw();

        imgui.end_frame();
        device.end_frame();
        device.present();
    }

    imgui.shutdown();
    device.shutdown();
    window.destroy();
    g_memory.detach();

    return 0;
}
