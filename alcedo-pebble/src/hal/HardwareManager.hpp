#pragma once

// (使用するIMUライブラリのヘッダをインクルード)
#include <Wire.h>
#include <Arduino.h> 
#include "SensorQMI8658.hpp"
#include "main.h"

// --- HardwareManagerクラス ---
class HardwareManager {
public:
    HardwareManager();
    ~HardwareManager();
    void init(); // センサーの初期化
    void update(); // センサー値のポーリング

    void vibrate(int ms);
    ImuData getImuData();
    float getAccelX(); // 簡易アクセサ
    float getAccelY(); // 簡易アクセサ
    
    // PowerManagerがADCを使うために公開
    uint16_t readBatteryAdc();

private:
    ImuData currentImu;
    // (IMUライブラリのインスタンスなど)
    SensorQMI8658 imuSensor; 
};