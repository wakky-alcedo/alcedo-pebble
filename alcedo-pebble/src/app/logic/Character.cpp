#include "Character.hpp"
#include "app/AlcedoPebble.hpp" // HardwareManager にアクセスするため

Character::Character(lv_obj_t* lvObject) : 
    lvCharacterImage(lvObject),
    currentState(State::Idle),
    stateStartTime(0) 
{
    Serial0.println("Character constructor");
    if (lvCharacterImage) {
        lv_obj_add_event_cb(lvCharacterImage, character_event_cb, LV_EVENT_CLICKED, this);
        lv_obj_add_flag(lvCharacterImage, LV_OBJ_FLAG_CLICKABLE);
    }
    init();
}

Character::~Character() {
    Serial0.println("Character destructor");
    if (lvCharacterImage) {
       lv_obj_remove_event_cb_with_user_data(lvCharacterImage, character_event_cb, this);
    }
}

void Character::init() {
    if (!lvCharacterImage) return;
    // lv_img_set_src(lvCharacterImage, &img_char_idle);
    stateStartTime = millis();
}

void Character::update(const ImuData& imu) {
    if (!lvCharacterImage) return;

    unsigned long now = millis();
    // 状態遷移のロジック
    if (currentState == State::Touched) {
        if (now - stateStartTime > 500) { // 0.5秒でIdleに戻る
            currentState = State::Idle;
            stateStartTime = now;
            // lv_img_set_src(lvCharacterImage, &img_char_idle);
        }
        return; // タッチ中は他の処理をしない
    }
    
    float tilt = imu.accelX;
    if (tilt > 0.5f) { // 右に傾き
        if (currentState != State::Tilting) {
            currentState = State::Tilting;
            // lv_img_set_src(lvCharacterImage, &img_char_tilt_right);
        }
    } else if (tilt < -0.5f) { // 左に傾き
        if (currentState != State::Tilting) {
            currentState = State::Tilting;
            // lv_img_set_src(lvCharacterImage, &img_char_tilt_left);
        }
    } else { // アイドル状態
        if (currentState != State::Idle) {
            currentState = State::Idle;
            stateStartTime = now;
            // lv_img_set_src(lvCharacterImage, &img_char_idle);
        }
    }
}

// --- LVGL イベントコールバック (static) ---
void Character::character_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    Character* self = static_cast<Character*>(lv_event_get_user_data(e));

    if (code == LV_EVENT_CLICKED) {
        if (self) {
            self->onTouched();
        }
    }
}

void Character::onTouched() {
    if (currentState != State::Touched) {
        currentState = State::Touched;
        stateStartTime = millis();
        // lv_img_set_src(lvCharacterImage, &img_char_touched);
        
        // 振動（HardwareManager呼び出し）
        AlcedoPebble::getInstance().getHardwareManager().vibrate(30);
    }
}