#ifdef ARDUINO_ARCH_ESP32
#include <Arduino.h>
#endif
#define LGFX_USE_V1
#include <hal/display_manager.h>
#include <ui/ui.h>
#include <config.h>

void setup() {
#ifdef ARDUINO_ARCH_ESP32
    Serial0.begin(115200);
    Serial0.println("Display Test 2 Start");
#endif

    // --- 初期化処理 ---
    display_init();       // ディスプレイとLVGL
#ifdef ARDUINO_ARCH_ESP32
    Serial0.println("Display initialized");
#endif

    ui_init();            // UIオブジェクトを作成 (display_initの後)
    // ui_events_init();     // UIイベントを関連付け (ui_initの後)
#ifdef ARDUINO_ARCH_ESP32
    Serial0.println("UI initialized");
#endif
}

void loop() {
    display_handler();
}


// -------------------------------------------
//  PCシミュレーション用の main()
//  (ESP32環境では Arduino framework が main() を提供)
// -------------------------------------------
#ifndef ARDUINO_ARCH_ESP32
int main(int argc, char *argv[])
{
    setup();

    // シミュレーションのメインループ
    while (1) {
        loop();
    }
    return 0;
}
#endif