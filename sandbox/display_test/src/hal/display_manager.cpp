#include "display_manager.h"
#include <LovyanGFX.hpp>
#include <config.h>
#include <lvgl.h>
#include <ui/ui.h>

// LovyanGFXのインスタンス (config.hで設定)
// #include <LGFX_Config.hpp>
// static LGFX_Config lcd;

// LVGL用バッファ
static const uint32_t screenWidth = 240;
static const uint32_t screenHeight = 240;
static lv_disp_draw_buf_t draw_buf;
// static lv_color_t buf[screenWidth * 10];
static lv_color_t buf[screenWidth * screenHeight / 10];

/**
 * @brief LVGL用コールバック: ディスプレイへの描画
 * @param disp LVGLのディスプレイドライバ
 * @param area 描画領域
 * @param color_p 描画する色データ
 */
void display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    // lcd.pushImage(area->x1, area->y1, w, h, color_p);
    
    lcd.startWrite();
    lcd.setAddrWindow( area->x1, area->y1, w, h );
    // lcd.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    lcd.writePixels((lgfx::rgb565_t *)&color_p->full, w * h );
    lcd.endWrite();

    lv_disp_flush_ready(disp);
}

/**
 * @brief LVGL用コールバック: タッチパネルの読み取り
 * @param indev LVGLの入力デバイスドライバ
 * @param data LVGLの入力デバイスデータ
 */
void touchpad_read(lv_indev_drv_t *indev, lv_indev_data_t *data) {
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
}

void display_init() {
    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

    lcd.begin();
    lcd.setRotation( 1 ); /* Landscape orientation, flipped */
    // lv_init();
    // lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

    // ディスプレイドライバの登録
    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = display_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // 入力デバイス（タッチ）ドライバの登録
    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    lv_indev_drv_register(&indev_drv);
}

void display_handler() {
    // lcd.initBus();
    lv_timer_handler(); // LVGLのタイマーハンドラを呼び出す
}