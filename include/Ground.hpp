#pragma once

#include "raylib.h"
#include <cmath>
#include <cstdint>

namespace PlayAsGobo {

// Forward declaration to avoid duplication
struct Circle;

class Ground {
public:
    // Constructors
    Ground(float x, float y, float width, float height);
    Ground(float x, float y, float width, float height, const Texture2D& groundTexture);
    Ground(float x, float y, float width, float height, Color color);
    Ground(float x, float y, float width, float height, const Texture2D& groundTexture, Color tintColor);
    Ground(const Rectangle& bounds);
    Ground(const Rectangle& bounds, const Texture2D& groundTexture);
    Ground(const Rectangle& bounds, Color color);
    Ground(const Rectangle& bounds, const Texture2D& groundTexture, Color tintColor);
    
    // Disable copy operations for performance (grounds are typically unique)
    Ground(const Ground&) = delete;
    Ground& operator=(const Ground&) = delete;
    
    // Enable move operations
    Ground(Ground&&) = default;
    Ground& operator=(Ground&&) = default;
    
    // Destructor
    ~Ground() = default;
    
    // Getters (const correctness)
    [[nodiscard]] const Rectangle& GetBounds() const noexcept { return m_bounds; }
    [[nodiscard]] float GetX() const noexcept { return m_bounds.x; }
    [[nodiscard]] float GetY() const noexcept { return m_bounds.y; }
    [[nodiscard]] float GetWidth() const noexcept { return m_bounds.width; }
    [[nodiscard]] float GetHeight() const noexcept { return m_bounds.height; }
    [[nodiscard]] Vector2 GetPosition() const noexcept { return {m_bounds.x, m_bounds.y}; }
    [[nodiscard]] Vector2 GetSize() const noexcept { return {m_bounds.width, m_bounds.height}; }
    [[nodiscard]] Vector2 GetCenter() const noexcept;
    [[nodiscard]] bool HasTexture() const noexcept { return m_hasTexture; }
    [[nodiscard]] Color GetTintColor() const noexcept { return m_tintColor; }
    [[nodiscard]] float GetArea() const noexcept { return m_bounds.width * m_bounds.height; }
    
    // Setters with validation
    void SetPosition(float x, float y);
    void SetPosition(Vector2 position);
    void SetSize(float width, float height);
    void SetSize(Vector2 size);
    void SetBounds(const Rectangle& newBounds);
    void SetTexture(const Texture2D& groundTexture);
    void SetTintColor(Color color) noexcept { m_tintColor = color; }
    void RemoveTexture() noexcept;
    
    // Movement and transformation
    void Move(float deltaX, float deltaY) noexcept;
    void Move(Vector2 delta) noexcept;
    void Scale(float factor);
    void Scale(float factorX, float factorY);
    
    // Collision detection
    [[nodiscard]] bool CheckCollision(const Circle& other) const noexcept;
    [[nodiscard]] bool CheckCollision(const Rectangle& other) const noexcept;
    [[nodiscard]] bool CheckCollision(Vector2 point) const noexcept;
    [[nodiscard]] bool IsInside(const Circle& other) const noexcept;
    [[nodiscard]] bool IsInside(const Rectangle& other) const noexcept;
    
    // Rendering
    void Draw() const;
    void DrawOutline(Color outlineColor = BLACK, float thickness = 1.0f) const;
    void DrawWithOffset(Vector2 offset) const;

private:
    // Constants
    static constexpr float MIN_DIMENSION = 1.0f;
    static constexpr float MAX_DIMENSION = 10000.0f;
    static constexpr Color DEFAULT_COLOR = GREEN;
    static constexpr Color DEFAULT_TINT = WHITE;
    
    // Member variables
    Rectangle m_bounds{0.0f, 0.0f, 0.0f, 0.0f};
    Texture2D m_texture{};
    Color m_tintColor{DEFAULT_COLOR};
    bool m_hasTexture{false};
    
    // Private helper methods
    void ValidateDimensions(float width, float height) const;
    void ValidateTexture(const Texture2D& texture) const;
    void DrawTexturedGround() const;
    void DrawSolidGround() const;
    void CalculateTileLayout(float textureWidth, float textureHeight,
                           std::int32_t& tilesX, std::int32_t& tilesY) const;
    void DrawTile(float textureWidth, float textureHeight,
                 std::int32_t tileX, std::int32_t tileY) const;
    Rectangle CalculateClippedDestRect(const Rectangle& destRect) const noexcept;
    Rectangle CalculateClippedSourceRect(const Rectangle& sourceRect, 
                                       const Rectangle& destRect,
                                       const Rectangle& clippedDestRect) const noexcept;
};

} // namespace PlayAsGobo