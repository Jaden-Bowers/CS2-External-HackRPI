#include "head_overlay.hpp"

#include <imgui.h>

#include "../render/overlay_draw.hpp"
#include "../sdk/entity_list.hpp"

namespace {
    constexpr float kHeadOffset = 64.f;
    constexpr float kHeadRadius = 8.f;
}

HeadOverlay g_head_overlay;

void HeadOverlay::run(bool enabled) {
    if (!enabled)
        return;

    CsPlayerPawn local = g_entity_list.local_player();
    if (!local.valid())
        return;

    const int local_team = local.team();
    const view_matrix_t view_matrix = local.view_matrix();
    const ImVec2 display = overlay_draw::DisplaySize();

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

        overlay_draw::AddCircle(ImVec2(screen.x, screen.y), kHeadRadius, IM_COL32(255, 0, 0, 200), 2.0f);
    }
}