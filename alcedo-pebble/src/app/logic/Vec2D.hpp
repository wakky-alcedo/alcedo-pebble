#pragma once

// --- 2Dベクトルクラス（PhysicsEngine用） ---
class Vec2D {
public:
    float x, y;
    Vec2D(float x_ = 0.0f, float y_ = 0.0f);
    Vec2D operator+(const Vec2D& other) const;
    Vec2D operator-(const Vec2D& other) const;
    Vec2D operator*(float scalar) const;
    float length() const;
    Vec2D normalized() const;
};