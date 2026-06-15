#pragma once
#include "main.h"
#include "app/Scene.hpp"

class SettingsScene : public Scene {
public:
    SettingsScene();
    virtual ~SettingsScene();
    virtual void onEnter() override;
    virtual void update() override;
    virtual void onExit() override;

private:
    static void event_handler(lv_event_t * e);
    lv_timer_t* updateTimer = nullptr;
    static void update_timer_cb(lv_timer_t * timer);
};