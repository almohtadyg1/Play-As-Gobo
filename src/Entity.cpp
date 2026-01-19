#include "Entity.hpp"
#include <stdexcept>
#include <algorithm>

namespace PlayAsGobo {

Entity::Entity(Vector2 center, float radius)
    : m_bounds(center, radius) {
    ValidateRadius(radius);
}

Entity::Entity(float x, float y, float radius)
    : m_bounds(x, y, radius) {
    ValidateRadius(radius);
}

void Entity::ValidateRadius(float radius) const {
    if (radius < MIN_RADIUS) {
        throw std::invalid_argument("Entity radius cannot be less than " + 
                                  std::to_string(MIN_RADIUS));
    }
    if (radius > MAX_RADIUS) {
        throw std::invalid_argument("Entity radius cannot be greater than " + 
                                  std::to_string(MAX_RADIUS));
    }
}

void Entity::SetPosition(Vector2 position) noexcept {
    m_bounds.center = position;
}

void Entity::SetPosition(float x, float y) noexcept {
    m_bounds.center.x = x;
    m_bounds.center.y = y;
}

void Entity::SetX(float x) noexcept {
    m_bounds.center.x = x;
}

void Entity::SetY(float y) noexcept {
    m_bounds.center.y = y;
}

void Entity::SetRadius(float radius) {
    ValidateRadius(radius);
    m_bounds.radius = radius;
}

void Entity::Move(Vector2 delta) noexcept {
    m_bounds.center.x += delta.x;
    m_bounds.center.y += delta.y;
}

void Entity::Move(float deltaX, float deltaY) noexcept {
    m_bounds.center.x += deltaX;
    m_bounds.center.y += deltaY;
}

void Entity::Jump() noexcept {
    m_velocityY = JUMP_FORCE;
    m_isOnGround = false;
}

} // namespace PlayAsGobo