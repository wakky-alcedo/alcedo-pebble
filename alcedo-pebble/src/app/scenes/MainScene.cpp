#include "MainScene.hpp"
#include "MenuScene.hpp" // メニューシーンへ遷移するため
#include "app/logic/Character.hpp" 
#include "app/AlcedoPebble.hpp"

MainScene::MainScene() : 
    lvCharacterImage(nullptr), 
    lvTimeLabel(nullptr),
    lvWeatherIcon(nullptr),
    lvNotificationIcon(nullptr),
    lastTimeUpdate(0),
    lastNotificationCount(0),
	eventEnableTimer(nullptr),
	lvMainBatteryLabel(nullptr),
    lastBatteryPercentage(-1)
{}

MainScene::~MainScene() {
    Serial0.println("01 MainScene destroyed");
}

void MainScene::onEnter() {
    Serial0.println("01 MainScene onEnter");

    if (ui_MainScreen) {
        lv_scr_load(ui_MainScreen);
    } else {
        Serial0.println("01 ERROR: ui_MainScreen is NULL!");
        return;
    }

    // SLSが生成したUIオブジェクトへのポインタを保存
    // (ui_MainScreen, ui_CharacterImage などは ui.h で宣言されている前提)
    this->lvCharacterImage = ui_CharacterImage; // (ui.h で宣言されているグローバル変数を代入)
    this->lvTimeLabel = ui_TimeLabel;
    this->lvWeatherIcon = ui_WeatherIcon;
    this->lvNotificationIcon = ui_NotificationIcon;
	this->lvMainBatteryLabel = ui_MainBatteryLabel;
    
    // キャラクタークラスを初期化
    character = std::make_unique<Character>(this->lvCharacterImage);
    Serial0.println("01 Character created");

    // UIの初期値を強制的に反映
    updateTimeLabel();
    updateWeatherIcon();
    updateNotificationIcon();
	updateBatteryLabel();

    // シーン遷移用のイベントハンドラをスクリーンに登録
	// スクリーン自体をクリック可能（イベントを受け取れるよう）にする
	lv_obj_add_flag(ui_MainScreen, LV_OBJ_FLAG_CLICKABLE);

	// lv_obj_add_event_cb(ui_MainScreen, main_screen_event_cb, LV_EVENT_LONG_PRESSED, this);
	// lv_obj_add_event_cb(ui_MainScreen, main_screen_event_cb, LV_EVENT_ALL, this);
	// thisを入れることで，lv_event_get_user_data(e)でMainSceneのインスタンスを取得可能にする
    // 200ms後にイベントを有効にするワンショットタイマーを作成
    eventEnableTimer = lv_timer_create(enable_events_timer_cb, EVENT_DEBOUNCE_TIME, this);
    lv_timer_set_repeat_count(eventEnableTimer, 1); // 1回だけ実行
}

void MainScene::update() {
    AlcedoPebble& pebble = AlcedoPebble::getInstance();
    HardwareManager& hw = pebble.getHardwareManager();
    BleManager& ble = pebble.getBleManager();

    // 1. センサーやBLEから最新情報を取得
    ImuData imu = hw.getImuData();
    time_t now = ble.getCurrentTime();
    NotificationData notification = ble.getLatestNotification();
    WeatherData weather = ble.getWeatherData();

    // 2. キャラクターロジックの更新
    if (character) {
        character->update(imu); 
    }

    // 3. UI更新（変更があった場合のみ）
    if (now != lastTimeUpdate) {
        updateTimeLabel();
        lastTimeUpdate = now;
    }
    if (weather.condition != lastWeatherCondition) {
        updateWeatherIcon();
        lastWeatherCondition = weather.condition;
    }
    if (notification.count != lastNotificationCount) {
        updateNotificationIcon();
        lastNotificationCount = notification.count;
    }
	updateBatteryLabel();	
    
    // 4. IMUによる操作検知
	constexpr float ACTIVITY_THRESHOLD = 1.5f; // (仮)
    if (std::abs(imu.accelX) > ACTIVITY_THRESHOLD || std::abs(imu.accelY) > ACTIVITY_THRESHOLD) {
        pebble.getPowerManager().notifyActivity();
    }
}

void MainScene::onExit() {
    Serial0.println("01 MainScene onExit");

	// 遅延タイマーが残っていれば削除
    if (eventEnableTimer) {
        lv_timer_del(eventEnableTimer);
        eventEnableTimer = nullptr;
    }

	// LVGLオブジェクトからイベントを削除
    character.reset(); 
    if (ui_MainScreen) {
       lv_obj_remove_event_cb_with_user_data(ui_MainScreen, main_screen_event_cb, this);
    }
}

// --- プライベートメソッド ---
void MainScene::updateTimeLabel() {
    if (!lvTimeLabel) return;
    BleManager& ble = AlcedoPebble::getInstance().getBleManager();
    time_t now = ble.getCurrentTime();
    struct tm* timeinfo = localtime(&now);
    char timeStr[6]; // "HH:MM\0"
    if (timeinfo) { strftime(timeStr, sizeof(timeStr), "%H:%M", timeinfo); } 
    else { strcpy(timeStr, "--:--"); }
    lv_label_set_text(lvTimeLabel, timeStr);
}

void MainScene::updateWeatherIcon() {
    if (!lvWeatherIcon) return;
    // (天候に応じて lv_img_set_src を実行するスタブ)
}

void MainScene::updateNotificationIcon() {
    if (!lvNotificationIcon) return;
    BleManager& ble = AlcedoPebble::getInstance().getBleManager();
    NotificationData notification = ble.getLatestNotification();
    if (notification.count > 0) {
        lv_obj_clear_flag(lvNotificationIcon, LV_OBJ_FLAG_HIDDEN); // 表示
        // (カテゴリに応じて lv_img_set_src を実行するスタブ)
    } else {
        lv_obj_add_flag(lvNotificationIcon, LV_OBJ_FLAG_HIDDEN); // 非表示
    }
}

/**
 * @brief バッテリーラベルの更新
 */
void MainScene::updateBatteryLabel() {
    // ラベルが存在しない場合は何もしない
    if (!lvMainBatteryLabel) return;

    // PowerManagerから現在のバッテリー情報を取得
    int percentage = AlcedoPebble::getInstance().getPowerManager().getBatteryPercentage();
    
    // 前回と値が変わったときだけテキストを更新（描画負荷を下げるため）
    if (percentage != lastBatteryPercentage) {
         lv_label_set_text_fmt(lvMainBatteryLabel, "%d%%", percentage);
         lastBatteryPercentage = percentage;
    }
}

// --- LVGL イベントコールバック (static) ---
void MainScene::main_screen_event_cb(lv_event_t * e) {
    MainScene* self = static_cast<MainScene*>(lv_event_get_user_data(e));
	if (self && lv_event_get_code(e) == LV_EVENT_LONG_PRESSED) {
        self->onScreenLongPress();
    }
}

void MainScene::onScreenLongPress() {
    AlcedoPebble& pebble = AlcedoPebble::getInstance();
    pebble.getPowerManager().notifyActivity(); // 操作を通知
    pebble.getSceneManager().changeScene<MenuScene>();
}

void MainScene::enable_events_timer_cb(lv_timer_t * timer) {
    MainScene* self = static_cast<MainScene*>(timer->user_data);
    if (self) {
        Serial.println("01 LONG_PRESSED event callback added to ui_MainScreen.");
        if (ui_MainScreen) {
            lv_obj_add_event_cb(ui_MainScreen, main_screen_event_cb, LV_EVENT_ALL, self);
        }
        self->eventEnableTimer = nullptr; // タイマーハンドルをクリア
    }
}