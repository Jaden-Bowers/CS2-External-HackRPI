#pragma once

#include <imgui.h>
#include <string_view>

namespace overlay_draw {
    void AddBox(const ImVec2& min, const ImVec2& max, ImU32 color, float thickness = 1.0f);
    void AddCircle(const ImVec2& center, float radius, ImU32 color, float thickness = 1.0f);
    void AddDot(const ImVec2& center, float radius, ImU32 color);
    void AddHealthBar(const ImVec2& min, const ImVec2& max, float fraction);
    void AddText(const ImVec2& position, ImU32 color, std::string_view text);

    ImVec2 DisplaySize();
}
