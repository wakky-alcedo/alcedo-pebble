#include "ui_events.h"
#include "ui/ui.h"

std::vector<lv_obj_t*> log_list_buttons;

// void DropdownStartupScreenChange(lv_event_t * e) {
void settings_dropdown_startup_screen_handler(lv_event_t * e) {
    char buf[32];
    lv_dropdown_get_selected_str(ui_startupscreen, buf, sizeof(buf));
    // if (strcmp(buf, "Home") == 0) settings.startupScreen = "home";
    // else if (strcmp(buf, "Raw") == 0) settings.startupScreen = "raw";
    // else if (strcmp(buf, "Analytics") == 0) settings.startupScreen = "analytics";
    // save_settings();
}


static void log_list_event_handler(lv_event_t * e) {
    lv_obj_t* target = lv_event_get_target(e);
    // const char* file_name_cstr = lv_list_get_btn_text(ui_loglist, target);
    char file_name_cstr[50];
    lv_roller_get_selected_str(target, (char*)file_name_cstr, 50);
    // Serial.println("Selected log file: " + String(file_name_cstr));
    // if (String(file_name_cstr) == "Realtime") {
    //     current_mode = DisplayMode::LIVE;
    //     // Serial.println("Switching to LIVE mode");
    //     // Serial.println("Reading log file: " + currentLogFile);
    //     String content = read_log_file_content(currentLogFile);
    //     lv_textarea_set_text(ui_SerialPloter, content.c_str());
    // } else {
    //     current_mode = DisplayMode::PLAYBACK;
    //     // Serial.println("Switching to PLAYBACK mode");
    //     String file_name = "/" + String(file_name_cstr);
    //     // Serial.println("Reading log file: " + file_name);
    //     String content = read_log_file_content(file_name);
    //     lv_textarea_set_text(ui_SerialPloter, content.c_str());
    // }
}

// --- 初期化 ---
void ui_events_init() {
    // SquareLine Studioのイベント機能で Call Function を使った場合は不要
    // 手動で関連付ける場合はここに記述する
    lv_obj_add_event_cb(ui_startupscreen, settings_dropdown_startup_screen_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_loglist, log_list_event_handler, LV_EVENT_CLICKED, NULL);
}