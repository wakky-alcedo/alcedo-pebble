#include "PowerManager.hpp"
#include "app/AlcedoPebble.hpp" // DisplayManagerにアクセスするため

PowerManager::PowerManager() : 
    hwManager(nullptr),
    isIdleMode(false), 
    lastActivityTime(0),
	data()
{}

PowerManager::~PowerManager() {}

void PowerManager::init(HardwareManager* hw) {
    this->hwManager = hw;
    this->lastActivityTime = millis();
    Serial0.println("PowerManager init complete.");
}

void PowerManager::update() {
    // if (isIdleMode) return; // アイドル中は更新しない

    unsigned long now = millis();

    // 1. バッテリー電圧の監視 (10秒ごとなど)
    if (now % 1000 == 0) { // (簡易的な10秒ごとに20回処理)
        if (hwManager) {
            uint16_t adcValue = hwManager->readBatteryAdc();
			if (adcValue == 0) {
				Serial0.println("Battery Not Connected.");
			} else{
				float measuredVoltage = 3.3 / (1<<12) * 3 * adcValue; // ref: datasheet, 分圧比3倍
				if (data.voltage < 0) {
					data.voltage = measuredVoltage; // 初回はそのまま
				} else {
					data.voltage = data.voltage * 0.9f + measuredVoltage * 0.1f; // ローパスフィルタ
				}
				data.percentage = (int)((data.voltage - 3.2f) / (4.2f - 3.2f) * 100.0f);
				if (data.percentage > 100) data.percentage = 100;
				if (data.percentage < 0) data.percentage = 0;
				// Serial0.printf("Battery Voltage: %.2f V, Percentage: %d%%\n", data.voltage, data.percentage);
			}
        }
    }

    // 2. 自動アイドルモードへの移行チェック
    if (now - lastActivityTime > IDLE_TIMEOUT_MS) {
        Serial0.println("Idle timeout. Entering Idle Mode.");
        enterIdleMode();
    }

	// 3. バッテリーが20%以下であればDeep Sleepへ移行
	if (data.percentage > 0 && data.percentage <= 20) {
		Serial0.println("Battery low. Entering Deep Sleep.");
		enterDeepSleep();
	}
}

void PowerManager::enterIdleMode() {
    isIdleMode = true;
    AlcedoPebble::getInstance().getDisplayManager().setBrightness(0); // バックライトOFF
    // (ESP32 Light Sleep移行処理)
}

void PowerManager::exitIdleMode() {
    isIdleMode = false;
    lastActivityTime = millis();
    AlcedoPebble::getInstance().getDisplayManager().setBrightness(30); // バックライトON todo 変数で管理
}

void PowerManager::enterDeepSleep() {
    Serial0.println("Entering Deep Sleep. Bye.");
    // (ESP32 Deep Sleep移行処理)
	esp_deep_sleep_start();
}

void PowerManager::notifyActivity() {
    if (isIdleMode) {
		Serial0.println("Activity detected. Exiting Idle Mode.");
        exitIdleMode();
    }
    lastActivityTime = millis();
}