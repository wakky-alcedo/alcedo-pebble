#include "app/scenes/PebbleSkipScene.hpp"
#include "app/AlcedoPebble.hpp"
#include "app/scenes/HomeScene.hpp"
#include "ui/ui.h"

PebbleSkipScene::PebbleSkipScene() {
    Serial0.println("PebbleSkipScene constructor");
}

PebbleSkipScene::~PebbleSkipScene() {
    Serial0.println("PebbleSkipScene destroyed");
}

void PebbleSkipScene::onEnter() {
    Serial0.println("PebbleSkipScene onEnter");

    if (ui_SkipScreen) {
        lv_scr_load(ui_SkipScreen);
        lvStoneImage = ui_SkipStoneImage;
        lvScoreLabel = ui_SkipScoreLabel;
        lvMessageLabel = ui_SkipMessageLabel;
    } else {
        // (SLS未反映時のダミー)
        lv_obj_t* scr = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(scr, lv_color_hex(0x87CEEB), 0);
        lv_scr_load(scr);
        
        lvMessageLabel = lv_label_create(scr);
        lv_label_set_text(lvMessageLabel, "SWING ME!");
        lv_obj_center(lvMessageLabel);
        
        lvScoreLabel = lv_label_create(scr);
        lv_label_set_text(lvScoreLabel, "");
        lv_obj_align(lvScoreLabel, LV_ALIGN_TOP_MID, 0, 20);

        lvStoneImage = lv_btn_create(scr); 
        lv_obj_set_size(lvStoneImage, 40, 40);
        lv_obj_set_style_radius(lvStoneImage, 20, 0);
        lv_obj_set_style_bg_color(lvStoneImage, lv_color_hex(0x555555), 0);
        lv_obj_align(lvStoneImage, LV_ALIGN_BOTTOM_MID, 0, -40);
        
        lv_obj_add_event_cb(scr, screen_event_cb, LV_EVENT_LONG_PRESSED, this);
    }

    swingDetector = std::make_unique<SwingDetector>();
    state = State::READY;
    skipCount = 0;
    
    // ★修正: アニメーション変数の初期化 (再プレイ時の不具合防止)
    currentBounce = 0;
    nextBounceTime = 0;
    
    // 石の初期位置リセット
    if (lvStoneImage) {
		lv_img_set_zoom(lvStoneImage, 256); // 初期サイズ
        lv_obj_align(lvStoneImage, LV_ALIGN_BOTTOM_MID, 0, -20); // 手前
        lv_img_set_angle(lvStoneImage, 0); // 角度リセット

		// ★修正: 回転時の欠け防止フラグを追加
        // lv_obj_add_flag(lvStoneImage, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    }

    if (lvMessageLabel) lv_label_set_text(lvMessageLabel, "READY...\nSWING!");
    if (lvScoreLabel) lv_label_set_text(lvScoreLabel, "");
    
    if (ui_SkipScreen) {
        lv_obj_add_event_cb(ui_SkipScreen, screen_event_cb, LV_EVENT_LONG_PRESSED, this);
    }
}

void PebbleSkipScene::update() {
    HardwareManager& hw = AlcedoPebble::getInstance().getHardwareManager();

    switch (state) {
    case State::READY: {
        ImuData imu;
        imu = hw.getImuData();
        SwingResult result = swingDetector->update(imu);

        if (result.detected) {
            Serial.printf("Swing! Pow:%.2f Spin:%.2f Stab:%.2f\n", 
                          result.power, result.spin, result.stability);
            
            // 結果を保存
            skipCount = result.estimatedSkips;
            resPower = result.power;
            resSpin = result.spin;
            resStability = result.stability;

            state = State::THROWN;
            animStartTime = millis();
            hw.vibrate(50); // キャスト音

            if (lvMessageLabel) lv_label_set_text(lvMessageLabel, "");
        }
        break;
    }
    case State::THROWN:
        updateAnimation();
        break;

    case State::RESULT:
        break;
    }
}

void PebbleSkipScene::updateAnimation() {
    unsigned long elapsed = millis() - animStartTime;
    float t = elapsed / 1000.0f; // 経過秒数

    if (lvStoneImage) {
        // 1. 移動アニメーション (減速しながら奥へ)
        // 初期Y: -20, 目標Y: -200 (画面上部)
        // t=0で0, t=3で1.0になるような減衰カーブ
        float progress = 1.0f - exp(-t * 1.5f); // 減速曲線
        int y = -20 - (int)(progress * 180.0f); 
        
        // 2. サイズ変更 (遠近法)
        // 60px -> 10px
        int size = 60 - (int)(progress * 50.0f);
        if (size < 10) size = 10;
        
        // 3. 回転アニメーション (高速回転)
        // 1秒間に2回転 (7200度/秒) * 時間
        int16_t angle = (int)(t * 7200) % 3600;

        // 反映
        lv_obj_set_y(lvStoneImage, y); 
		lv_img_set_zoom(lvStoneImage, (size * 256) / 60); // サイズ変更
        lv_img_set_angle(lvStoneImage, angle); // 0.1度単位
    }

    // 4. 跳ねるタイミングで振動
    // 最初の跳ねは間隔が広く、徐々に狭くなるのがリアル
    
    if (elapsed == 0) { // 初期化
        currentBounce = 0;
        nextBounceTime = 200; // 最初の着水まで200ms
    }

    if (skipCount > 0 && elapsed > nextBounceTime && currentBounce < skipCount) {
        AlcedoPebble::getInstance().getHardwareManager().vibrate(20); // 着水振動
        
        // 次のバウンドまでの時間を計算 (だんだん短くする)
        // 例: 300ms -> 270ms -> ...
        int interval = 300 - (currentBounce * 20);
        if (interval < 50) interval = 50; // 最小間隔
        
        nextBounceTime = elapsed + interval;
        currentBounce++;
    }

    // アニメーション終了 (3秒後 または 画面外に出たら)
    if (elapsed > 3000) {
        showResult();
    }
}

void PebbleSkipScene::showResult() {
    state = State::RESULT;
    
    // ★修正: 詳細情報の表示
    char buf[128];
    if (skipCount == 0) {
        sprintf(buf, "PLOP...\n(0 skips)\nPow[G]:%.1f\nSpin[dps]:%.1f\nStab:%.1f", 
				resPower, resSpin, resStability);
    } else {
        // 回転(Spin)と安定性(Stab)も表示
        sprintf(buf, "%d SKIPS!\nPow[G]: %.1f\nSpin[dps]: %.1f\nStab: %.1f", 
                skipCount, resPower, resSpin, resStability);
    }
    
    if (lvScoreLabel) lv_label_set_text(lvScoreLabel, buf);
    if (lvMessageLabel) lv_label_set_text(lvMessageLabel, "Long press to exit");
}

void PebbleSkipScene::onExit() {
    Serial0.println("PebbleSkipScene onExit");
    if (ui_SkipScreen) {
        lv_obj_remove_event_cb_with_user_data(ui_SkipScreen, screen_event_cb, this);
    }
}

void PebbleSkipScene::screen_event_cb(lv_event_t * e) {
    PebbleSkipScene* self = static_cast<PebbleSkipScene*>(lv_event_get_user_data(e));
    if (!self) return;

    if (lv_event_get_code(e) == LV_EVENT_LONG_PRESSED) {
        AlcedoPebble::getInstance().getSceneManager().changeScene<HomeScene>();
    }
}