#include "aim_bot.hpp"
#define NOMINMAX
#include <windows.h>

#include <algorithm>
#include <cmath>

#include "../render/overlay_draw.hpp"
#include "../sdk/entity_list.hpp"

namespace {
    constexpr float kHeadOffset = 64.f;
}

AimBot g_aimbot;

void AimBot::run(bool enabled, float fov_radius) {
    if (!enabled)
        return;

    fov_radius = std::clamp(fov_radius, 10.f, 200.f);

    CsPlayerPawn local = g_entity_list.local_player();
    if (!local.valid())
        return;

    const int local_team = local.team();
    const view_matrix_t view_matrix = local.view_matrix();
    const ImVec2 display = overlay_draw::DisplaySize();
    const float mid_x = display.x * 0.5f;
    const float mid_y = display.y * 0.5f;

    float best_distance = fov_radius;
    float best_dx = 0.f;
    float best_dy = 0.f;
    bool found_target = false;

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

        const float dx = screen.x - mid_x;
        const float dy = screen.y - mid_y;
        const float distance = std::sqrt(dx * dx + dy * dy);
        if (distance <= best_distance) {
            best_distance = distance;
            best_dx = dx;
            best_dy = dy;
            found_target = true;
        }
    }

    if (!found_target)
        return;

    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    input.mi.dx = static_cast<LONG>(std::lround(best_dx));
    input.mi.dy = static_cast<LONG>(std::lround(best_dy));
    SendInput(1, &input, sizeof(INPUT));
}
