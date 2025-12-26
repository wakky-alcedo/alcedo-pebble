#pragma once

#include "main.h"

// スイング判定結果
struct SwingResult {
    bool detected = false;
    float power = 0.0f;     // スイングの強さ[G]]
    float spin = 0.0f;      // 回転数[dps]
    float stability = 0.0f; // 軸の安定性 (0.0 - 1.0, 1.0がベスト)
    int estimatedSkips = 0; // 推定スキップ回数
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

    float maxAccelNorm = 0.0f; // 最大加速度（ノルム）[G]
    float maxGyroZ = 0.0f;        // 最大回転力[dps]
    float accumGyroXY = 0.0f;     // ブレの蓄積[dps]
    int sampleCount = 0;          // サンプル数
    
    // 閾値定数（ジャイロZ軸）
    const float THRESHOLD_START = 200.0f; // 開始閾値 (200 dps)
    const float THRESHOLD_END = 50.0f;    // 終了閾値 (50 dps)
};