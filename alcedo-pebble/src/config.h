/**
 * @file config.h
 * @brief プロジェクト全体のハードウェアピン設定や定数を定義します。
 */
#pragma once

// --- Hardware Pin Definitions ---
#define SD_CS_PIN 5  ///< SDカードのCS (Chip Select) ピン

// LovyanGFXのヘッダをインクルード（ここでピン設定を行うため）
#ifdef ARDUINO_ARCH_ESP32
#include <hal/LGFX_Config.hpp>
static LGFX_Config lcd;
#endif

// --- UART Settings ---
#define UART_INSTANCE UART_NUM_0    ///< 使用するUARTの番号 (UART0)
#define UART_RX_PIN 3      ///< UART0の受信(RX)ピン
#define UART_TX_PIN 1      ///< UART0の送信(TX)ピン
#define UART_BAUDRATE 115200 ///< UARTのボーレート

// --- Application Settings ---
#define LOG_FILE_PREFIX "/log_"            ///< ログファイルの接頭辞
#define LOG_FILE_EXTENSION ".csv"          ///< ログファイルの拡張子
#define SETTINGS_FILE "/settings.json"     ///< 設定ファイル名