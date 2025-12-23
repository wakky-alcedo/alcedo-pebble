#include "app/scenes/SettingsScene.hpp"
#include "app/AlcedoPebble.hpp"
#include "app/scenes/HomeScene.hpp" // ★変更: 戻り先をHomeSceneに

SettingsScene::SettingsScene() {
    Serial.println("04 SettingsScene constructor");
}

SettingsScene::~SettingsScene() {
    Serial.println("04 SettingsScene destroyed");
    // タイマーが残っていれば削除
    if (updateTimer) {
        lv_timer_del(updateTimer);
        updateTimer = nullptr;
    }
}

void SettingsScene::onEnter() {
    Serial.println("04 SettingsScene onEnter");

    // 1. SLSで作成したスクリーンを表示
    lv_scr_load(ui_SettingsScreen);

    // 2. SLSで作成したウィジェットにイベントハンドラを登録
    
    // 戻るボタン
    if (ui_SettingsBackButton) {
        lv_obj_add_event_cb(ui_SettingsBackButton, event_handler, LV_EVENT_CLICKED, this);
    }
    
    // 明るさスライダー
    if (ui_SettingsBrightnessSlider) {
        // 現在の明るさを取得してスライダーに反映
        auto brightness = AlcedoPebble::getInstance().getDisplayManager().getLgfx()->getBrightness();
        lv_slider_set_value(ui_SettingsBrightnessSlider, (brightness * 100) / 255, LV_ANIM_OFF);
        lv_obj_add_event_cb(ui_SettingsBrightnessSlider, event_handler, LV_EVENT_VALUE_CHANGED, this);
    }

    // バイブON/OFFスイッチ
    if (ui_SettingsVibrateSwitch) {
        // 現在の設定値を取得して反映
        bool vibrateEnabled = AlcedoPebble::getInstance().getHardwareManager().isVibrateEnabled();
        if (vibrateEnabled) {
             lv_obj_add_state(ui_SettingsVibrateSwitch, LV_STATE_CHECKED);
        } else {
             lv_obj_clear_state(ui_SettingsVibrateSwitch, LV_STATE_CHECKED);
        }
        lv_obj_add_event_cb(ui_SettingsVibrateSwitch, event_handler, LV_EVENT_VALUE_CHANGED, this);
    }

    // ディープスリープボタン
    if (ui_SettingsDeepSleepButton) {
        lv_obj_add_event_cb(ui_SettingsDeepSleepButton, event_handler, LV_EVENT_CLICKED, this);
    }

    // 3. バッテリー残量などを定期更新するタイマーを開始
    update_timer_cb(nullptr); // 最初に1回呼び出し
    updateTimer = lv_timer_create(update_timer_cb, 5000, this); // 5秒ごとに更新
}

void SettingsScene::update() {
    // (タイマーベースのため，ここでは特に処理なし)
}

void SettingsScene::onExit() {
    Serial.println("04 SettingsScene onExit");
    
    // 登録したイベントを削除
    if (ui_SettingsBackButton) lv_obj_remove_event_cb_with_user_data(ui_SettingsBackButton, event_handler, this);
    if (ui_SettingsBrightnessSlider) lv_obj_remove_event_cb_with_user_data(ui_SettingsBrightnessSlider, event_handler, this);
    if (ui_SettingsVibrateSwitch) lv_obj_remove_event_cb_with_user_data(ui_SettingsVibrateSwitch, event_handler, this);
    if (ui_SettingsDeepSleepButton) lv_obj_remove_event_cb_with_user_data(ui_SettingsDeepSleepButton, event_handler, this);
    
    // タイマー削除
    if (updateTimer) {
        lv_timer_del(updateTimer);
        updateTimer = nullptr;
    }
}


// --- LVGLイベントコールバック (static) ---

// 設定画面の全ウィジェットのイベントを処理
void SettingsScene::event_handler(lv_event_t * e) {
    SettingsScene* self = static_cast<SettingsScene*>(lv_event_get_user_data(e));
    if (!self) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);

    // --- 戻るボタン ---
    if (target == ui_SettingsBackButton && code == LV_EVENT_CLICKED) {
        Serial.println("04 Back Button clicked, returning to HomeScene.");
        AlcedoPebble::getInstance().getHardwareManager().vibrate(50);
        // ★変更: HomeSceneに戻る
        AlcedoPebble::getInstance().getSceneManager().changeScene<HomeScene>();
    }

    // --- 明るさスライダー ---
    else if (target == ui_SettingsBrightnessSlider && code == LV_EVENT_VALUE_CHANGED) {
        int32_t value = lv_slider_get_value(target); // 0-100
        uint8_t brightness = (value * 255) / 100; // 0-255に変換
        AlcedoPebble::getInstance().getDisplayManager().getLgfx()->setBrightness(brightness);
    }

    // --- バイブスイッチ ---
    else if (target == ui_SettingsVibrateSwitch && code == LV_EVENT_VALUE_CHANGED) {
        bool enabled = lv_obj_has_state(target, LV_STATE_CHECKED);
        Serial.printf("04 Vibrate set to: %d\n", enabled);
        // HardwareManagerに設定を保存
        AlcedoPebble::getInstance().getHardwareManager().setVibrateEnabled(enabled);
    }

    // --- ディープスリープボタン ---
    else if (target == ui_SettingsDeepSleepButton && code == LV_EVENT_CLICKED) {
        Serial.println("04 Deep Sleep Button clicked.");
        // PowerManagerを呼び出し
        AlcedoPebble::getInstance().getPowerManager().enterDeepSleep();
    }
}

// --- タイマーコールバック (static) ---
void SettingsScene::update_timer_cb(lv_timer_t * timer) {
    // データを取得
    auto battData = AlcedoPebble::getInstance().getPowerManager().getBatteryData();

    // バッテリーラベル更新
    if (ui_SettingsBatteryLabel) {
        lv_label_set_text_fmt(ui_SettingsBatteryLabel, "Battery: %d%% (%.2fV)", battData.percentage, battData.voltage);
    }

    // BLEステータスラベル更新
    if (ui_SettingsBleLabel) {
         lv_label_set_text(ui_SettingsBleLabel, "BLE: Disconnected");
    }
}