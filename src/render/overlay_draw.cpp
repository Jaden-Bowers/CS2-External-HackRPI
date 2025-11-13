#include "overlay_draw.hpp"

namespace overlay_draw {

namespace {
    ImDrawList* BackgroundList() {
        return ImGui::GetBackgroundDrawList();
    }
}

void AddBox(const ImVec2& min, const ImVec2& max, ImU32 color, float thickness) {
    BackgroundList()->AddRect(min, max, color, 0.f, 0, thickness);
}

void AddCircle(const ImVec2& center, float radius, ImU32 color, float thickness) {
    BackgroundList()->AddCircle(center, radius, color, 32, thickness);
}

void AddDot(const ImVec2& center, float radius, ImU32 color) {
    BackgroundList()->AddCircleFilled(center, radius, color, 16);
}

void AddHealthBar(const ImVec2& min, const ImVec2& max, float fraction) {
    const float clamped = fraction < 0.f ? 0.f : (fraction > 1.f ? 1.f : fraction);
    ImVec2 filled = min;
    filled.y = max.y - (max.y - min.y) * clamped;
    BackgroundList()->AddRectFilled(min, max, IM_COL32(10, 10, 10, 240));
    BackgroundList()->AddRectFilled(filled, max, IM_COL32(46, 204, 113, 255));
}

void AddText(const ImVec2& position, ImU32 color, std::string_view text) {
    BackgroundList()->AddText(position, color, text.data());
}

ImVec2 DisplaySize() {
    return ImGui::GetIO().DisplaySize;
}

} // namespace overlay_draw
