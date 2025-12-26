#include "app/scenes/HomeScene.hpp"
#include "app/AlcedoPebble.hpp"
#include "app/scenes/QrScene.hpp"
#include "app/scenes/SettingsScene.hpp"

/**
 * @brief HomeSceneのコンストラクタ
 */
HomeScene::HomeScene() {
    Serial.println("HomeScene constructor");
}

/**
 * @brief HomeSceneのデストラクタ
 */
HomeScene::~HomeScene() {
    Serial.println("HomeScene destroyed");
}

/**
 * @brief シーン開始時の処理
 * UIのロード、物理エンジンの初期化、イベントハンドラの登録を行う
 */
void HomeScene::onEnter() {
    Serial.println("HomeScene onEnter");

    // 1. SLSの画面をロード
    if (!ui_HomeScreen) ui_init(); 
    lv_scr_load(ui_HomeScreen);

    // 親画面のスクロールは無効化 (コンテナだけスクロールさせたい)
    lv_obj_set_scrollbar_mode(ui_HomeScreen, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(ui_HomeScreen, LV_OBJ_FLAG_SCROLLABLE);

    // 2. SLSで作ったコンテナとページを取得
    if (ui_ScrollContainer) {
        // ★修正: スナップモードを START (上端合わせ) に変更
        // CENTERだと計算誤差で中間に止まることがあるため
        lv_obj_set_scroll_snap_y(ui_ScrollContainer, LV_SCROLL_SNAP_START);
        lv_obj_add_flag(ui_ScrollContainer, LV_OBJ_FLAG_SCROLL_ONE);
        
        // ★追加: 余計なバウンドや親への伝播を防ぐ
        lv_obj_clear_flag(ui_ScrollContainer, LV_OBJ_FLAG_SCROLL_ELASTIC); 
        lv_obj_clear_flag(ui_ScrollContainer, LV_OBJ_FLAG_SCROLL_CHAIN); // 親へのスクロール伝播禁止
        
        // ★追加: パディングとボーダーを強制的に0にする (ズレ防止)
        lv_obj_set_style_pad_all(ui_ScrollContainer, 0, LV_PART_MAIN);
        lv_obj_set_style_border_width(ui_ScrollContainer, 0, LV_PART_MAIN);
        
        // イベント登録 (ページ切り替え検知用)
        lv_obj_add_event_cb(ui_ScrollContainer, scroll_event_cb, LV_EVENT_SCROLL_END, this);
    } else {
        Serial.println("ERROR: ui_ScrollContainer is NULL!");
    }

    // 3. キャラクター初期化 (ui_PageMainにあるはず)
    if (ui_CharacterImage) {
        character = std::make_unique<Character>(ui_CharacterImage);
    } else {
        Serial.println("WARN: ui_CharacterImage is NULL");
    }

    // 4. 物理エンジン初期化
    physicsEngine = std::make_unique<PhysicsEngine>(120.0f);
    
    // 1. 物理演算用のPebbleオブジェクトを生成
    pebbles.clear(); 
    float radius = 30.0f; // pebbleの半径
	if (ui_PebbleIconQR) {
        pebbles.push_back(std::make_unique<Pebble>(ui_PebbleIconQR, Vec2D(120, 80), radius));
    }
    if (ui_PebbleIconGame) {
        pebbles.push_back(std::make_unique<Pebble>(ui_PebbleIconGame, Vec2D(80, 150), radius));
    }
    if (ui_PebbleIconSettings) {
        pebbles.push_back(std::make_unique<Pebble>(ui_PebbleIconSettings, Vec2D(160, 150), radius));
    }

    // 2. イベント登録 & 初期位置合わせ
    for (auto& p : pebbles) {
        // ★重要: 初期位置を強制的に適用
        p->syncObj();

        if (p->getObj()) {
            lv_obj_add_event_cb(p->getObj(), pebble_event_cb, LV_EVENT_CLICKED, p.get());
            lv_obj_add_flag(p->getObj(), LV_OBJ_FLAG_CLICKABLE);
            // ジェスチャーバブルを無効化 (小石操作で画面スクロールさせない)
            lv_obj_clear_flag(p->getObj(), LV_OBJ_FLAG_GESTURE_BUBBLE);
        }
    }
}

/**
 * @brief 毎フレームの更新処理
 */
void HomeScene::update() {
    HardwareManager& hw = AlcedoPebble::getInstance().getHardwareManager();

	// IMUデータ取得
	ImuData imuData = hw.getImuData();

    // 現在のスクロール位置を取得
    if (!ui_ScrollContainer) return;

    lv_coord_t scrollY = lv_obj_get_scroll_y(ui_ScrollContainer);

    // 1. メイン画面の更新 (キャラクター)
    if (scrollY < 240) {
        if (character) character->update(imuData);
    }

    // 2. メニュー画面の更新 (物理演算)
    // 少しでも下にスクロールしたら演算開始 (閾値 10px)
    if (scrollY > 10) {

		// 1. 経過時間(dt)を計算
		unsigned long now = millis();
		float dt = (now - lastUpdateTime) / 1000.0f;
		lastUpdateTime = now;
		if (dt > 0.1f) dt = 0.1f;
		if (dt <= 0) return;

		// 2. 重力ベクトルをIMUから計算
		float gravityStrength = 2000.0f;
		Vec2D gravity = Vec2D(imuData.accelX * gravityStrength, imuData.accelY * gravityStrength);

		// 3. 物理エンジンを更新
		if (physicsEngine->update(pebbles, gravity, dt)) {
			// 壁に衝突した場合
			static unsigned long lastVib = 0;
			// 連続振動を防ぐため，前回の振動から150ms以上経過している場合のみ振動
			if (millis() - lastVib > 150 && !AlcedoPebble::getInstance().getPowerManager().isIdle()) { 
				hw.vibrate(24); // Sharp Tick 1 - 100%
				lastVib = millis();
			}
		}

		// 4. 物理演算の結果をLVGLのUIオブジェクトに同期
		for (auto& pebble : pebbles) {
			pebble->syncObj();
		}
    }
    
    // IMUによる操作検知 (参考コードの閾値 1.5f を採用)
    if (std::abs(imuData.accelX) > 1.5f || std::abs(imuData.accelY) > 1.5f) {
        AlcedoPebble::getInstance().getPowerManager().notifyActivity();
    }

    updateCommonUi();
}

/**
 * @brief 共通UI (時計、バッテリー) の更新
 */
void HomeScene::updateCommonUi() {
    BleManager& ble = AlcedoPebble::getInstance().getBleManager();
    auto battData = AlcedoPebble::getInstance().getPowerManager().getBatteryData();
    time_t now = ble.getCurrentTime();

    if (now != lastUpdateTime) {
        if (ui_TimeLabel) {
            struct tm* timeinfo = localtime(&now);
            if (timeinfo) lv_label_set_text_fmt(ui_TimeLabel, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
        }
        lastUpdateTime = now;
    }

    if (battData.percentage != lastBatteryPercentage) {
        if (ui_MainBatteryLabel) {
            lv_label_set_text_fmt(ui_MainBatteryLabel, "%d%%", battData.percentage);
        }
        lastBatteryPercentage = battData.percentage;
    }
}

/**
 * @brief シーン終了時の処理
 */
void HomeScene::onExit() {
    Serial.println("HomeScene onExit");
    pebbles.clear();
    
    if (ui_ScrollContainer) {
        lv_obj_remove_event_cb_with_user_data(ui_ScrollContainer, scroll_event_cb, this);
    }
}

// --- イベントハンドラ ---

/**
 * @brief スクロール完了イベント
 */
void HomeScene::scroll_event_cb(lv_event_t* e) {
    HomeScene* self = static_cast<HomeScene*>(lv_event_get_user_data(e));
    if (!self) return;

    Serial.println("Scroll Snap Finished");
    AlcedoPebble::getInstance().getHardwareManager().vibrate(24); // Sharp Tick
}

/**
 * @brief 小石クリックイベント
 */
void HomeScene::pebble_event_cb(lv_event_t* e) {
    Pebble* pebble = static_cast<Pebble*>(lv_event_get_user_data(e));
    if (!pebble) return;

    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_event_stop_bubbling(e);
        AlcedoPebble::getInstance().getHardwareManager().vibrate(24); // Sharp Tick
        
        if (pebble->getObj() == ui_PebbleIconQR) {
            AlcedoPebble::getInstance().getSceneManager().changeScene<QrScene>();
        } else if (pebble->getObj() == ui_PebbleIconSettings) {
            AlcedoPebble::getInstance().getSceneManager().changeScene<SettingsScene>();
        }
    }
}