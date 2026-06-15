/**
 * @file DisplayManager.h
 * @brief ディスプレイ(LovyanGFX)とGUIライブラリ(LVGL)の初期化と管理を行います。
 */
#pragma once

#include "main.h"
#include <hal/LGFX_Config.hpp>

// // LGFXの型を前方宣言
// namespace lgfx {
//     class LGFX_Device;
//     using LGFX = LGFX_Device;
// }

// --- DisplayManagerクラス ---
class DisplayManager {
public:
    DisplayManager();
    ~DisplayManager();
    void init(); // LVGLとSLS UIの初期化
    void update(); // lv_timer_handler() の呼び出し
    void setBrightness(uint8_t percentage);
    
    // LGFXインスタンスへの参照を公開（稀に必要）
    lgfx::LGFX_Device* getLgfx();

private:
    std::unique_ptr<lgfx::LGFX_Device> lgfx;
};
