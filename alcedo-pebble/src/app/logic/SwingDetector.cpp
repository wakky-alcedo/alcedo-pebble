#include "app/logic/SwingDetector.hpp"

SwingDetector::SwingDetector() {
    reset();
}

void SwingDetector::reset() {
    state = State::IDLE;
    maxAccelNorm = 0.0f;
}

SwingResult SwingDetector::update(const ImuData& imu) {
    SwingResult result;
    result.detected = false;

    // 加速度の大きさ（ノルム）を計算 (重力含む)
    float norm = std::sqrt(imu.accelX * imu.accelX + imu.accelY * imu.accelY + imu.accelZ * imu.accelZ);

    switch (state) {
    case State::IDLE:
        // 閾値を超えたらスイング開始とみなす
        if (norm > THRESHOLD_START) {
            state = State::WAIT_PEAK;
            maxAccelNorm = norm;
            // 本来はここで姿勢（重力方向とのズレ）を記録して「水平に投げているか」を判定する
        }
        break;

    case State::WAIT_PEAK:
        // 最大加速度を更新
        if (norm > maxAccelNorm) {
            maxAccelNorm = norm;
        }

        // 加速度が落ち着いたらスイング終了とみなす
        if (norm < THRESHOLD_END) {
            state = State::IDLE;
            result.detected = true;
            
            // パワー計算 (最大5GくらいをMAXとする)
            float p = (maxAccelNorm - THRESHOLD_START) / (5.0f - THRESHOLD_START);
            if (p > 1.0f) p = 1.0f;
            if (p < 0.0f) p = 0.0f;
            result.power = p;

            // 安定性 (今回は簡易的に、Z軸方向のブレが少ないほど良いとするロジックなどが入る)
            // 仮実装: 常にナイススローとする
            result.stability = 1.0f; 
        }
        break;

    case State::COOLDOWN:
        break;
    }

    return result;
}