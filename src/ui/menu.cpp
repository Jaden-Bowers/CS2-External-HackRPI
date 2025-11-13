#include "menu.hpp"

#include <windows.h>

#include <imgui.h>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <string>

#include "../core/log.hpp"

void Menu::handle_hotkeys() {
    if (GetAsyncKeyState(VK_INSERT) & 1)
        m_is_open = !m_is_open;
}

void Menu::draw() {
    handle_hotkeys();
    if (!m_is_open)
        return;

    ImGui::SetNextWindowSize(ImVec2(320.f, 220.f), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("cs2-external", &m_is_open, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    const bool previous_esp = m_esp_enabled;
    if (ImGui::Checkbox("Enable ESP", &m_esp_enabled) && previous_esp != m_esp_enabled) {
        Log::instance().push(m_esp_enabled ? "ESP enabled" : "ESP disabled");
    }

    const bool previous_crosshair = m_crosshair_enabled;
    if (ImGui::Checkbox("Custom Crosshair", &m_crosshair_enabled) && previous_crosshair != m_crosshair_enabled) {
        Log::instance().push(m_crosshair_enabled ? "Custom crosshair enabled" : "Custom crosshair disabled");
    }

    const bool previous_trigger_bot = m_trigger_bot_enabled;
    if (ImGui::Checkbox("Trigger Bot", &m_trigger_bot_enabled) && previous_trigger_bot != m_trigger_bot_enabled) {
        Log::instance().push(m_trigger_bot_enabled ? "Trigger bot enabled" : "Trigger bot disabled");
    }

    float delay = m_trigger_bot_delay_seconds;
    if (ImGui::SliderFloat("Trigger Bot Delay (s)", &m_trigger_bot_delay_seconds, 0.05f, 0.5f, "%.2f s")) {
        m_trigger_bot_delay_seconds = std::clamp(m_trigger_bot_delay_seconds, 0.05f, 0.5f);
        if (fabsf(m_trigger_bot_delay_seconds - delay) > FLT_EPSILON) {
            Log::instance().push(std::string("Trigger bot delay set to ") + std::to_string(m_trigger_bot_delay_seconds) + "s");
        }
    }

    const bool previous_draw_heads = m_draw_heads_enabled;
    if (ImGui::Checkbox("Draw Heads", &m_draw_heads_enabled) && previous_draw_heads != m_draw_heads_enabled) {
        Log::instance().push(m_draw_heads_enabled ? "Head circles enabled" : "Head circles disabled");
    }

    ImGui::Separator();
    if (ImGui::Button("Exit")) {
        Log::instance().push("Quitting application.");
        PostQuitMessage(0);
    }

    ImGui::TextUnformatted("Log");
    if (ImGui::BeginChild("log_window", ImVec2(0.f, 0.f), true)) {
        const auto entries = Log::instance().snapshot();
        for (const auto& entry : entries) {
            ImGui::TextUnformatted(entry.c_str());
        }
    }
    ImGui::EndChild();

    ImGui::End();
}
