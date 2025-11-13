#pragma once

#include <cstdint>

#include "../core/math.hpp"
#include "memory.h"
#include "offsets.h"

class BaseEntity {
public:
    BaseEntity() = default;
    explicit BaseEntity(uintptr_t address) : m_address(address) {}

    bool valid() const { return m_address != 0; }
    uintptr_t address() const { return m_address; }

    int health() const { return g_memory.read<int>(m_address + offsets::m_iHealth); }
    int team() const { return g_memory.read<int>(m_address + offsets::m_iTeamNum); }

protected:
    uintptr_t m_address = 0;
};

class CsPlayerController : public BaseEntity {
public:
    using BaseEntity::BaseEntity;

    uint32_t pawn_handle() const { return g_memory.read<uint32_t>(m_address + offsets::m_hPlayerPawn); }
    bool pawn_alive() const { return g_memory.read<bool>(m_address + offsets::m_bPawnIsAlive); }
};

class CsPlayerPawn : public BaseEntity {
public:
    using BaseEntity::BaseEntity;

    vec3_t camera_pos() const { return g_memory.read<vec3_t>(m_address + offsets::m_vecLastClipCameraPos); }
    vec3_t origin() const { return g_memory.read<vec3_t>(m_address + offsets::m_vOldOrigin); }

    view_matrix_t view_matrix() const { return g_memory.read<view_matrix_t>(addresses::view_matrix); }
};

class EntityList {
public:
    CsPlayerPawn local_player() const;
    CsPlayerPawn player_pawn(int index) const;

private:
    uintptr_t list_entry_for_handle(uint32_t handle) const;
};

extern EntityList g_entity_list;