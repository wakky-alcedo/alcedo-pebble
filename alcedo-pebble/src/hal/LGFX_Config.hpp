#ifdef ARDUINO_ARCH_ESP32

#pragma once

//
// 【方法3: マニュアル設定】
// ご提供いただいたピン配置図とエラー画像に基づき，
// `_light_instance` の宣言漏れを修正しました．
//
// 使い方:
// 1. `src/DisplayManager.cpp` で `LGFX_AUTODETECT.hpp` をコメントアウトする．
// 2. 代わりに `#include "LGFX_Config.hpp"` のコメントを解除する．
//

#define LGFX_USE_V1

#include <LovyanGFX.hpp>

// ★ LCD仕様画像(image_d00fa2.png)で確認したチップを指定
// Display Chip: GC9A01A -> lgfx::Panel_GC9A01
// Touch Chip: CST816S -> lgfx::Touch_CST816 (互換)

class LGFX_Config : public lgfx::LGFX_Device
// class LGFX_Config : public lgfx::v1::LGFX_Device
{
    // ★ 各インスタンスをクラスメンバとして宣言する
    lgfx::Panel_GC9A01 _panel_instance;
    lgfx::Bus_SPI       _bus_instance;
    lgfx::Light_PWM     _light_instance; // <-- 【修正点】この行を追加
    lgfx::Touch_CST816S  _touch_instance;

public:
    LGFX_Config(void)
    {
        { // SPIバスの設定
            auto cfg = _bus_instance.config();
            cfg.spi_host = SPI3_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 40000000;
            cfg.freq_read  = 16000000;
            cfg.spi_3wire  = true;
            cfg.use_lock   = true;
            cfg.dma_channel = 1;

            // ★ ピン配置図(image_d0007d.png)に基づく設定
            cfg.pin_sclk = 10; // LCD_CLK
            cfg.pin_mosi = 11; // LCD_MOSI
            cfg.pin_miso = 12; // LCD_MISO (念のため設定)
            cfg.pin_dc   = 8;  // LCD_DC

            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        { // ディスプレイパネル(GC9A01A)の設定
            auto cfg = _panel_instance.config();

            // ★ ピン配置図に基づく設定
            cfg.pin_cs           = 9;  // LCD_CS
            cfg.pin_rst          = 14; // LCD_RST
            cfg.pin_busy         = -1;

            cfg.panel_width      = 240;
            cfg.panel_height     = 240;
            cfg.offset_x         = 0;
            cfg.offset_y         = 0;
            cfg.offset_rotation  = 0;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits  = 1;
            cfg.readable         = true;
            cfg.invert           = true; // (円形ディスプレイは通常 Invert)
            cfg.rgb_order        = false; // RGB
            cfg.bus_shared       = true;

            _panel_instance.config(cfg);
        }

        { // バックライトの設定
            // ★ `_light_instance` が宣言されたので，エラーは解消されます
            auto cfg = _light_instance.config();
            
            // ★ ピン配置図に基づく設定
            cfg.pin_bl = 2;    // LCD_BL
            cfg.invert = false;
            cfg.freq   = 44100;
            cfg.pwm_channel = 7;
            
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        { // タッチスクリーン(CST816S)の設定
            auto cfg = _touch_instance.config();
            
            cfg.x_min = 0;
            cfg.x_max = 239;
            cfg.y_min = 0;
            cfg.y_max = 239;
            
            // ★ ピン配置図に基づく設定
            cfg.pin_int = 5;   // TP_INT
            cfg.pin_rst = 13;  // TP_RST
            cfg.i2c_port = 0;  // (I2C_NUM_0 or I2C_NUM_1)
            cfg.i2c_addr = 0x15; // (CST816のI2Cアドレス)
            cfg.pin_sda = 6;   // TP_SDA
            cfg.pin_scl = 7;   // TP_SCL
            cfg.freq = 400000; // 400kHz
			// cfg.freq = 100000; // 100kHz
            
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        setPanel(&_panel_instance);
    }
};

#endif