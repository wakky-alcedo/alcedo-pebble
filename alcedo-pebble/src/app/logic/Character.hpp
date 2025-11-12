#pragma once

#include "main.h"

class Character {
public:
    Character(lv_obj_t* lvObject);
    ~Character();

    void init();
    void update(const ImuData& imu);

private:
    lv_obj_t* lvCharacterImage; // SLSで作成された画像オブジェクト
    
    enum class State {
        Idle,
        Tilting,
        Touched
    };
    State currentState;
    unsigned long stateStartTime = 0;

    // LVGLイベントコールバック
    static void character_event_cb(lv_event_t * e);
    void onTouched();
};