#pragma once

#include "main.h"
#include "app/Scene.hpp"

// --- QrSceneクラス ---
// QRコード表示画面
class QrScene : public Scene {
public:
    QrScene();
    virtual ~QrScene();

    virtual void onEnter() override;
    virtual void update() override;
    virtual void onExit() override;

private:
    const char* qrData = "https://www.espressif.com/"; // QRコードにする文字列

    // ワンショットタイマー (イベント登録遅延用)
    lv_timer_t* eventEnableTimer = nullptr;
    // タイマーコールバック
    static void enable_events_timer_cb(lv_timer_t* timer);

    // LVGLイベントコールバック (static)
    static void screen_event_cb(lv_event_t * e); // 画面背景クリック
};