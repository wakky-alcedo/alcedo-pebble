/**
 * @file PowerManager.hpp
 * @brief 電源管理クラスの定義
 */
#pragma once
#include "HardwareManager.hpp"

/**
 * @brief バッテリーデータ構造体
 */
struct BatteryData {
    float voltage = -1.0f;
    int percentage = -1;
};

// --- PowerManagerクラス ---
class PowerManager {
public:
    PowerManager();
    ~PowerManager();
    void init(HardwareManager* hw); // バッテリーADCのためにHWManagerが必要
    void update(); // バッテリー電圧の監視，アイドル検知

    float getBatteryVoltage() const { return data.voltage; }
    int getBatteryPercentage() const { return data.percentage; }
	BatteryData getBatteryData() const { return data; }
    void enterIdleMode();
    void exitIdleMode();
    void enterDeepSleep();
    void notifyActivity(); // 操作を検知したら呼ぶ

private:
    HardwareManager* hwManager; // ADC読み取りに使用
    bool isIdleMode = false;
    unsigned long lastActivityTime = 0;
	BatteryData data;
    
    static const unsigned long IDLE_TIMEOUT_MS = 30000; // 30秒
};