#pragma once

#include <cstdint>

namespace offsets {
    constexpr std::uintptr_t m_iHealth = 0x34C;
    constexpr std::uintptr_t m_iTeamNum = 0x3EB;

    constexpr std::uintptr_t m_hPlayerPawn = 0x8FC;
    constexpr std::uintptr_t m_bPawnIsAlive = 0x904;

    constexpr std::uintptr_t m_vecLastClipCameraPos = 0x3DC4;
    constexpr std::uintptr_t m_vOldOrigin = 0x15A0;
}

namespace signatures {
    inline constexpr const char* entity_list = "48 89 35 ? ? ? ? 48 85 F6";
    inline constexpr const char* local_player_controller = "48 8B 05 ? ? ? ? 41 89 BE";
    inline constexpr const char* view_matrix = "48 8D 0D ? ? ? ? 48 C1 E0 06";
}

namespace addresses {
    inline uintptr_t entity_list = 0;
    inline uintptr_t local_player_controller = 0;
    inline uintptr_t view_matrix = 0;
}