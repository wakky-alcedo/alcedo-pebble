#pragma once

#include "main.h"

// スイング判定結果
struct SwingResult {
    bool detected = false; // スイングが確定したか
    float power = 0.0f;    // スイングの強さ (0.0 - 1.0)
    float stability = 0.0f; // 角度の安定性 (0.0 - 1.0, 1.0がベスト)
};

// --- SwingDetectorクラス ---
// 「振る」動作を検知するロジック
class SwingDetector {
public:
    SwingDetector();
    
    // 測定開始（構え状態にする）
    void reset();
    
    // 毎フレーム更新。スイングが終わったら結果を返す
    SwingResult update(const ImuData& imu);

private:
    enum class State {
        IDLE,       // 待機中
        WAIT_PEAK,  // 加速中（ピーク待ち）
        COOLDOWN    // 判定後のクールダウン
    };
    State state = State::IDLE;

    float maxAccelNorm = 0.0f; // 最大加速度（ノルム）
    float startZAngle = 0.0f;  // 開始時の角度
    
    // 閾値定数
    const float THRESHOLD_START = 1.5f; // 開始閾値 (1.5G)
    const float THRESHOLD_END = 0.8f;   // 終了閾値 (0.8G)
};