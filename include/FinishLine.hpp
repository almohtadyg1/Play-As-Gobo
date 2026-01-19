#pragma once

#include "raylib.h"
#include <cmath>
#include <cstdint>

namespace PlayAsGobo {

// Forward declaration to avoid duplication
struct Circle;

class FinishLine {
public:
    // Constructors
    FinishLine(float x, float y, float width, float height);
    FinishLine(float x, float y, float width, float height, const Texture2D& finishLineTexture);
    FinishLine(float x, float y, float width, float height, Color color);
    FinishLine(float x, float y, float width, float height, const Texture2D& finishLineTexture, Color tintColor);
    FinishLine(const Rectangle& bounds);
    FinishLine(const Rectangle& bounds, const Texture2D& finishLineTexture);
    FinishLine(const Rectangle& bounds, Color color);
    FinishLine(const Rectangle& bounds, const Texture2D& finishLineTexture, Color tintColor);
    
    // Disable copy operations for performance (finish lines are typically unique)
    FinishLine(const FinishLine&) = delete;
    FinishLine& operator=(const FinishLine&) = delete;
    
    // Enable move operations
    FinishLine(FinishLine&&) = default;
    FinishLine& operator=(FinishLine&&) = default;
    
    // Destructor
    ~FinishLine() = default;
    
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
    [[nodiscard]] bool IsActive() const noexcept { return m_isActive; }
    
    // Setters with validation
    void SetPosition(float x, float y);
    void SetPosition(Vector2 position);
    void SetSize(float width, float height);
    void SetSize(Vector2 size);
    void SetBounds(const Rectangle& newBounds);
    void SetTexture(const Texture2D& finishLineTexture);
    void SetTintColor(Color color) noexcept { m_tintColor = color; }
    void SetActive(bool active) noexcept { m_isActive = active; }
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
    [[nodiscard]] bool IsEntityCrossing(const Circle& entity) const noexcept;
    
    // State management
    void Activate() noexcept { m_isActive = true; }
    void Deactivate() noexcept { m_isActive = false; }
    void Toggle() noexcept { m_isActive = !m_isActive; }
    
    // Rendering
    void Draw() const;
    void DrawWithAnimation(float animationTime) const;
    void DrawOutline(Color outlineColor = BLACK, float thickness = 2.0f) const;
    void DrawWithOffset(Vector2 offset) const;

private:
    // Constants
    static constexpr float MIN_DIMENSION = 1.0f;
    static constexpr float MAX_DIMENSION = 5000.0f;
    static constexpr Color DEFAULT_COLOR = YELLOW;
    static constexpr Color DEFAULT_TINT = WHITE;
    static constexpr Color INACTIVE_COLOR = GRAY;
    static constexpr float ANIMATION_PULSE_SPEED = 3.0f;
    static constexpr float ANIMATION_OPACITY_MIN = 0.6f;
    static constexpr float ANIMATION_OPACITY_MAX = 1.0f;
    
    // Member variables
    Rectangle m_bounds{0.0f, 0.0f, 0.0f, 0.0f};
    Texture2D m_texture{};
    Color m_tintColor{DEFAULT_COLOR};
    bool m_hasTexture{false};
    bool m_isActive{true};
    
    // Private helper methods
    void ValidateDimensions(float width, float height) const;
    void ValidateTexture(const Texture2D& texture) const;
    void DrawTexturedFinishLine() const;
    void DrawSolidFinishLine() const;
    void DrawAnimatedFinishLine(float animationTime) const;
    void CalculateTileLayout(float textureWidth, float textureHeight,
                           std::int32_t& tilesX, std::int32_t& tilesY) const;
    void DrawTile(float textureWidth, float textureHeight,
                 std::int32_t tileX, std::int32_t tileY) const;
    void DrawAnimatedTile(float textureWidth, float textureHeight,
                         std::int32_t tileX, std::int32_t tileY, float alpha) const;
    Rectangle CalculateClippedDestRect(const Rectangle& destRect) const noexcept;
    Rectangle CalculateClippedSourceRect(const Rectangle& sourceRect, 
                                       const Rectangle& destRect,
                                       const Rectangle& clippedDestRect) const noexcept;
    [[nodiscard]] Color GetCurrentTintColor() const noexcept;
    [[nodiscard]] float CalculateAnimationAlpha(float animationTime) const noexcept;
};

} // namespace PlayAsGobo