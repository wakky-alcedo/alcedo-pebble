
#include "main.h"
#include <config.h>
#include "app/AlcedoPebble.hpp" // シングルトン本体

// シングルトンインスタンスへの参照
AlcedoPebble& pebble = AlcedoPebble::getInstance();

void setup() {
#ifdef ARDUINO_ARCH_ESP32
    Serial0.begin(115200);
#endif

    // --- 初期化処理 ---
	pebble.init();
}

void loop() {
	pebble.loop();
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