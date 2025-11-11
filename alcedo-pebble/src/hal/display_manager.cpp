/**
 * @file display_manager.cpp
 * @brief ディスプレイ管理モジュール
 */
#include "display_manager.h"
#include <config.h>
#include <lvgl.h>
#include <ui/ui.h>

// LovyanGFXのインスタンス (config.hで設定)
// #include <LGFX_Config.hpp>
// static LGFX_Config lcd;
// --- プラットフォーム固有のインクルード ---
#ifdef ARDUINO_ARCH_ESP32
    // ==================
    //  ESP32実機 (LGFX)
    // ==================
    #include <LovyanGFX.hpp>
    #include <LGFX_Config.hpp> // ★ご自身のLGFX設定
    static LGFX_Config lcd;     // ★LGFXインスタンス

#else
    // ==================
    //  PCシミュレーション (SDL)
    // ==================
    // #include <SDL2/SDL.h>
    #include <SDL.h>
    #include <sdl/sdl.h>
    // #include "lv_drivers/sdl/sdl.h" // PlatformIO (native) 経由でインストール
#endif

// --- LVGL用バッファ (共通) ---
static const uint32_t screenWidth = 240;
static const uint32_t screenHeight = 240;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10]; // (buf2 を追加してダブルバッファにすると高速化します)

/**
 * @brief LVGL用コールバック: ディスプレイへの描画
 * @param disp LVGLのディスプレイドライバ
 * @param area 描画領域
 * @param color_p 描画する色データ
 */
// #ifdef ARDUINO_ARCH_ESP32
void display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
#ifdef ARDUINO_ARCH_ESP32
    // --- ESP32 (LGFX) の描画処理 ---
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    // lcd.pushImage(area->x1, area->y1, w, h, color_p);
    
    lcd.startWrite();
    lcd.setAddrWindow( area->x1, area->y1, w, h );
    // lcd.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    lcd.writePixels((lgfx::rgb565_t *)&color_p->full, w * h );
    lcd.endWrite();

    lv_disp_flush_ready(disp);
#else
    // --- PCシミュレーション (SDL) の描画処理 ---
    // sdl_display_flush(disp, area, color_p);
#endif
}
// #endif

/**
 * @brief LVGL用コールバック: タッチパネルの読み取り
 * @param indev LVGLの入力デバイスドライバ
 * @param data LVGLの入力デバイスデータ
 */
#ifdef ARDUINO_ARCH_ESP32
void touchpad_read(lv_indev_drv_t *indev, lv_indev_data_t *data) {
#ifdef ARDUINO_ARCH_ESP32
    // --- ESP32 (LGFX) のタッチ処理 ---
    uint16_t touchX = 0, touchY = 0;
    bool touched = lcd.getTouch(&touchX, &touchY);
    data->state = touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    if (touched) {
        data->point.x = touchX;
        data->point.y = touchY;
        data->state = LV_INDEV_STATE_PR;

        // Serial.print( "Data x " );
        // Serial.println( touchX );

        // Serial.print( "Data y " );
        // Serial.println( touchY );
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
#else
    // --- PCシミュレーション (SDL) のマウス処理 ---
    sdl_mouse_read(indev, data);
#endif
}
#endif

/**
 * @brief ディスプレイとLVGLの初期化
 */
void display_init() {
    lv_init(); // 共通

#if LV_USE_LOG != 0
    // my_print(); // (もし my_print が Serial を使っているなら、それも #ifdef で囲う)
#endif

#ifdef ARDUINO_ARCH_ESP32
    // --- ESP32 (LGFX) の初期化 ---
    lcd.begin();
    lcd.setRotation( 1 ); 
    
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

    // ディスプレイドライバの登録
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = display_flush; // ★ 実機専用コールバック
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // 入力デバイス（タッチ）ドライバの登録
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read; // ★ 実機専用コールバック
    lv_indev_drv_register(&indev_drv);
#else
    // --- PCシミュレーション (SDL) の初期化 (LVGL v8 内蔵) ---
    // (SDL_Init() は lv_sdl_window_create の中で自動的に呼ばれます)
    
    // 画面解像度は lv_conf.h の LV_HOR_RES_MAX / LV_VER_RES_MAX (例: 240x240) で設定
    // ★ これが v8 の新しい関数です
    // lv_disp_t* disp = lv_sdl_window_create(LV_HOR_RES_MAX, LV_VER_RES_MAX);
    // lv_disp_t* disp = lv_disp_drv_register(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);
    // static lv_disp_drv_t disp_drv;
    // lv_disp_drv_init(&disp_drv);
    // disp_drv.hor_res = screenWidth;
    // disp_drv.ver_res = screenHeight;
    // // disp_drv.flush_cb = display_flush; // ★ 実機専用コールバック
    // disp_drv.draw_buf = &draw_buf;
    // lv_disp_t* disp = lv_disp_drv_register(&disp_drv);
    // if (disp == NULL) {
    //     return; 
    // }
    sdl_init();
    
    // マウス入力を作成
    // ★ これが v8 の新しい関数です
    // lv_indev_t* mouse = lv_sdl_mouse_create();
    // if (mouse == NULL) {
    //     return;
    // }
#endif
}

/**
 * @brief ディスプレイの周期処理 (共通)
 */
void display_handler() {
    lv_timer_handler(); // LVGLのタイマーハンドラを呼び出す

    #ifndef ARDUINO_ARCH_ESP32
    SDL_Delay(5);
    #endif
}