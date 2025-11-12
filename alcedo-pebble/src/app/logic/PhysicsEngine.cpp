#include "PhysicsEngine.hpp"

// --- Pebble クラス実装 ---
Pebble::Pebble(Vec2D startPos, float radius, int id) 
    : pos(startPos), 
      oldPos(startPos), 
      accel(0, 0), 
      radius(radius), 
      id(id),
      lvglObject(nullptr) {}

void Pebble::syncLvglObject() {
    if (lvglObject) {
        lv_obj_set_pos(lvglObject, 
                       static_cast<lv_coord_t>(pos.x - radius), 
                       static_cast<lv_coord_t>(pos.y - radius));
    }
}

void Pebble::integrate(float dt) {
    float dtSq = dt * dt;
    Vec2D velocity = pos - oldPos;
    Vec2D nextPos = pos + velocity + accel * dtSq;
    oldPos = pos;
    pos = nextPos;
    accel = Vec2D(0, 0);
}

// --- PhysicsEngine クラス実装 ---
void PhysicsEngine::update(std::vector<std::unique_ptr<Pebble>>& pebbles, Vec2D gravity, Vec2D containerCenter, float containerRadius, float dt) {
    if (pebbles.empty() || dt == 0) return;

    for (auto& pebble : pebbles) {
        pebble->accel = pebble->accel + gravity;
    }
    for (auto& pebble : pebbles) {
        pebble->integrate(dt);
    }

    const int substeps = 4; 
    for (int i = 0; i < substeps; ++i) {
        solveCollisions(pebbles);
        applyContainerConstraints(pebbles, containerCenter, containerRadius);
    }
}

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

void PhysicsEngine::applyContainerConstraints(std::vector<std::unique_ptr<Pebble>>& pebbles, Vec2D containerCenter, float containerRadius) {
    for (auto& pebblePtr : pebbles) {
        Pebble& pebble = *pebblePtr;
        Vec2D toPebble = pebble.pos - containerCenter;
        float dist = toPebble.length();
        float maxDist = containerRadius - pebble.radius;
        if (dist > maxDist) {
            Vec2D correction = toPebble.normalized() * (maxDist - dist);
            pebble.pos = pebble.pos + correction;
        }
    }
}