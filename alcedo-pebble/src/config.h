/**
 * @file config.h
 * @brief プロジェクト全体のハードウェアピン設定や定数を定義します。
 */
#pragma once

// LovyanGFXのヘッダをインクルード（ここでピン設定を行うため）
#ifdef ARDUINO_ARCH_ESP32
#include <hal/LGFX_Config.hpp>
static LGFX_Config lcd;
#endif

