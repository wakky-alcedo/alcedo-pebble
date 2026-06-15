#include "app/logic/SwingDetector.hpp"

SwingDetector::SwingDetector() {
    reset();
}

void SwingDetector::reset() {
    state = State::IDLE;

    maxAccelNorm = 0.0f;
    maxGyroZ = 0.0f;
    accumGyroXY = 0.0f;
    sampleCount = 0;
}

SwingResult SwingDetector::update(const ImuData& imu) {
    SwingResult result;
    result.detected = false;

    // 加速度の大きさ (重力含む)
    float accNorm = std::sqrt(imu.accelX * imu.accelX + imu.accelY * imu.accelY + imu.accelZ * imu.accelZ);
    // 回転の強さ (Z軸回転 = 手首のスナップ)
    float gyroZ = std::abs(imu.gyroZ);
    // ブレの強さ (X, Y軸回転 = 無駄な動き)
    float gyroXY = std::sqrt(imu.gyroX * imu.gyroX + imu.gyroY * imu.gyroY);

    switch (state) {
    case State::IDLE:
        if (gyroZ > THRESHOLD_START) {
            state = State::WAIT_PEAK;
            maxAccelNorm = accNorm;
            maxGyroZ = gyroZ;
            accumGyroXY = gyroXY;
            sampleCount = 1;
        }
        break;

    case State::WAIT_PEAK:
        // 最大値を更新
        if (accNorm > maxAccelNorm) maxAccelNorm = accNorm;
        if (gyroZ > maxGyroZ) maxGyroZ = gyroZ;
        
        // ブレを積算
        accumGyroXY += gyroXY;
        sampleCount++;

        // 終了判定（ジャイロZ軸が閾値を下回ったら）
        if (gyroZ < THRESHOLD_END) {
            state = State::IDLE;
            result.detected = true;
            
            // 1. パワー評価 (1.5G 〜 8.0G を 0.0 〜 1.0 に)
            // ★修正: 上限を5Gから8Gに上げ、簡単に満点が出ないようにする
			result.power = maxAccelNorm;
			float max_power_g = 15.0f; // 最大パワーの基準値
            float power_score = (maxAccelNorm - THRESHOLD_START) / (max_power_g - THRESHOLD_START);
            if (power_score > 1.0f) power_score = 1.0f;
            if (power_score < 0.0f) power_score = 0.0f;

            // 2. 回転評価
            // 手首のスナップが効いているほど高評価
			result.spin = maxGyroZ;
			float max_spin_dps = 1000.0f; // 最大回転力の基準値
            float spin_score = maxGyroZ / max_spin_dps;
            if (spin_score > 1.0f) spin_score = 1.0f;

            // 3. 安定性評価 (ブレの平均値)
            // ブレが少ないほど 1.0 に近づく
            float avgWobble = (sampleCount > 0) ? (accumGyroXY / sampleCount) : 0.0f;
            // 500 dps 以上のブレで安定性0になる設定
            float stability = 1.0f - (avgWobble / 500.0f);
            if (stability < 0.0f) stability = 0.0f;
            result.stability = stability;

            // 4. 総合スコア計算
            // パワー重視だが、回転と安定性もボーナスとして加味
            // Base: Power * 10
            // Bonus: Spin * 3 + Stability * 2
            float rawScore = (power_score * 7.0f) + (spin_score * 2.0f) + (result.stability * 3.0f);
            
            // 係数を調整して 0 〜 15 くらいに収める
            result.estimatedSkips = (int)rawScore;
            
            // パワーが低すぎる場合は失敗
            if (result.power < 0.15f) result.estimatedSkips = 0;
            
            // 上限キャップ
            if (result.estimatedSkips > 99) result.estimatedSkips = 99;
        }
        break;

    case State::COOLDOWN:
        break;
    }

    return result;
}