/**
 * PhysicsEngine.cpp
 * @brief 小石オブジェクトの物理シミュレーションを行うエンジンの実装ファイル
 */

#include "PhysicsEngine.hpp"

// --- Pebble クラス実装 ---
/**
 * @brief Pebbleコンストラクタ
 * @param obj 対応するLVGLオブジェクト
 * @param startPos 初期位置
 * @param radius 小石の半径
 */
Pebble::Pebble(lv_obj_t* obj, Vec2D startPos, float radius) 
    : pos(startPos), 
	  oldPos(startPos), 
	  accel(0, 0), 
	  radius(radius), 
	  obj(obj), 
	  isDragging(false) {}

/**
 * @brief Pebbleの位置を対応するLVGLオブジェクトに同期します。
 */
void Pebble::syncObj() {
    if (obj) {
        // LVGLの Align が CENTER の場合、(0,0) は画面中心、かつオブジェクトの中心基準になります。
        // そのため、画面半分の加算や半径の減算は不要です。
        lv_coord_t x = static_cast<lv_coord_t>(pos.x);
        lv_coord_t y = static_cast<lv_coord_t>(pos.y);
        lv_obj_set_pos(obj, x, y);
    }
}

/**
 * @brief Pebbleの位置を時間積分で更新します。
 * @param dt 時間ステップ
 */
void Pebble::integrate(float dt) {
    if (isDragging) return;
	const float damping = 0.95f;
    float dtSq = dt * dt;
    Vec2D velocity = pos - oldPos; // 厳密には，速度/dtだけど、次の式でも*dtしていないので相殺
    Vec2D nextPos = pos + velocity * damping + accel * dtSq;
    oldPos = pos;
    pos = nextPos;
    accel = Vec2D(0, 0);
}

// --- PhysicsEngine クラス実装 ---
/**
 * @brief PhysicsEngineコンストラクタ
 * @param containerRadius コンテナの半径
 */
PhysicsEngine::PhysicsEngine(float containerRadius) : containerRadius(containerRadius), dtSq(0.0f) {}
PhysicsEngine::~PhysicsEngine() {}

/**
 * @brief 物理エンジンの状態を更新します。
 * @param pebbles 小石オブジェクトのリスト
 * @param gravity 重力ベクトル
 * @param dt 時間ステップ
 * @return コンテナ制約が適用されたかどうか(衝突したかどうか)
 */
bool PhysicsEngine::update(std::vector<std::unique_ptr<Pebble>>& pebbles, Vec2D gravity, float dt) {
    dtSq = dt * dt; 
    applyGravity(pebbles, gravity);
    integrate(pebbles, dt);
    const int substeps = 4; // 衝突解決のサブステップ数
	bool collided = false;
    for (int i = 0; i < substeps; ++i) {
        solveCollisions(pebbles);
        collided = applyContainerConstraints(pebbles, dt) || collided;
    }
    return collided;
}

void PhysicsEngine::applyGravity(std::vector<std::unique_ptr<Pebble>>& pebbles, Vec2D gravity) {
    for (auto& pebble : pebbles) {
		if (!pebble->isDragging) {
			pebble->accel = pebble->accel + gravity * 0.1f; // 質量=0.1f と仮定
        }
    }
}

void PhysicsEngine::integrate(std::vector<std::unique_ptr<Pebble>>& pebbles, float dt) {
	for (auto& pebble : pebbles) {
        pebble->integrate(dt);
	}
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
            if (p1.isDragging || p2.isDragging) continue;
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
 * @param dt 時間ステップ
 * @return コンテナ制約が適用されたかどうか(衝突したかどうか)
 */
bool PhysicsEngine::applyContainerConstraints(std::vector<std::unique_ptr<Pebble>>& pebbles, float dt) {
    const Vec2D containerCenter(0.0f, 0.0f);
    bool hardCollision = false; // 強い衝突があったか

    for (auto& p : pebbles) {
        if (p->isDragging) continue;
        Vec2D toPebble = p->pos - containerCenter;
        float dist = toPebble.length();
        float maxDist = containerRadius - p->radius; 
        if (dist > maxDist) {
            // 壁に接触している

            // 1. 衝撃の強さを計算する
            // 速度ベクトル (現在の移動量)
            Vec2D velocity = p->pos - p->oldPos; // 厳密には，速度/dt
            // 壁の法線ベクトル (中心から外向き)
            Vec2D normal = toPebble.normalized();
            
            // 速度と法線の内積をとる (壁に向かう速度成分)
            float impact = velocity.dot(normal) / dt; // 速度は位置差分なのでdtで割る

            // 2. 閾値判定
            // impact > 0 : 壁に向かって動いている
            // impact > threshold : 一定以上の勢いでぶつかった
            if (impact > 10.0f) {
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