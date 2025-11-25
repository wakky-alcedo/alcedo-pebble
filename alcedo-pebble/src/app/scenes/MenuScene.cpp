#include "MenuScene.hpp"
#include "MainScene.hpp" // メインシーンに戻るため
#include "QrScene.hpp"
#include "app/AlcedoPebble.hpp"

MenuScene::MenuScene() :
	eventEnableTimer(nullptr)
{
    lastUpdateTime = millis();
}

MenuScene::~MenuScene() {
    Serial0.println("02 MenuScene destroyed");
}

void MenuScene::onEnter() {
    Serial0.println("02 MenuScene onEnter");

    if (ui_MenuScreen) {
        lv_scr_load(ui_MenuScreen);
    } else {
        Serial0.println("02 ERROR: ui_MenuScreen is NULL!");
        return;
    }

    // lv_obj_add_event_cb(ui_MenuScreen, screen_event_cb, LV_EVENT_ALL, this);
	// 200ms後にクリックイベントを有効にするワンショットタイマーを作成
    eventEnableTimer = lv_timer_create(enable_events_timer_cb, EVENT_DEBOUNCE_TIME, this);
    lv_timer_set_repeat_count(eventEnableTimer, 1); // 1回だけ実行

    // 1. 物理演算用のPebbleオブジェクトを生成
    pebbles.clear(); 
    float radius = 30.0f; // pebbleの半径
    pebbles.push_back(std::make_unique<Pebble>(Vec2D(120, 80), radius, 0)); // ID 0: QR
    pebbles.push_back(std::make_unique<Pebble>(Vec2D(80, 150), radius, 1)); // ID 1: Game
    pebbles.push_back(std::make_unique<Pebble>(Vec2D(160, 150), radius, 2)); // ID 2: Settings

    // 2. PebbleオブジェクトとLVGLのUIオブジェクトを紐付け
    // (ui_PebbleIconQR などは ui.h で宣言されているグローバル変数を代入)
    if (ui_PebbleIconQR) {
        pebbles[0]->lvglObject = ui_PebbleIconQR;
        lv_obj_add_event_cb(ui_PebbleIconQR, pebble_event_cb, LV_EVENT_CLICKED, this);
        lv_obj_add_flag(ui_PebbleIconQR, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(ui_PebbleIconQR, LV_OBJ_FLAG_GESTURE_BUBBLE);
    }
    if (ui_PebbleIconGame) {
        pebbles[1]->lvglObject = ui_PebbleIconGame;
        lv_obj_add_event_cb(ui_PebbleIconGame, pebble_event_cb, LV_EVENT_CLICKED, this);
        lv_obj_add_flag(ui_PebbleIconGame, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(ui_PebbleIconGame, LV_OBJ_FLAG_GESTURE_BUBBLE);
    }
    if (ui_PebbleIconSettings) {
        pebbles[2]->lvglObject = ui_PebbleIconSettings;
        lv_obj_add_event_cb(ui_PebbleIconSettings, pebble_event_cb, LV_EVENT_CLICKED, this);
        lv_obj_add_flag(ui_PebbleIconSettings, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(ui_PebbleIconSettings, LV_OBJ_FLAG_GESTURE_BUBBLE);
    }
    
    // 3. 物理演算後の位置を一度UIに反映
    for (auto& pebble : pebbles) {
        pebble->syncLvglObject();
    }
    
    lastUpdateTime = millis();
}

void MenuScene::update() {
    AlcedoPebble& pebble = AlcedoPebble::getInstance();
    HardwareManager& hw = pebble.getHardwareManager();

    // 1. 経過時間(dt)を計算
    unsigned long now = millis();
    float dt = (now - lastUpdateTime) / 1000.0f;
    lastUpdateTime = now;
    if (dt > 0.1f) dt = 0.1f;
    if (dt <= 0) return;

    // 2. IMUから重力ベクトルを取得
    ImuData imu = hw.getImuData();
    float gravityStrength = 2000.0f;
    Vec2D gravity = Vec2D(imu.accelX * gravityStrength, imu.accelY * gravityStrength);

    // 3. 物理エンジンを更新
    physicsEngine.update(pebbles, gravity, containerCenter, containerRadius, dt);

    // 4. 物理演算の結果をLVGLのUIオブジェクトに同期
    for (auto& pebble : pebbles) {
        pebble->syncLvglObject();
    }
    
    // 5. IMUによる操作検知
	constexpr float ACTIVITY_THRESHOLD = 1.5f; // (仮)
    if (std::abs(imu.accelX) > ACTIVITY_THRESHOLD || std::abs(imu.accelY) > ACTIVITY_THRESHOLD) {
        pebble.getPowerManager().notifyActivity();
    }
}

void MenuScene::onExit() {
    Serial0.println("02 MenuScene onExit");

    // 遅延タイマーが残っていれば削除
    if (eventEnableTimer) {
        lv_timer_del(eventEnableTimer);
        eventEnableTimer = nullptr;
    }

    // LVGLオブジェクトからイベントを削除
    if (ui_PebbleIconQR) lv_obj_remove_event_cb_with_user_data(ui_PebbleIconQR, pebble_event_cb, this);
    if (ui_PebbleIconGame) lv_obj_remove_event_cb_with_user_data(ui_PebbleIconGame, pebble_event_cb, this);
    if (ui_PebbleIconSettings) lv_obj_remove_event_cb_with_user_data(ui_PebbleIconSettings, pebble_event_cb, this);
    if (ui_MenuScreen) lv_obj_remove_event_cb_with_user_data(ui_MenuScreen, screen_event_cb, this);
    pebbles.clear();
}

// --- LVGL イベントコールバック (static) ---
void MenuScene::pebble_event_cb(lv_event_t * e) {
	// クリックされたらイベント伝播を止める
	// (背景の screen_event_cb が呼ばれるのを防ぐ)
	lv_event_stop_bubbling(e); 
    
	MenuScene* self = static_cast<MenuScene*>(lv_event_get_user_data(e));
    if (self && lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_t* objClicked = lv_event_get_target(e);
        self->onPebbleClicked(objClicked);
    }
}

void MenuScene::onPebbleClicked(lv_obj_t* objClicked) {
	AlcedoPebble::getInstance().getHardwareManager().vibrate(100);

	// どの小石がクリックされたか判定
	if (objClicked == ui_PebbleIconQR) {
		Serial.println("Pebble QR clicked! Changing to QrScene.");
		// (QrScene に遷移)
		AlcedoPebble::getInstance().getSceneManager().changeScene<QrScene>();
	}
	else if (objClicked == ui_PebbleIconGame) {
		Serial.println("Pebble Game clicked!");
		// (ゲームシーンに遷移)
	}
	else if (objClicked == ui_PebbleIconSettings) {
		Serial.println("Pebble Settings clicked!");
		// (設定シーンに遷移)
	}
}

void MenuScene::screen_event_cb(lv_event_t * e) {
    MenuScene* self = static_cast<MenuScene*>(lv_event_get_user_data(e));
    if (self && lv_event_get_code(e) == LV_EVENT_LONG_PRESSED) {
        self->onScreenClicked();
    }
}

void MenuScene::onScreenClicked() {
    Serial0.println("02 Screen clicked, returning to MainScene.");
    AlcedoPebble::getInstance().getSceneManager().changeScene<MainScene>();
}

void MenuScene::enable_events_timer_cb(lv_timer_t * timer) {
    MenuScene* self = static_cast<MenuScene*>(timer->user_data);
    if (self) {
        Serial.println("02 Long press events enabled for MenuScreen.");
        if (ui_MenuScreen) {
            lv_obj_add_event_cb(ui_MenuScreen, screen_event_cb, LV_EVENT_ALL, self);
        }
        self->eventEnableTimer = nullptr; // タイマーハンドルをクリア
    }
}