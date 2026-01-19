#include "Ground.hpp"
#include "Entity.hpp" // For Circle definition
#include <stdexcept>
#include <algorithm>
#include <iostream>

namespace PlayAsGobo {

// Constructor implementations
Ground::Ground(float x, float y, float width, float height)
    : m_bounds{x, y, width, height}
    , m_tintColor(DEFAULT_COLOR)
    , m_hasTexture(false) {
    ValidateDimensions(width, height);
}

Ground::Ground(float x, float y, float width, float height, const Texture2D& groundTexture)
    : m_bounds{x, y, width, height}
    , m_texture(groundTexture)
    , m_tintColor(DEFAULT_TINT)
    , m_hasTexture(true) {
    ValidateDimensions(width, height);
    ValidateTexture(groundTexture);
}

Ground::Ground(float x, float y, float width, float height, Color color)
    : m_bounds{x, y, width, height}
    , m_tintColor(color)
    , m_hasTexture(false) {
    ValidateDimensions(width, height);
}

Ground::Ground(float x, float y, float width, float height, 
               const Texture2D& groundTexture, Color tintColor)
    : m_bounds{x, y, width, height}
    , m_texture(groundTexture)
    , m_tintColor(tintColor)
    , m_hasTexture(true) {
    ValidateDimensions(width, height);
    ValidateTexture(groundTexture);
}

Ground::Ground(const Rectangle& bounds)
    : m_bounds(bounds)
    , m_tintColor(DEFAULT_COLOR)
    , m_hasTexture(false) {
    ValidateDimensions(bounds.width, bounds.height);
}

Ground::Ground(const Rectangle& bounds, const Texture2D& groundTexture)
    : m_bounds(bounds)
    , m_texture(groundTexture)
    , m_tintColor(DEFAULT_TINT)
    , m_hasTexture(true) {
    ValidateDimensions(bounds.width, bounds.height);
    ValidateTexture(groundTexture);
}

Ground::Ground(const Rectangle& bounds, Color color)
    : m_bounds(bounds)
    , m_tintColor(color)
    , m_hasTexture(false) {
    ValidateDimensions(bounds.width, bounds.height);
}

Ground::Ground(const Rectangle& bounds, const Texture2D& groundTexture, Color tintColor)
    : m_bounds(bounds)
    , m_texture(groundTexture)
    , m_tintColor(tintColor)
    , m_hasTexture(true) {
    ValidateDimensions(bounds.width, bounds.height);
    ValidateTexture(groundTexture);
}

// Validation methods
void Ground::ValidateDimensions(float width, float height) const {
    if (width < MIN_DIMENSION || width > MAX_DIMENSION) {
        throw std::invalid_argument("Ground width must be between " + 
                                  std::to_string(MIN_DIMENSION) + " and " + 
                                  std::to_string(MAX_DIMENSION));
    }
    if (height < MIN_DIMENSION || height > MAX_DIMENSION) {
        throw std::invalid_argument("Ground height must be between " + 
                                  std::to_string(MIN_DIMENSION) + " and " + 
                                  std::to_string(MAX_DIMENSION));
    }
}

void Ground::ValidateTexture(const Texture2D& texture) const {
    if (texture.id == 0) {
        throw std::invalid_argument("Ground texture is not properly loaded (id = 0)");
    }
    if (texture.width <= 0 || texture.height <= 0) {
        throw std::invalid_argument("Ground texture has invalid dimensions");
    }
}

// Getters
Vector2 Ground::GetCenter() const noexcept {
    return {
        m_bounds.x + m_bounds.width / 2.0f,
        m_bounds.y + m_bounds.height / 2.0f
    };
}

// Setters
void Ground::SetPosition(float x, float y) {
    m_bounds.x = x;
    m_bounds.y = y;
}

void Ground::SetPosition(Vector2 position) {
    m_bounds.x = position.x;
    m_bounds.y = position.y;
}

void Ground::SetSize(float width, float height) {
    ValidateDimensions(width, height);
    m_bounds.width = width;
    m_bounds.height = height;
}

void Ground::SetSize(Vector2 size) {
    ValidateDimensions(size.x, size.y);
    m_bounds.width = size.x;
    m_bounds.height = size.y;
}

void Ground::SetBounds(const Rectangle& newBounds) {
    ValidateDimensions(newBounds.width, newBounds.height);
    m_bounds = newBounds;
}

void Ground::SetTexture(const Texture2D& groundTexture) {
    ValidateTexture(groundTexture);
    m_texture = groundTexture;
    m_hasTexture = true;
}

void Ground::RemoveTexture() noexcept {
    m_hasTexture = false;
    m_texture = Texture2D{}; // Reset to default state
}

// Movement and transformation
void Ground::Move(float deltaX, float deltaY) noexcept {
    m_bounds.x += deltaX;
    m_bounds.y += deltaY;
}

void Ground::Move(Vector2 delta) noexcept {
    m_bounds.x += delta.x;
    m_bounds.y += delta.y;
}

void Ground::Scale(float factor) {
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

void Ground::Scale(float factorX, float factorY) {
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
bool Ground::CheckCollision(const Circle& other) const noexcept {
    return CheckCollisionCircleRec(other.center, other.radius, m_bounds);
}

bool Ground::CheckCollision(const Rectangle& other) const noexcept {
    return CheckCollisionRecs(m_bounds, other);
}

bool Ground::CheckCollision(Vector2 point) const noexcept {
    return CheckCollisionPointRec(point, m_bounds);
}

bool Ground::IsInside(const Circle& other) const noexcept {
    const float circleLeft = other.center.x - other.radius;
    const float circleRight = other.center.x + other.radius;
    const float circleTop = other.center.y - other.radius;
    const float circleBottom = other.center.y + other.radius;
    
    return circleLeft >= m_bounds.x &&
           circleRight <= m_bounds.x + m_bounds.width &&
           circleTop >= m_bounds.y &&
           circleBottom <= m_bounds.y + m_bounds.height;
}

bool Ground::IsInside(const Rectangle& other) const noexcept {
    return other.x >= m_bounds.x &&
           other.x + other.width <= m_bounds.x + m_bounds.width &&
           other.y >= m_bounds.y &&
           other.y + other.height <= m_bounds.y + m_bounds.height;
}

// Rendering helper methods
void Ground::CalculateTileLayout(float textureWidth, float textureHeight,
                                std::int32_t& tilesX, std::int32_t& tilesY) const {
    tilesX = static_cast<std::int32_t>(std::ceil(m_bounds.width / textureWidth));
    tilesY = static_cast<std::int32_t>(std::ceil(m_bounds.height / textureHeight));
    
    // Sanity check to prevent excessive tiling
    constexpr std::int32_t MAX_TILES = 1000;
    if (tilesX > MAX_TILES || tilesY > MAX_TILES) {
        std::cerr << "Warning: Ground texture tiling exceeds maximum (" << MAX_TILES << ") - performance may be affected" << std::endl;
    }
}

Rectangle Ground::CalculateClippedDestRect(const Rectangle& destRect) const noexcept {
    Rectangle clipped = destRect;
    
    // Clip to ground bounds
    const float groundRight = m_bounds.x + m_bounds.width;
    const float groundBottom = m_bounds.y + m_bounds.height;
    
    if (clipped.x + clipped.width > groundRight) {
        clipped.width = groundRight - clipped.x;
    }
    if (clipped.y + clipped.height > groundBottom) {
        clipped.height = groundBottom - clipped.y;
    }
    
    return clipped;
}

Rectangle Ground::CalculateClippedSourceRect(const Rectangle& sourceRect, 
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

void Ground::DrawTile(float textureWidth, float textureHeight,
                     std::int32_t tileX, std::int32_t tileY) const {
    const Rectangle sourceRect = {0.0f, 0.0f, textureWidth, textureHeight};
    const Rectangle destRect = {
        m_bounds.x + tileX * textureWidth,
        m_bounds.y + tileY * textureHeight,
        textureWidth,
        textureHeight
    };
    
    // Clip destination rectangle to ground bounds
    const Rectangle clippedDestRect = CalculateClippedDestRect(destRect);
    
    // Skip if tile is completely outside bounds
    if (clippedDestRect.width <= 0.0f || clippedDestRect.height <= 0.0f) {
        return;
    }
    
    // Calculate corresponding source rectangle for clipped destination
    const Rectangle clippedSourceRect = CalculateClippedSourceRect(sourceRect, destRect, clippedDestRect);
    
    DrawTexturePro(m_texture, clippedSourceRect, clippedDestRect, Vector2{0.0f, 0.0f}, 0.0f, m_tintColor);
}

void Ground::DrawTexturedGround() const {
    if (!m_hasTexture || m_texture.id == 0) {
        std::cerr << "Warning: Attempting to draw textured ground without valid texture" << std::endl;
        DrawSolidGround();
        return;
    }
    
    const float textureWidth = static_cast<float>(m_texture.width);
    const float textureHeight = static_cast<float>(m_texture.height);
    
    if (textureWidth <= 0.0f || textureHeight <= 0.0f) {
        std::cerr << "Warning: Ground texture has invalid dimensions" << std::endl;
        DrawSolidGround();
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

void Ground::DrawSolidGround() const {
    DrawRectangleRec(m_bounds, m_tintColor);
}

// Main rendering methods
void Ground::Draw() const {
    if (m_hasTexture) {
        DrawTexturedGround();
    } else {
        DrawSolidGround();
    }
}

void Ground::DrawOutline(Color outlineColor, float thickness) const {
    DrawRectangleLinesEx(m_bounds, thickness, outlineColor);
}

void Ground::DrawWithOffset(Vector2 offset) const {
    const Rectangle offsetBounds = {
        m_bounds.x + offset.x,
        m_bounds.y + offset.y,
        m_bounds.width,
        m_bounds.height
    };
    
    if (m_hasTexture) {
        // Temporarily adjust bounds for offset drawing
        const Rectangle originalBounds = m_bounds;
        const_cast<Ground*>(this)->m_bounds = offsetBounds;
        DrawTexturedGround();
        const_cast<Ground*>(this)->m_bounds = originalBounds;
    } else {
        DrawRectangleRec(offsetBounds, m_tintColor);
    }
}

} // namespace PlayAsGobo