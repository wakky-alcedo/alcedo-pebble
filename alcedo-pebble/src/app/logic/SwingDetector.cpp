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
        if (accNorm > THRESHOLD_START) {
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

        // 終了判定
        if (accNorm < THRESHOLD_END) {
            state = State::IDLE;
            result.detected = true;
            
            // 1. パワー評価 (1.5G 〜 8.0G を 0.0 〜 1.0 に)
            // ★修正: 上限を5Gから8Gに上げ、簡単に満点が出ないようにする
            float p = (maxAccelNorm - THRESHOLD_START) / (8.0f - THRESHOLD_START);
            if (p > 1.0f) p = 1.0f;
            if (p < 0.0f) p = 0.0f;
            result.power = p;

            // 2. 回転評価 (2000 dps を MAX とする)
            // 手首のスナップが効いているほど高評価
            float s = maxGyroZ / 2000.0f; // QMI8658のスケール設定によるが、数千dps出る想定
            if (s > 1.0f) s = 1.0f;
            result.spin = s;

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
            float rawScore = (result.power * 10.0f) + (result.spin * 3.0f) + (result.stability * 2.0f);
            
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