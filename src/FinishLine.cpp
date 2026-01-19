#include "FinishLine.hpp"
#include "Entity.hpp" // For Circle definition
#include <stdexcept>
#include <algorithm>
#include <iostream>

namespace PlayAsGobo {

// Constructor implementations
FinishLine::FinishLine(float x, float y, float width, float height)
    : m_bounds{x, y, width, height}
    , m_tintColor(DEFAULT_COLOR)
    , m_hasTexture(false) {
    ValidateDimensions(width, height);
}

FinishLine::FinishLine(float x, float y, float width, float height, const Texture2D& finishLineTexture)
    : m_bounds{x, y, width, height}
    , m_texture(finishLineTexture)
    , m_tintColor(DEFAULT_TINT)
    , m_hasTexture(true) {
    ValidateDimensions(width, height);
    ValidateTexture(finishLineTexture);
}

FinishLine::FinishLine(float x, float y, float width, float height, Color color)
    : m_bounds{x, y, width, height}
    , m_tintColor(color)
    , m_hasTexture(false) {
    ValidateDimensions(width, height);
}

FinishLine::FinishLine(float x, float y, float width, float height, 
                       const Texture2D& finishLineTexture, Color tintColor)
    : m_bounds{x, y, width, height}
    , m_texture(finishLineTexture)
    , m_tintColor(tintColor)
    , m_hasTexture(true) {
    ValidateDimensions(width, height);
    ValidateTexture(finishLineTexture);
}

FinishLine::FinishLine(const Rectangle& bounds)
    : m_bounds(bounds)
    , m_tintColor(DEFAULT_COLOR)
    , m_hasTexture(false) {
    ValidateDimensions(bounds.width, bounds.height);
}

FinishLine::FinishLine(const Rectangle& bounds, const Texture2D& finishLineTexture)
    : m_bounds(bounds)
    , m_texture(finishLineTexture)
    , m_tintColor(DEFAULT_TINT)
    , m_hasTexture(true) {
    ValidateDimensions(bounds.width, bounds.height);
    ValidateTexture(finishLineTexture);
}

FinishLine::FinishLine(const Rectangle& bounds, Color color)
    : m_bounds(bounds)
    , m_tintColor(color)
    , m_hasTexture(false) {
    ValidateDimensions(bounds.width, bounds.height);
}

FinishLine::FinishLine(const Rectangle& bounds, const Texture2D& finishLineTexture, Color tintColor)
    : m_bounds(bounds)
    , m_texture(finishLineTexture)
    , m_tintColor(tintColor)
    , m_hasTexture(true) {
    ValidateDimensions(bounds.width, bounds.height);
    ValidateTexture(finishLineTexture);
}

// Validation methods
void FinishLine::ValidateDimensions(float width, float height) const {
    if (width < MIN_DIMENSION || width > MAX_DIMENSION) {
        throw std::invalid_argument("FinishLine width must be between " + 
                                  std::to_string(MIN_DIMENSION) + " and " + 
                                  std::to_string(MAX_DIMENSION));
    }
    if (height < MIN_DIMENSION || height > MAX_DIMENSION) {
        throw std::invalid_argument("FinishLine height must be between " + 
                                  std::to_string(MIN_DIMENSION) + " and " + 
                                  std::to_string(MAX_DIMENSION));
    }
}

void FinishLine::ValidateTexture(const Texture2D& texture) const {
    if (texture.id == 0) {
        throw std::invalid_argument("FinishLine texture is not properly loaded (id = 0)");
    }
    if (texture.width <= 0 || texture.height <= 0) {
        throw std::invalid_argument("FinishLine texture has invalid dimensions");
    }
}

// Getters
Vector2 FinishLine::GetCenter() const noexcept {
    return {
        m_bounds.x + m_bounds.width / 2.0f,
        m_bounds.y + m_bounds.height / 2.0f
    };
}

// Setters
void FinishLine::SetPosition(float x, float y) {
    m_bounds.x = x;
    m_bounds.y = y;
}

void FinishLine::SetPosition(Vector2 position) {
    m_bounds.x = position.x;
    m_bounds.y = position.y;
}

void FinishLine::SetSize(float width, float height) {
    ValidateDimensions(width, height);
    m_bounds.width = width;
    m_bounds.height = height;
}

void FinishLine::SetSize(Vector2 size) {
    ValidateDimensions(size.x, size.y);
    m_bounds.width = size.x;
    m_bounds.height = size.y;
}

void FinishLine::SetBounds(const Rectangle& newBounds) {
    ValidateDimensions(newBounds.width, newBounds.height);
    m_bounds = newBounds;
}

void FinishLine::SetTexture(const Texture2D& finishLineTexture) {
    ValidateTexture(finishLineTexture);
    m_texture = finishLineTexture;
    m_hasTexture = true;
}

void FinishLine::RemoveTexture() noexcept {
    m_hasTexture = false;
    m_texture = Texture2D{}; // Reset to default state
}

// Movement and transformation
void FinishLine::Move(float deltaX, float deltaY) noexcept {
    m_bounds.x += deltaX;
    m_bounds.y += deltaY;
}

void FinishLine::Move(Vector2 delta) noexcept {
    m_bounds.x += delta.x;
    m_bounds.y += delta.y;
}

void FinishLine::Scale(float factor) {
    if (factor <= 0.0f) {
        throw std::invalid_argument("Scale factor must be positive");
    }
    
    const float newWidth = m_bounds.width * factor;
    const float newHeight = m_bounds.height * factor;
    
    ValidateDimensions(newWidth, newHeight);
    
    // Scale from center
    const Vector2 center = GetCenter();
    m_bounds.width = newWidth;
    m_bounds.height = newHeight;
    m_bounds.x = center.x - newWidth / 2.0f;
    m_bounds.y = center.y - newHeight / 2.0f;
}

void FinishLine::Scale(float factorX, float factorY) {
    if (factorX <= 0.0f || factorY <= 0.0f) {
        throw std::invalid_argument("Scale factors must be positive");
    }
    
    const float newWidth = m_bounds.width * factorX;
    const float newHeight = m_bounds.height * factorY;
    
    ValidateDimensions(newWidth, newHeight);
    
    // Scale from center
    const Vector2 center = GetCenter();
    m_bounds.width = newWidth;
    m_bounds.height = newHeight;
    m_bounds.x = center.x - newWidth / 2.0f;
    m_bounds.y = center.y - newHeight / 2.0f;
}

// Collision detection
bool FinishLine::CheckCollision(const Circle& other) const noexcept {
    if (!m_isActive) return false;
    return CheckCollisionCircleRec(other.center, other.radius, m_bounds);
}

bool FinishLine::CheckCollision(const Rectangle& other) const noexcept {
    if (!m_isActive) return false;
    return CheckCollisionRecs(m_bounds, other);
}

bool FinishLine::CheckCollision(Vector2 point) const noexcept {
    if (!m_isActive) return false;
    return CheckCollisionPointRec(point, m_bounds);
}

bool FinishLine::IsEntityCrossing(const Circle& entity) const noexcept {
    if (!m_isActive) return false;
    
    // Check if entity center has crossed the finish line (assuming vertical finish line)
    const float finishLineCenter = m_bounds.x + m_bounds.width / 2.0f;
    const float entityLeft = entity.center.x - entity.radius;
    const float entityRight = entity.center.x + entity.radius;
    
    // Entity is crossing if it overlaps the center line
    return entityLeft <= finishLineCenter && entityRight >= finishLineCenter &&
           entity.center.y >= m_bounds.y && 
           entity.center.y <= m_bounds.y + m_bounds.height;
}

// Helper methods
Color FinishLine::GetCurrentTintColor() const noexcept {
    return m_isActive ? m_tintColor : INACTIVE_COLOR;
}

float FinishLine::CalculateAnimationAlpha(float animationTime) const noexcept {
    const float normalizedTime = std::sin(animationTime * ANIMATION_PULSE_SPEED);
    const float alpha = ANIMATION_OPACITY_MIN + 
                       (ANIMATION_OPACITY_MAX - ANIMATION_OPACITY_MIN) * 
                       ((normalizedTime + 1.0f) / 2.0f);
    return std::clamp(alpha, ANIMATION_OPACITY_MIN, ANIMATION_OPACITY_MAX);
}

// Rendering helper methods
void FinishLine::CalculateTileLayout(float textureWidth, float textureHeight,
                                    std::int32_t& tilesX, std::int32_t& tilesY) const {
    tilesX = static_cast<std::int32_t>(std::ceil(m_bounds.width / textureWidth));
    tilesY = static_cast<std::int32_t>(std::ceil(m_bounds.height / textureHeight));
    
    // Sanity check to prevent excessive tiling
    constexpr std::int32_t MAX_TILES = 500;
    if (tilesX > MAX_TILES || tilesY > MAX_TILES) {
        std::cerr << "Warning: FinishLine texture tiling exceeds maximum (" << MAX_TILES 
                  << ") - performance may be affected" << std::endl;
    }
}

Rectangle FinishLine::CalculateClippedDestRect(const Rectangle& destRect) const noexcept {
    Rectangle clipped = destRect;
    
    // Clip to finish line bounds
    const float finishRight = m_bounds.x + m_bounds.width;
    const float finishBottom = m_bounds.y + m_bounds.height;
    
    if (clipped.x + clipped.width > finishRight) {
        clipped.width = finishRight - clipped.x;
    }
    if (clipped.y + clipped.height > finishBottom) {
        clipped.height = finishBottom - clipped.y;
    }
    
    return clipped;
}

Rectangle FinishLine::CalculateClippedSourceRect(const Rectangle& sourceRect, 
                                                const Rectangle& destRect,
                                                const Rectangle& clippedDestRect) const noexcept {
    Rectangle clippedSource = sourceRect;
    
    // Adjust source rectangle proportionally to destination clipping
    if (destRect.width > 0.0f && destRect.height > 0.0f) {
        const float widthRatio = clippedDestRect.width / destRect.width;
        const float heightRatio = clippedDestRect.height / destRect.height;
        
        clippedSource.width = sourceRect.width * widthRatio;
        clippedSource.height = sourceRect.height * heightRatio;
    }
    
    return clippedSource;
}

void FinishLine::DrawTile(float textureWidth, float textureHeight,
                         std::int32_t tileX, std::int32_t tileY) const {
    const Rectangle sourceRect = {0.0f, 0.0f, textureWidth, textureHeight};
    const Rectangle destRect = {
        m_bounds.x + tileX * textureWidth,
        m_bounds.y + tileY * textureHeight,
        textureWidth,
        textureHeight
    };
    
    // Clip destination rectangle to finish line bounds
    const Rectangle clippedDestRect = CalculateClippedDestRect(destRect);
    
    // Skip if tile is completely outside bounds
    if (clippedDestRect.width <= 0.0f || clippedDestRect.height <= 0.0f) {
        return;
    }
    
    // Calculate corresponding source rectangle for clipped destination
    const Rectangle clippedSourceRect = CalculateClippedSourceRect(sourceRect, destRect, clippedDestRect);
    
    DrawTexturePro(m_texture, clippedSourceRect, clippedDestRect, 
                   Vector2{0.0f, 0.0f}, 0.0f, GetCurrentTintColor());
}

void FinishLine::DrawAnimatedTile(float textureWidth, float textureHeight,
                                 std::int32_t tileX, std::int32_t tileY, float alpha) const {
    const Rectangle sourceRect = {0.0f, 0.0f, textureWidth, textureHeight};
    const Rectangle destRect = {
        m_bounds.x + tileX * textureWidth,
        m_bounds.y + tileY * textureHeight,
        textureWidth,
        textureHeight
    };
    
    // Clip destination rectangle to finish line bounds
    const Rectangle clippedDestRect = CalculateClippedDestRect(destRect);
    
    // Skip if tile is completely outside bounds
    if (clippedDestRect.width <= 0.0f || clippedDestRect.height <= 0.0f) {
        return;
    }
    
    // Calculate corresponding source rectangle for clipped destination
    const Rectangle clippedSourceRect = CalculateClippedSourceRect(sourceRect, destRect, clippedDestRect);
    
    // Apply animation alpha to tint color
    Color animatedColor = GetCurrentTintColor();
    animatedColor.a = static_cast<unsigned char>(animatedColor.a * alpha);
    
    DrawTexturePro(m_texture, clippedSourceRect, clippedDestRect, 
                   Vector2{0.0f, 0.0f}, 0.0f, animatedColor);
}

void FinishLine::DrawTexturedFinishLine() const {
    if (!m_hasTexture || m_texture.id == 0) {
        std::cerr << "Warning: Attempting to draw textured finish line without valid texture" << std::endl;
        DrawSolidFinishLine();
        return;
    }
    
    const float textureWidth = static_cast<float>(m_texture.width);
    const float textureHeight = static_cast<float>(m_texture.height);
    
    if (textureWidth <= 0.0f || textureHeight <= 0.0f) {
        std::cerr << "Warning: FinishLine texture has invalid dimensions" << std::endl;
        DrawSolidFinishLine();
        return;
    }
    
    std::int32_t tilesX, tilesY;
    CalculateTileLayout(textureWidth, textureHeight, tilesX, tilesY);
    
    for (std::int32_t x = 0; x < tilesX; ++x) {
        for (std::int32_t y = 0; y < tilesY; ++y) {
            DrawTile(textureWidth, textureHeight, x, y);
        }
    }
}

void FinishLine::DrawSolidFinishLine() const {
    DrawRectangleRec(m_bounds, GetCurrentTintColor());
}

void FinishLine::DrawAnimatedFinishLine(float animationTime) const {
    const float alpha = CalculateAnimationAlpha(animationTime);
    
    if (m_hasTexture && m_texture.id != 0) {
        const float textureWidth = static_cast<float>(m_texture.width);
        const float textureHeight = static_cast<float>(m_texture.height);
        
        if (textureWidth > 0.0f && textureHeight > 0.0f) {
            std::int32_t tilesX, tilesY;
            CalculateTileLayout(textureWidth, textureHeight, tilesX, tilesY);
            
            for (std::int32_t x = 0; x < tilesX; ++x) {
                for (std::int32_t y = 0; y < tilesY; ++y) {
                    DrawAnimatedTile(textureWidth, textureHeight, x, y, alpha);
                }
            }
            return;
        }
    }
    
    // Fallback to solid color with animation
    Color animatedColor = GetCurrentTintColor();
    animatedColor.a = static_cast<unsigned char>(animatedColor.a * alpha);
    DrawRectangleRec(m_bounds, animatedColor);
}

// Main rendering methods
void FinishLine::Draw() const {
    if (m_hasTexture) {
        DrawTexturedFinishLine();
    } else {
        DrawSolidFinishLine();
    }
}

void FinishLine::DrawWithAnimation(float animationTime) const {
    DrawAnimatedFinishLine(animationTime);
}

void FinishLine::DrawOutline(Color outlineColor, float thickness) const {
    DrawRectangleLinesEx(m_bounds, thickness, outlineColor);
}

void FinishLine::DrawWithOffset(Vector2 offset) const {
    const Rectangle offsetBounds = {
        m_bounds.x + offset.x,
        m_bounds.y + offset.y,
        m_bounds.width,
        m_bounds.height
    };
    
    if (m_hasTexture) {
        // Temporarily adjust bounds for offset drawing
        const Rectangle originalBounds = m_bounds;
        const_cast<FinishLine*>(this)->m_bounds = offsetBounds;
        DrawTexturedFinishLine();
        const_cast<FinishLine*>(this)->m_bounds = originalBounds;
    } else {
        DrawRectangleRec(offsetBounds, GetCurrentTintColor());
    }
}

} // namespace PlayAsGobo