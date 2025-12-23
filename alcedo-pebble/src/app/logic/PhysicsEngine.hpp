/**
 * PhysicsEngine.hpp
 * @brief 小石オブジェクトの物理シミュレーションを行うエンジンのヘッダファイル
 */

#pragma once

#include "main.h"
#include "app/logic/Vec2D.hpp"

// --- Pebbleクラス ---
class Pebble {
public:
    // コンストラクタ
    Pebble(lv_obj_t* obj, Vec2D startPos, float radius);

    Vec2D pos;     // 現在の位置 (中心原点)
    Vec2D oldPos;  // 1フレーム前の位置
    Vec2D accel;   // 加速度
    float radius;  // 半径
    bool isDragging;

    void syncObj();
    void integrate(float dt);
	lv_obj_t* getObj() const { return obj; }

private:
	lv_obj_t* obj; // 対応するLVGLオブジェクト
};


// --- PhysicsEngineクラス ---
class PhysicsEngine {
public:
    PhysicsEngine(float containerRadius);
    ~PhysicsEngine();

    bool update(std::vector<std::unique_ptr<Pebble>>& pebbles, Vec2D gravity, float dt);

private:
    float containerRadius;
    float dtSq;
    
    void applyGravity(std::vector<std::unique_ptr<Pebble>>& pebbles, Vec2D gravity);
    void integrate(std::vector<std::unique_ptr<Pebble>>& pebbles, float dt);
    void solveCollisions(std::vector<std::unique_ptr<Pebble>>& pebbles);
    bool applyContainerConstraints(std::vector<std::unique_ptr<Pebble>>& pebbles);
};