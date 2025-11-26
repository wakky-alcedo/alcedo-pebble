#pragma once

#include "main.h"
#include "app/Scene.hpp"

// クラスの前方宣言
class Character;

// SLSが生成したUIのグローバル変数
// (ui.h を直接インクルードする)
#include "ui/ui.h"

class MainScene : public Scene {
public:
    MainScene();
    virtual ~MainScene() override;

    virtual void onEnter() override;
    virtual void update() override;
    virtual void onExit() override;

private:
	// キャラクターオブジェクト
    std::unique_ptr<Character> character;

    // SLSが生成したUIオブジェクトへのポインタ
    lv_obj_t* lvCharacterImage;
    lv_obj_t* lvTimeLabel;
    lv_obj_t* lvWeatherIcon;
    lv_obj_t* lvNotificationIcon;
	lv_obj_t* lvMainBatteryLabel;
    
    // UI更新のための内部状態
    time_t lastTimeUpdate = 0;
    int lastNotificationCount = 0;
    std::string lastWeatherCondition = "";
	int lastBatteryPercentage = 0;

    // LVGLイベント遅延登録用タイマー
    lv_timer_t* eventEnableTimer = nullptr;

    void updateTimeLabel();
    void updateWeatherIcon();
    void updateNotificationIcon();
	void updateBatteryLabel();
    
    // LVGLイベントコールバック
    static void main_screen_event_cb(lv_event_t * e);
    void onScreenLongPress();

	// タイマーコールバック
    static void enable_events_timer_cb(lv_timer_t * timer);
};