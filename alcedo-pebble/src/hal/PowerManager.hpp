#pragma once

#include "HardwareManager.hpp"

// --- PowerManagerクラス ---
class PowerManager {
public:
    PowerManager();
    ~PowerManager();
    void init(HardwareManager* hw); // バッテリーADCのためにHWManagerが必要
    void update(); // バッテリー電圧の監視，アイドル検知

    float getBatteryVoltage();
    int getBatteryPercentage();
    void enterIdleMode();
    void exitIdleMode();
    void enterDeepSleep();
    void notifyActivity(); // 操作を検知したら呼ぶ

private:
    HardwareManager* hwManager; // ADC読み取りに使用
    bool isIdleMode = false;
    unsigned long lastActivityTime = 0;
    float currentVoltage = -1.0f;
    int currentPercentage = 0;
    
    static const unsigned long IDLE_TIMEOUT_MS = 30000; // 30秒
};