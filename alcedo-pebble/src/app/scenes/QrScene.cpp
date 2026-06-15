#include "app/scenes/QrScene.hpp"
#include "app/AlcedoPebble.hpp"
#include "app/scenes/HomeScene.hpp" // ★変更: 戻り先をHomeSceneに

QrScene::QrScene() {
    Serial.println("03 QrScene constructor");
}

QrScene::~QrScene() {
    Serial.println("03 QrScene destroyed");
    // タイマーが残っていれば削除
    if (eventEnableTimer) {
        lv_timer_del(eventEnableTimer);
        eventEnableTimer = nullptr;
    }
}

void QrScene::onEnter() {
    Serial.println("03 QrScene onEnter");

    // 1. SLSで作成したスクリーンを表示
    lv_scr_load(ui_QrScreen);

    // 2. SLSで作成したQRコードウィジェットにデータを設定
    // if (ui_QrCodeObj) {
    //     // (QRコードの色を設定 - 背景黒，コード白)
    //     lv_obj_set_style_border_color(ui_QrCodeObj, lv_color_hex(0x000000), 0);
    //     lv_obj_set_style_background_color(ui_QrCodeObj, lv_color_hex(0xFFFFFF), 0);

    //     // (データをセットしてQRコードを生成)
    //     lv_qrcode_update(ui_QrCodeObj, qrData, strlen(qrData));
        
    //     // (中央に配置 - SLS側でも設定可能)
    //     lv_obj_align(ui_QrCodeObj, LV_ALIGN_CENTER, 0, 0);

    // } else {
    //     Serial.println("ERROR: ui_QrCodeObj is NULL!");
    // }

    // 3. 背景クリックイベントハンドラの登録 (遅延実行)
    eventEnableTimer = lv_timer_create(enable_events_timer_cb, EVENT_DEBOUNCE_TIME, this);
    lv_timer_set_repeat_count(eventEnableTimer, 1); // 1回だけ実行
}

// ワンショットタイマーコールバック (イベント登録)
void QrScene::enable_events_timer_cb(lv_timer_t* timer) {
    QrScene* self = static_cast<QrScene*>(timer->user_data);
    if (!self) {
         Serial.println("ERROR: 03 enable_events_timer_cb self is NULL");
         return;
    }

    self->eventEnableTimer = nullptr; // タイマー参照をクリア

    if (ui_QrScreen) {
        // スクリーン全体（背景）にクリックイベントを設定 (メニューに戻る用)
        lv_obj_add_event_cb(ui_QrScreen, screen_event_cb, LV_EVENT_CLICKED, self);
        Serial.println("03 CLICKED event callback added to ui_QrScreen.");
    }
}


void QrScene::update() {
    // (静的なQRコードなので特に処理なし)
}

void QrScene::onExit() {
    Serial.println("03 QrScene onExit");
    
    // 登録したイベントを削除 (重要)
    if (ui_QrScreen) {
        lv_obj_remove_event_cb_with_user_data(ui_QrScreen, screen_event_cb, this);
    }
}


// --- LVGLイベントコールバック (static) ---

// 画面背景がクリックされた
void QrScene::screen_event_cb(lv_event_t * e) {
    QrScene* self = static_cast<QrScene*>(lv_event_get_user_data(e));
    
    // ★★★ NULLチェック
    if (!self) {
        return;
    }

    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        Serial.println("03 Screen clicked, returning to HomeScene.");
        AlcedoPebble::getInstance().getHardwareManager().vibrate(50);
        // ★変更: HomeSceneに戻る
        AlcedoPebble::getInstance().getSceneManager().changeScene<HomeScene>();
    }
}