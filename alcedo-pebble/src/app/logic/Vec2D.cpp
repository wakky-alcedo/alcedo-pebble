#include "Vec2D.hpp"
#include <cmath>

Vec2D::Vec2D(float x_, float y_) : x(x_), y(y_) {}

Vec2D Vec2D::operator+(const Vec2D& other) const { 
    return Vec2D(x + other.x, y + other.y); 
}

Vec2D Vec2D::operator-(const Vec2D& other) const { 
    return Vec2D(x - other.x, y - other.y); 
}

Vec2D Vec2D::operator*(float scalar) const { 
    return Vec2D(x * scalar, y * scalar); 
}

float Vec2D::length() const { 
    return std::sqrt(x * x + y * y); 
}

/**
 * @brief ベクトルを正規化します。
 * @return 長さが1の正規化されたベクトル
 */
Vec2D Vec2D::normalized() const { 
    float l = length(); 
    return (l > 0) ? Vec2D(x / l, y / l) : Vec2D(0, 0); 
}

/**
 * @brief 内積を計算します。
 * @param other もう一方のベクトル
 * @return 内積の値
 */
float Vec2D::dot(const Vec2D& other) const {
	return x * other.x + y * other.y;
}