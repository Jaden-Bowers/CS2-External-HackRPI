#include "auto_shoot.hpp"
#define NOMINMAX
#include <windows.h>

#include <imgui.h>
#include <algorithm>
#include <chrono>
#include <cmath>

#include "../render/overlay_draw.hpp"
#include "../sdk/entity_list.hpp"

AutoShoot g_auto_shoot;

namespace {
    constexpr float kCrosshairTolerance = 4.f;
    constexpr float kHeadOffset = 64.f;

    double CurrentTimeSeconds() {
        return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }
}

void AutoShoot::update(bool enabled, float delay_seconds) {
    if (!enabled) {
        m_target_locked = false;
        return;
    }

    delay_seconds = std::max(0.05f, delay_seconds);
    const double now = CurrentTimeSeconds();

    CsPlayerPawn local = g_entity_list.local_player();
    if (!local.valid()) {
        m_target_locked = false;
        return;
    }

    const int local_team = local.team();
    const view_matrix_t view_matrix = local.view_matrix();
    const ImVec2 display = overlay_draw::DisplaySize();
    const float screen_mid_x = display.x * 0.5f;
    const float screen_mid_y = display.y * 0.5f;

    bool target_found = false;

    for (int i = 0; i < 64; ++i) {
        CsPlayerPawn pawn = g_entity_list.player_pawn(i);
        if (!pawn.valid())
            continue;

        if (pawn.team() == local_team)
            continue;

        if (pawn.health() <= 0)
            continue;

        vec3_t head = pawn.origin();
        head.z += kHeadOffset;

        vec2_t screen{};
        if (!head.world_to_screen(screen, view_matrix, display.x, display.y))
            continue;

        if (fabsf(screen.x - screen_mid_x) <= kCrosshairTolerance && fabsf(screen.y - screen_mid_y) <= kCrosshairTolerance) {
            target_found = true;
            break;
        }
    }

    if (target_found && (now - m_last_shot_time) >= delay_seconds) {
        fire_once();
        m_last_shot_time = now;
    }

    m_target_locked = target_found;
}

void AutoShoot::fire_once() {
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(2, inputs, sizeof(INPUT));
}
