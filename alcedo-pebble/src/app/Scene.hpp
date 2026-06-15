#pragma once

// --- シーン基底クラス ---
class Scene {
public:
    virtual ~Scene() = default;
    virtual void onEnter() = 0; // シーン開始時
    virtual void update() = 0; // 毎フレームの更新
    virtual void onExit() = 0; // シーン終了時
};