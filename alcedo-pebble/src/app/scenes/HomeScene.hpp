#pragma once

#include "main.h"
#include "app/Scene.hpp"
#include "app/logic/Character.hpp"
#include "app/logic/PhysicsEngine.hpp"
#include "ui/ui.h"

class HomeScene : public Scene {
public:
    HomeScene();
    virtual ~HomeScene();

    virtual void onEnter() override;
    virtual void update() override;
    virtual void onExit() override;

private:
    std::unique_ptr<Character> character;
    std::unique_ptr<PhysicsEngine> physicsEngine;
	// PhysicsEngine physicsEngine;
    std::vector<std::unique_ptr<Pebble>> pebbles;
    
	Vec2D containerCenter = Vec2D(0, 0); // todo 本来configに入れるべき
    float containerRadius = 120.0f;
    time_t lastUpdateTime = 0;
    int lastBatteryPercentage = -1;

    void updateCommonUi();

    static void scroll_event_cb(lv_event_t* e);
    static void pebble_event_cb(lv_event_t* e);
};