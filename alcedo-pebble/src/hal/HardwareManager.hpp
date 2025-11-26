#pragma once

// (使用するIMUライブラリのヘッダをインクルード)
#include <Wire.h>
#include <Arduino.h> 
#include "SensorQMI8658.hpp"
#include "Adafruit_DRV2605.h"
#include "main.h"

// --- HardwareManagerクラス ---
class HardwareManager {
public:
    HardwareManager();
    ~HardwareManager();
    void init(); // センサーの初期化
    void update(); // センサー値のポーリング

	// バイブレーション制御
    void vibrate(uint8_t effect);

	// IMU
    ImuData getImuData();
    float getAccelX(); // 簡易アクセサ
    float getAccelY(); // 簡易アクセサ
    
    // PowerManagerがADCを使うために公開
    uint16_t readBatteryAdc();

private:
    ImuData currentImu;
    // (IMUライブラリのインスタンスなど)
    SensorQMI8658 imuSensor; // QMI8658センサ
	Adafruit_DRV2605 vibeMotor; // バイブレーションモータ
};