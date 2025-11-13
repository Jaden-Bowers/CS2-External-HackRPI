#include "entity_list.hpp"

EntityList g_entity_list;

CsPlayerPawn EntityList::local_player() const {
    if (!g_memory.valid() || !addresses::local_player_controller)
        return CsPlayerPawn{};

    const uintptr_t controller = g_memory.read<uintptr_t>(addresses::local_player_controller);
    if (!controller)
        return CsPlayerPawn{};

    const uint32_t pawn_index = g_memory.read<uint32_t>(controller + offsets::m_hPlayerPawn);
    const uintptr_t entry = list_entry_for_handle(pawn_index);
    if (!entry)
        return CsPlayerPawn{};

    return CsPlayerPawn(g_memory.read<uintptr_t>(entry + 112 * (pawn_index & 0x1FF)));
}

CsPlayerPawn EntityList::player_pawn(int index) const {
    if (!g_memory.valid())
        return CsPlayerPawn{};

    const uintptr_t list = g_memory.read<uintptr_t>(addresses::entity_list);
    if (!list)
        return CsPlayerPawn{};

    const uintptr_t entry = g_memory.read<uintptr_t>(list + ((8 * (index & 0x7FFF)) >> 9) + 16);
    if (!entry)
        return CsPlayerPawn{};

    const uintptr_t controller_address = g_memory.read<uintptr_t>(entry + 112 * (index & 0x7FFF));
    if (!controller_address)
        return CsPlayerPawn{};

    CsPlayerController controller(controller_address);
    const uint32_t pawn_handle = controller.pawn_handle();
    if (!pawn_handle || !controller.pawn_alive())
        return CsPlayerPawn{};

    const uintptr_t pawn_entry = list_entry_for_handle(pawn_handle);
    if (!pawn_entry)
        return CsPlayerPawn{};

    return CsPlayerPawn(g_memory.read<uintptr_t>(pawn_entry + 112 * (pawn_handle & 0x1FF)));
}

uintptr_t EntityList::list_entry_for_handle(uint32_t handle) const {
    if (!handle || handle == 0xFFFFFFFF)
        return 0;

    const uintptr_t list = g_memory.read<uintptr_t>(addresses::entity_list);
    if (!list)
        return 0;

    return g_memory.read<uintptr_t>(list + 0x8 * ((handle & 0x7FFF) >> 9) + 16);
}