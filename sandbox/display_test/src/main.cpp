#include <lvgl.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <hal/display_manager.h>
#include <app/ui_events.h>
#include <config.h>
// #include <hal/LGFX_Config.hpp>
#include <ui/ui.h>

/*Don't forget to set Sketchbook location in File/Preferences to the path of your UI project (the parent foder of this INO file)*/

/*Change to your screen resolution*/

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    // Serial.printf(buf);
    Serial.flush();
}
#endif

void setup() {
    Serial.begin( 115200 ); // prepare for possible serial debug
    Serial.println("Setup started");

    String LVGL_Arduino = "LVGL_Arduino ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
    Serial.println( LVGL_Arduino );

    // --- 初期化処理 ---
    display_init();       // ディスプレイとLVGL
    Serial.println("Display initialized");

    ui_init();            // UIオブジェクトを作成 (display_initの後)
    ui_events_init();     // UIイベントを関連付け (ui_initの後)
    Serial.println("UI initialized");
    lv_textarea_add_text(ui_debugmonitor, "UI initialized\n");

    Serial.println( "Setup done" );
    lv_textarea_add_text(ui_debugmonitor, "Setup done\n");
}

void loop()
{
    // lv_timer_handler(); /* let the GUI do its work */

    // if (SerialPort.available()) {
    //     String receivedData = SerialPort.readStringUntil('\n');
    //     processUartData(receivedData);
    // }

    // sprintf(chbuff, "%02d:%02d", tm->tm_hour, tm->tm_min);
    // lv_label_set_text(uic_sample, chbuff);
    // lv_label_set_text(uic_aa, chbuff);
    // lv_label_get

    display_handler();
}

