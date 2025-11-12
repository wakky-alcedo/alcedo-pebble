#include "PowerManager.hpp"
#include "app/AlcedoPebble.hpp" // DisplayManagerにアクセスするため

PowerManager::PowerManager() : 
    hwManager(nullptr),
    isIdleMode(false), 
    lastActivityTime(0),
    currentVoltage(0.0f),
    currentPercentage(0)
{}

PowerManager::~PowerManager() {}

void PowerManager::init(HardwareManager* hw) {
    this->hwManager = hw;
    this->lastActivityTime = millis();
    Serial0.println("PowerManager init complete.");
}

void PowerManager::update() {
    if (isIdleMode) return; // アイドル中は更新しない

    unsigned long now = millis();

    // 1. バッテリー電圧の監視 (10秒ごとなど)
    if (now % 10000 < 20) { // (簡易的な10秒ごと処理)
        if (hwManager) {
            uint16_t adcValue = hwManager->readBatteryAdc();
            float measuredVoltage = (adcValue / 4095.0f) * 3.3f * 2.0f; // (仮の計算式)
            currentVoltage = currentVoltage * 0.9f + measuredVoltage * 0.1f;
            currentPercentage = (int)((currentVoltage - 3.2f) / (4.2f - 3.2f) * 100.0f);
            if (currentPercentage > 100) currentPercentage = 100;
            if (currentPercentage < 0) currentPercentage = 0;
        }
    }

    // 2. 自動アイドルモードへの移行チェック
    if (now - lastActivityTime > IDLE_TIMEOUT_MS) {
        Serial0.println("Idle timeout. Entering Idle Mode.");
        enterIdleMode();
    }
}

float PowerManager::getBatteryVoltage() {
    return currentVoltage;
}

int PowerManager::getBatteryPercentage() {
    return currentPercentage;
}

void PowerManager::enterIdleMode() {
    isIdleMode = true;
    AlcedoPebble::getInstance().getDisplayManager().setBrightness(0); // バックライトOFF
    // (ESP32 Light Sleep移行処理)
}

void PowerManager::exitIdleMode() {
    isIdleMode = false;
    lastActivityTime = millis();
    AlcedoPebble::getInstance().getDisplayManager().setBrightness(100); // バックライトON
}

void PowerManager::enterDeepSleep() {
    Serial0.println("Entering Deep Sleep. Bye.");
    // (ESP32 Deep Sleep移行処理)
}

void PowerManager::notifyActivity() {
    if (isIdleMode) {
		Serial0.println("Activity detected. Exiting Idle Mode.");
        exitIdleMode();
    }
    lastActivityTime = millis();
}