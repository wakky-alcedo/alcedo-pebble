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
    int skipCount = 0;

    // UIパーツへのポインタ (新しい名前に変更)
    lv_obj_t* lvStoneImage = nullptr;
    lv_obj_t* lvScoreLabel = nullptr;
    lv_obj_t* lvMessageLabel = nullptr;

    unsigned long animStartTime = 0;
    float animStoneX = 0;
    float animStoneY = 0;

    void updateAnimation();
    void showResult();
    
    static void screen_event_cb(lv_event_t * e);
};