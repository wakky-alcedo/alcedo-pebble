#pragma once

#include "main.h"

// --- BleManagerクラス ---
class BleManager {
public:
    BleManager();
    ~BleManager();
    void init(); // BLEの初期化とサービス定義
    void update(); // BLEイベントの処理

    bool isConnected();
    time_t getCurrentTime();
    WeatherData getWeatherData();
    NotificationData getLatestNotification();
    void clearNotification();

private:
    bool connected = false;
    time_t currentTime = 0;
    WeatherData weatherData;
    NotificationData notificationData;

    // (BLEライブラリ用のコールバッククラスや変数)
};