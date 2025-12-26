#pragma once

#include "main.h"
#include "app/Scene.hpp"
#include "app/logic/SwingDetector.hpp"

// --- PebbleSkipSceneクラス ---
// 水切りゲーム
class PebbleSkipScene : public Scene {
public:
    PebbleSkipScene();
    virtual ~PebbleSkipScene();

    virtual void onEnter() override;
    virtual void update() override;
    virtual void onExit() override;

private:
    enum class State {
        READY,
        THROWN,
        RESULT
    };
    State state = State::READY;

    std::unique_ptr<SwingDetector> swingDetector;
    
    // 結果データ
    int skipCount = 0;
    float resPower = 0.0f;
    float resSpin = 0.0f;
    float resStability = 0.0f;

    // UIパーツへのポインタ
    lv_obj_t* lvStoneImage = nullptr;
    lv_obj_t* lvScoreLabel = nullptr;
    lv_obj_t* lvMessageLabel = nullptr;

    // アニメーション用
    unsigned long animStartTime = 0;
    
    // ★修正: static変数をメンバ変数に変更 (バグ修正)
    int currentBounce = 0;
    unsigned long nextBounceTime = 0;

    void updateAnimation();
    void showResult();
    
    static void screen_event_cb(lv_event_t * e);
};