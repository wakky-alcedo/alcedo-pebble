#include "BleManager.hpp"
// (BLEライブラリのヘッダ)

BleManager::BleManager() :
    connected(false),
    currentTime(1678886400) // (仮の初期時刻)
{}

BleManager::~BleManager() {}

void BleManager::init() {
    Serial0.println("BleManager init...");
    // (BLE初期化処理のスタブ)
    Serial0.println("BLE Advertising started...");
}

void BleManager::update() {
    // (BLEイベントのポーリング)
}

bool BleManager::isConnected() {
    return connected;
}

time_t BleManager::getCurrentTime() {
    static unsigned long lastMillis = 0;
    if (millis() - lastMillis > 1000) {
        currentTime++;
        lastMillis = millis();
    }
    return currentTime;
}

WeatherData BleManager::getWeatherData() {
    return weatherData;
}

NotificationData BleManager::getLatestNotification() {
    return notificationData;
}

void BleManager::clearNotification() {
    notificationData.count = 0;
    notificationData.title = "";
}