#pragma once

// C/C++ 標準ライブラリ
#include <memory> // std::unique_ptr
#include <type_traits> // std::remove_pointer
#include <string>
#include <ctime>
#include <vector>
#include <cmath>

// Arduino / ESP-IDF
#ifdef ARDUINO_ARCH_ESP32
#include <Arduino.h>
#endif
#define LGFX_USE_V1

// LVGL (lv_conf.h がインクルードパスにある前提)
#include <lvgl.h>

#include "hal/DisplayManager.hpp"

// --- クラス前方宣言 ---
// (各ヘッダで相互参照を防ぐため)

// HAL
class DisplayManager;
class HardwareManager;
class PowerManager;
class BleManager;

// App
class AlcedoPebble;
class SceneManager;
class Scene;
class Character;
class PhysicsEngine;
class Pebble;
class Vec2D;

// --- グローバルなデータ構造体 ---
// (これらも専用ヘッダに分離可能だが，main.hに置いてもよい)

struct WeatherData {
    std::string condition = "N/A";
    int temperature = 0;
};
struct NotificationData {
    enum class Category { Other, IncomingCall, MissedCall, Social, Email };
    Category category = Category::Other;
    std::string title = "";
    int count = 0;
};
struct ImuData {
    float accelX = 0;
    float accelY = 0;
    float accelZ = 0;
    float gyroX = 0;
    float gyroY = 0;
    float gyroZ = 0;
};