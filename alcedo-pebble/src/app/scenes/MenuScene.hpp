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
	// 物理演算用のPebbleオブジェクト群
    std::vector<std::unique_ptr<Pebble>> pebbles;
    PhysicsEngine physicsEngine;

	// 物理演算用のコンテナパラメータ
    Vec2D containerCenter = Vec2D(0, 0);
    float containerRadius = 120.0f;
    unsigned long lastUpdateTime = 0;

	// LVGLイベント遅延登録用タイマー
    lv_timer_t* eventEnableTimer = nullptr;
    
    // LVGLイベントコールバック
    static void pebble_event_cb(lv_event_t * e);
    void onPebbleClicked(lv_obj_t* objClicked);

    static void screen_event_cb(lv_event_t * e);
    void onScreenClicked();

	// タイマーコールバック
    static void enable_events_timer_cb(lv_timer_t * timer);
};