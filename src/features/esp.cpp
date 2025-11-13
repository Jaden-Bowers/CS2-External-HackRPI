#include "esp.hpp"

#include <imgui.h>

#include "../render/overlay_draw.hpp"
#include "../sdk/entity_list.hpp"

Esp g_esp;

void Esp::run(bool enabled) {
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

        const int health = pawn.health();
        if (health <= 0)
            continue;

        if (pawn.team() == local_team)
            continue;

        vec3_t top = pawn.camera_pos();
        vec3_t bottom = pawn.origin();

        vec2_t screen_top{}, screen_bottom{};
        if (!top.world_to_screen(screen_top, view_matrix, display.x, display.y) ||
            !bottom.world_to_screen(screen_bottom, view_matrix, display.x, display.y))
            continue;

        const float height = screen_bottom.y - screen_top.y;
        const float width = height * 0.5f;

        const ImVec2 box_min(screen_top.x - width * 0.5f, screen_top.y);
        const ImVec2 box_max(screen_top.x + width * 0.5f, screen_bottom.y);

        overlay_draw::AddBox(box_min, box_max, IM_COL32(255, 255, 255, 255));
        overlay_draw::AddHealthBar(ImVec2(box_min.x - 6.f, box_min.y), ImVec2(box_min.x - 2.f, box_max.y), health / 100.f);
    }
}