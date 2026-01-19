#include "Player.hpp"
#include <stdexcept>
#include <algorithm>
#include <iostream>

namespace PlayAsGobo {

Player::Player(float x, float y, float radius,
               const std::vector<Texture2D>& playerTextures,
               const Music& walkSound, float scale, float speed)
    : Entity(x, y, radius * scale)
    , m_textures(playerTextures)
    , m_walkSound(walkSound)
    , m_moveSpeed(speed)
    , m_originalRadius(radius)
    , m_sizeScale(scale) {
    
    ValidateTextures();
    ValidateScale(scale);
    ValidateSpeed(speed);
}

void Player::ValidateTextures() const {
    if (m_textures.empty()) {
        throw std::invalid_argument("Player textures cannot be empty");
    }
    
    if (m_textures.size() < 3) {
        throw std::invalid_argument("Player requires at least 3 texture frames");
    }
    
    // Validate that textures are actually loaded
    for (std::size_t i = 0; i < m_textures.size(); ++i) {
        if (m_textures[i].id == 0) {
            throw std::invalid_argument("Player texture at index " + std::to_string(i) + 
                                      " is not properly loaded");
        }
    }
}

void Player::ValidateScale(float scale) const {
    constexpr float MIN_SCALE = 0.1f;
    constexpr float MAX_SCALE = 10.0f;
    
    if (scale < MIN_SCALE || scale > MAX_SCALE) {
        throw std::invalid_argument("Player scale must be between " + 
                                  std::to_string(MIN_SCALE) + " and " + 
                                  std::to_string(MAX_SCALE));
    }
}

void Player::ValidateSpeed(float speed) const {
    constexpr float MIN_SPEED = 1.0f;
    constexpr float MAX_SPEED = 2000.0f;
    
    if (speed < MIN_SPEED || speed > MAX_SPEED) {
        throw std::invalid_argument("Player speed must be between " + 
                                  std::to_string(MIN_SPEED) + " and " + 
                                  std::to_string(MAX_SPEED));
    }
}

void Player::TakeDamage() {
    m_sizeScale *= DAMAGE_SCALE_FACTOR;
    
    // Ensure we don't become too small
    constexpr float MIN_SCALE = 0.1f;
    if (m_sizeScale < MIN_SCALE) {
        m_sizeScale = MIN_SCALE;
    }
    
    UpdateRadius();
}

void Player::GrowLarger() {
    m_sizeScale *= GROWTH_SCALE_FACTOR;
    
    // Cap maximum size to prevent excessive growth
    constexpr float MAX_SCALE = 5.0f;
    if (m_sizeScale > MAX_SCALE) {
        m_sizeScale = MAX_SCALE;
    }
    
    UpdateRadius();
}

void Player::ShrinkSize() {
    m_sizeScale /= SHRINK_SCALE_FACTOR;
    
    // Ensure minimum size
    constexpr float MIN_SCALE = 0.1f;
    if (m_sizeScale < MIN_SCALE) {
        m_sizeScale = MIN_SCALE;
    }
    
    UpdateRadius();
}

void Player::ResetToOriginalSize() noexcept {
    m_sizeScale = 1.0f;
    UpdateRadius();
}

void Player::UpdateRadius() noexcept {
    const float newRadius = m_originalRadius * m_sizeScale;
    try {
        SetRadius(newRadius);
    } catch (const std::exception& e) {
        // Log error but don't crash - maintain current radius
        std::cerr << "Warning: Failed to update player radius: " << e.what() << std::endl;
    }
}

void Player::HandleInput(float deltaTime, const Rectangle& groundBounds,
                        ExplosionManager& explosionManager,
                        const Sound& explosionSound, bool soundEnabled) {
    if (deltaTime <= 0.0f) return;
    
    UpdateMusicStream(m_walkSound);
    
    // Handle movement input
    HandleMovementInput(deltaTime, groundBounds, soundEnabled);
    
    // Handle bomb input
    HandleBombInput(explosionManager, explosionSound, soundEnabled);
}

void Player::HandleMovementInput(float deltaTime, const Rectangle& groundBounds, bool soundEnabled) {
    const bool movingRight = (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D));
    const bool movingLeft = (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A));
    
    m_isMoving = false;
    
    if (movingRight && GetX() + GetRadius() < groundBounds.x + groundBounds.width) {
        SetX(GetX() + m_moveSpeed * deltaTime);
        m_isMoving = true;
    } else if (movingLeft && GetX() - GetRadius() > groundBounds.x) {
        SetX(GetX() - m_moveSpeed * deltaTime);
        m_isMoving = true;
    }
    
    // Handle walk sound
    if (m_isMoving && m_isOnGround && soundEnabled) {
        if (!IsMusicStreamPlaying(m_walkSound)) {
            PlayMusicStream(m_walkSound);
        }
    } else if (IsMusicStreamPlaying(m_walkSound)) {
        StopMusicStream(m_walkSound);
    }
}

void Player::HandleBombInput(ExplosionManager& explosionManager, 
                           const Sound& explosionSound, bool soundEnabled) {
    if (IsKeyPressed(KEY_SPACE) && m_canUseBomb) {
        // Create explosion at player's position
        const Vector2 explosionPosition = {GetX(), GetY() - GetRadius()};
        explosionManager.CreateExplosion(explosionPosition, explosionSound, soundEnabled);
        
        // Reduce player size
        const float newRadius = GetRadius() * BOMB_RADIUS_REDUCTION;
        const float minimumRadius = m_originalRadius;
        
        if (newRadius > minimumRadius) {
            SetRadius(newRadius);
            m_sizeScale = newRadius / m_originalRadius;
        } else {
            SetRadius(minimumRadius);
            m_sizeScale = 1.0f;
        }
        
        m_canUseBomb = false;
    }
}

void Player::UpdateAnimation(float deltaTime) {
    m_animationTimer += deltaTime;
    
    if (m_animationTimer >= ANIMATION_INTERVAL) {
        if (m_isMoving) {
            // Alternate between standing and walking frames
            m_currentFrame = (m_currentFrame == AnimationFrame::Standing) ? 
                           AnimationFrame::Walking : AnimationFrame::Standing;
        } else {
            // Alternate between standing and idle frames
            m_currentFrame = (m_currentFrame == AnimationFrame::Standing) ? 
                           AnimationFrame::Idle : AnimationFrame::Standing;
        }
        
        m_animationTimer = 0.0f;
    }
}

void Player::Update(float deltaTime) {
    if (deltaTime <= 0.0f) return;
    
    UpdateAnimation(deltaTime);
}

void Player::Draw(std::int32_t textureResolution, 
                 std::int32_t windowHeight, 
                 std::int32_t windowWidth) const {
    if (m_textures.empty()) {
        std::cerr << "Warning: No textures available for player rendering" << std::endl;
        return;
    }
    
    const std::size_t frameIndex = static_cast<std::size_t>(m_currentFrame);
    if (frameIndex >= m_textures.size()) {
        std::cerr << "Warning: Invalid frame index for player animation" << std::endl;
        return;
    }
    
    // Source rectangle (texture coordinates)
    const Rectangle sourceRect = {
        0.0f, 0.0f, 
        static_cast<float>(textureResolution), 
        static_cast<float>(textureResolution)
    };
    
    // Destination rectangle (screen coordinates)
    const float diameter = GetRadius() * 2.0f;
    const Rectangle destRect = {
        GetX() - GetRadius(), 
        GetY() - GetRadius(),
        diameter, 
        diameter
    };
    
    // Draw the player texture
    DrawTexturePro(m_textures[frameIndex], sourceRect, destRect, 
                   Vector2{0.0f, 0.0f}, 0.0f, WHITE);
    
    // Draw bomb indicator if bomb is available
    if (m_canUseBomb) {
        DrawBombIndicator(windowHeight);
    }
}

void Player::DrawBombIndicator(std::int32_t windowHeight) const {
    constexpr const char* BOMB_TEXT = "Space to Bomb!";
    constexpr std::int32_t FONT_SIZE = 30;
    constexpr std::int32_t TEXT_OFFSET_Y = 100;
    
    // Calculate text position
    const std::int32_t textWidth = MeasureText(BOMB_TEXT, FONT_SIZE);
    const std::int32_t textX = static_cast<std::int32_t>(GetX()) - textWidth / 2;
    const std::int32_t textY = windowHeight / 2 - TEXT_OFFSET_Y;
    
    // Draw pulsing text
    DrawText(BOMB_TEXT, textX, textY, FONT_SIZE, MAROON);
    
    // Calculate pulsing alpha for glow effect
    const float time = GetTime();
    const float alpha = (std::sin(time * BOMB_TEXT_PULSE_SPEED) + 1.0f) / 2.0f * BOMB_GLOW_OPACITY;
    
    // Draw animated glow effect
    const Vector2 glowPosition = {
        GetX(), 
        GetY() - GetRadius() * 0.75f
    };
    
    const float glowRadius = GetRadius() * 0.5f;
    DrawCircleGradient(
        static_cast<std::int32_t>(glowPosition.x),
        static_cast<std::int32_t>(glowPosition.y),
        static_cast<std::int32_t>(glowRadius),
        Fade(RED, alpha),
        BLANK
    );
}

} // namespace PlayAsGobo