#pragma once

class Menu {
public:
    void draw();
    bool is_open() const { return m_is_open; }
    bool esp_enabled() const { return m_esp_enabled; }
    bool crosshair_enabled() const { return m_crosshair_enabled; }
    bool trigger_bot_enabled() const { return m_trigger_bot_enabled; }
    bool draw_heads_enabled() const { return m_draw_heads_enabled; }
    float trigger_bot_delay() const { return m_trigger_bot_delay_seconds; }
    bool aimbot_enabled() const { return m_aimbot_enabled; }
    float aimbot_fov() const { return m_aimbot_fov; }

private:
    void handle_hotkeys();

    bool m_is_open = true;
    bool m_esp_enabled = true;
    bool m_crosshair_enabled = false;
    bool m_trigger_bot_enabled = false;
    bool m_draw_heads_enabled = false;
    float m_trigger_bot_delay_seconds = 0.18f;
    bool m_aimbot_enabled = false;
    float m_aimbot_fov = 80.f;
};
