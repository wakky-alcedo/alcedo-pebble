#include <Arduino.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <hal/display_manager.h>
#include <ui/ui.h>
#include <config.h>

void setup() {
    Serial0.begin(115200);
    Serial0.println("Display Test 2 Start");

    // --- 初期化処理 ---
    display_init();       // ディスプレイとLVGL
    Serial0.println("Display initialized");

    ui_init();            // UIオブジェクトを作成 (display_initの後)
    // ui_events_init();     // UIイベントを関連付け (ui_initの後)
    Serial0.println("UI initialized");
}

void loop() {
    display_handler();
}
