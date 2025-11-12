#pragma once

#include "main.h"
#include "app/Scene.hpp"
#include "app/logic/PhysicsEngine.hpp"

// (ui.h を直接インクルードする)
#include "ui/ui.h"

class MenuScene : public Scene {
public:
    MenuScene();
    virtual ~MenuScene() override;

    virtual void onEnter() override;
    virtual void update() override;
    virtual void onExit() override;

private:
    std::vector<std::unique_ptr<Pebble>> pebbles;
    PhysicsEngine physicsEngine;

    Vec2D containerCenter = Vec2D(120, 120);
    float containerRadius = 120.0f;
    unsigned long lastUpdateTime = 0;
    
    // LVGLイベントコールバック
    static void pebble_event_cb(lv_event_t * e);
    void onPebbleClicked(lv_obj_t* objClicked);
    static void screen_event_cb(lv_event_t * e);
    void onScreenClicked();
};