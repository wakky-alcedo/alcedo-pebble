#pragma once

#include "main.h"
#include "app/logic/Vec2D.hpp"

// --- Pebbleクラス ---
class Pebble {
public:
    Pebble(Vec2D startPos, float radius, int id);

    Vec2D pos;     // 現在の位置
    Vec2D oldPos;  // 1フレーム前の位置
    Vec2D accel;   // 現在フレームでかかる加速度
    float radius;  // 小石の半径
    int id;        // どの機能かを識別するID
    
    lv_obj_t* lvglObject; // この小石に対応するLVGLのUIオブジェクト

    void syncLvglObject();
    void integrate(float dt);
};

// --- PhysicsEngineクラス ---
class PhysicsEngine {
public:
    void update(std::vector<std::unique_ptr<Pebble>>& pebbles, Vec2D gravity, Vec2D containerCenter, float containerRadius, float dt);

private:
    void solveCollisions(std::vector<std::unique_ptr<Pebble>>& pebbles);
    void applyContainerConstraints(std::vector<std::unique_ptr<Pebble>>& pebbles, Vec2D containerCenter, float containerRadius);
};