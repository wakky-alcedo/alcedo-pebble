/**
 * PhysicsEngine.cpp
 * @brief 小石オブジェクトの物理シミュレーションを行うエンジンの実装ファイル
 */

#include "PhysicsEngine.hpp"

// --- Pebble クラス実装 ---
/**
 * @brief Pebbleコンストラクタ
 * @param startPos 初期位置
 * @param radius 小石の半径
 * @param id 小石の識別ID
 */
Pebble::Pebble(Vec2D startPos, float radius, int id) 
    : pos(startPos), 
      oldPos(startPos), 
      accel(0, 0), 
      radius(radius), 
      id(id),
      lvglObject(nullptr) {}

/**
 * @brief Pebbleの位置を対応するLVGLオブジェクトに同期します。
 */
void Pebble::syncLvglObject() {
    if (lvglObject) {
        lv_obj_set_pos(lvglObject, 
                       static_cast<lv_coord_t>(pos.x),
                       static_cast<lv_coord_t>(pos.y));
    }
}

/**
 * @brief Pebbleの位置を時間積分で更新します。
 * @param dt 時間ステップ
 */
void Pebble::integrate(float dt) {
	const float damping = 0.99f;
    float dtSq = dt * dt;
    Vec2D velocity = pos - oldPos; // 厳密には，速度*dt
    Vec2D nextPos = pos + velocity * damping + accel * dtSq;
    oldPos = pos;
    pos = nextPos;
    accel = Vec2D(0, 0);
}

// --- PhysicsEngine クラス実装 ---
/**
 * @brief 物理エンジンの状態を更新します。
 * @param pebbles 小石オブジェクトのリスト
 * @param gravity 重力ベクトル
 * @param containerCenter コンテナの中心位置
 * @param containerRadius コンテナの半径
 * @param dt 時間ステップ
 * @return コンテナ制約が適用されたかどうか(衝突したかどうか)
 */
bool PhysicsEngine::update(std::vector<std::unique_ptr<Pebble>>& pebbles, Vec2D gravity, Vec2D containerCenter, float containerRadius, float dt) {
    if (pebbles.empty() || dt == 0) return false;

    for (auto& pebble : pebbles) {
        pebble->accel = pebble->accel + gravity;
    }
    for (auto& pebble : pebbles) {
        pebble->integrate(dt);
    }

    const int substeps = 4; // 衝突解決のサブステップ数
	bool collided = false;
    for (int i = 0; i < substeps; ++i) {
        solveCollisions(pebbles);
        collided = applyContainerConstraints(pebbles, containerCenter, containerRadius) || collided;
    }
    return collided;
}

/**
 * @brief Pebble同士の衝突を解決します。
 * @param pebbles 小石オブジェクトのリスト
 */
void PhysicsEngine::solveCollisions(std::vector<std::unique_ptr<Pebble>>& pebbles) {
    for (size_t i = 0; i < pebbles.size(); ++i) {
        Pebble& p1 = *pebbles[i];
        for (size_t k = i + 1; k < pebbles.size(); ++k) {
            Pebble& p2 = *pebbles[k];
            Vec2D delta = p1.pos - p2.pos;
            float distSq = delta.x * delta.x + delta.y * delta.y;
            float totalRadius = p1.radius + p2.radius;
            if (distSq < totalRadius * totalRadius && distSq > 0.001f) {
                float dist = std::sqrt(distSq);
                float overlap = (totalRadius - dist) * 0.5f;
                Vec2D correction = delta.normalized() * overlap;
                p1.pos = p1.pos + correction;
                p2.pos = p2.pos - correction;
            }
        }
    }
}

/**
 * @brief Pebbleがコンテナの境界内に収まるように制約を適用します。
 * @param pebbles 小石オブジェクトのリスト
 * @param containerCenter コンテナの中心位置
 * 
 * @return コンテナ制約が適用されたかどうか(衝突したかどうか)
 */
bool PhysicsEngine::applyContainerConstraints(std::vector<std::unique_ptr<Pebble>>& pebbles, Vec2D containerCenter, float containerRadius) {
    bool hardCollision = false; // 強い衝突があったか

    for (auto& p : pebbles) {
        Vec2D toPebble = p->pos - containerCenter;
        float dist = toPebble.length();
        float maxDist = containerRadius - p->radius; 

        if (dist > maxDist) {
            // 壁に接触している

            // 1. 衝撃の強さを計算する
            // 速度ベクトル (現在の移動量)
            Vec2D velocity = p->pos - p->oldPos;
            // 壁の法線ベクトル (中心から外向き)
            Vec2D normal = toPebble.normalized();
            
            // 速度と法線の内積をとる (壁に向かう速度成分)
            float impact = velocity.dot(normal);

            // 2. 閾値判定
            // impact > 0 : 壁に向かって動いている
            // impact > threshold : 一定以上の勢いでぶつかった
            if (impact > 0.1f) {
                hardCollision = true;
            }

            // 3. 位置補正 (押し出し)
            Vec2D correction = normal * (maxDist - dist);
            p->pos = p->pos + correction;
            
            // (オプション: 反発させるならここで oldPos を操作する)
        }
    }
    return hardCollision;
}