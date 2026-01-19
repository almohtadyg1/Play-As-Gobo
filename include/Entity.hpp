#pragma once

#include "raylib.h"
#include <cstdint>
#include <string>

namespace PlayAsGobo {

struct Circle {
    Vector2 center{0.0f, 0.0f};
    float radius{0.0f};
    
    Circle() = default;
    Circle(Vector2 centerPos, float rad) : center(centerPos), radius(rad) {}
    Circle(float x, float y, float rad) : center{x, y}, radius(rad) {}
};

class Entity {
public:
    // Constructor and destructor
    Entity(Vector2 center, float radius);
    Entity(float x, float y, float radius);
    virtual ~Entity() = default;

    // Disable copy operations (entities should be unique)
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    
    // Enable move operations
    Entity(Entity&&) = default;
    Entity& operator=(Entity&&) = default;
    
    // Getters (const correctness)
    [[nodiscard]] const Circle& GetBounds() const noexcept { return m_bounds; }
    [[nodiscard]] Vector2 GetCenter() const noexcept { return m_bounds.center; }
    [[nodiscard]] float GetRadius() const noexcept { return m_bounds.radius; }
    [[nodiscard]] float GetX() const noexcept { return m_bounds.center.x; }
    [[nodiscard]] float GetY() const noexcept { return m_bounds.center.y; }
    [[nodiscard]] float GetVelocityY() const noexcept { return m_velocityY; }
    [[nodiscard]] bool IsOnGround() const noexcept { return m_isOnGround; }
    [[nodiscard]] bool CanPhase() const noexcept { return m_canPhase; }
    
    // Setters with validation
    void SetPosition(Vector2 position) noexcept;
    void SetPosition(float x, float y) noexcept;
    void SetX(float x) noexcept;
    void SetY(float y) noexcept;
    void SetRadius(float radius);
    void SetVelocityY(float velocity) noexcept { m_velocityY = velocity; }
    void SetOnGround(bool onGround) noexcept { m_isOnGround = onGround; }
    void SetCanPhase(bool canPhase) noexcept { m_canPhase = canPhase; }
    
    // Movement methods
    void Move(Vector2 delta) noexcept;
    void Move(float deltaX, float deltaY) noexcept;
    void Jump() noexcept;
    
    // Pure virtual methods that derived classes must implement
    virtual void Update(float deltaTime) = 0;
    virtual void Draw(std::int32_t textureResolution, 
                     std::int32_t windowHeight, 
                     std::int32_t windowWidth) const = 0;

protected:
    // Protected constants
    static constexpr float JUMP_FORCE = -550.0f;
    static constexpr float MIN_RADIUS = 1.0f;
    static constexpr float MAX_RADIUS = 1000.0f;
    
    // Protected members for derived classes
    Circle m_bounds;
    float m_velocityY{0.0f};
    bool m_isOnGround{false};
    bool m_canPhase{false};
    
private:
    // Private validation methods
    void ValidateRadius(float radius) const;
};

} // namespace PlayAsGobo