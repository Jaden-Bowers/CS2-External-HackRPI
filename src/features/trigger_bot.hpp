#pragma once

class TriggerBot {
public:
    void update(bool enabled, float delay_seconds);

private:
    void fire_once();

    bool m_target_locked = false;
    double m_last_shot_time = 0.0;
};

extern TriggerBot g_trigger_bot;
