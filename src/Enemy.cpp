#include "Enemy.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace PlayAsGobo {

Enemy::Enemy(float x, float y, float radius,
             const std::vector<Texture2D>& enemyTextures,
             float speed, EnemyDirection initialDirection)
    : Entity(x, y, radius)
    , m_textures(enemyTextures)
    , m_moveSpeed(speed)
    , m_direction(initialDirection) {
    
    ValidateTextures();
    ValidateSpeed(speed);
}

void Enemy::ValidateTextures() const {
    if (m_textures.empty()) {
        throw std::invalid_argument("Enemy textures cannot be empty");
    }
    
    if (m_textures.size() < 4) {
        throw std::invalid_argument("Enemy requires at least 4 texture frames (idle + 3 running)");
    }
    
    // Validate that textures are actually loaded
    for (std::size_t i = 0; i < m_textures.size(); ++i) {
        if (m_textures[i].id == 0) {
            throw std::invalid_argument("Enemy texture at index " + std::to_string(i) + 
                                      " is not properly loaded");
        }
    }
}

void Enemy::ValidateSpeed(float speed) const {
    if (speed < MIN_MOVE_SPEED || speed > MAX_MOVE_SPEED) {
        throw std::invalid_argument("Enemy speed must be between " + 
                                  std::to_string(MIN_MOVE_SPEED) + " and " + 
                                  std::to_string(MAX_MOVE_SPEED));
    }
}

void Enemy::SetMoveSpeed(float speed) {
    ValidateSpeed(speed);
    m_moveSpeed = speed;
}

void Enemy::FlipDirection() noexcept {
    m_direction = (m_direction == EnemyDirection::Right) ? 
                  EnemyDirection::Left : EnemyDirection::Right;
}

bool Enemy::ShouldMoveRight(float finishLineX, float mapWidth) const noexcept {
    return GetX() < finishLineX && 
           GetX() + GetRadius() < mapWidth;
}

bool Enemy::ShouldMoveLeft(float finishLineX) const noexcept {
    return GetX() > finishLineX && 
           GetX() - GetRadius() > 0.0f;
}

bool Enemy::IsPlayerInJumpRange(const Player& player) const noexcept {
    const float playerX = player.GetX();
    const float enemyX = GetX();
    const float distance = std::abs(playerX - enemyX);
    
    if (distance > PLAYER_DETECTION_RANGE) {
        return false;
    }
    
    // Check if player is in the direction we're facing
    if (m_direction == EnemyDirection::Right) {
        return playerX >= enemyX && playerX <= enemyX + PLAYER_DETECTION_RANGE;
    } else {
        return playerX <= enemyX && playerX >= enemyX - PLAYER_DETECTION_RANGE;
    }
}

float Enemy::CalculateVolumeByDistance(const Player& player) const noexcept {
    const float distance = std::abs(GetX() - player.GetX());
    const float adjustedDistance = std::max(distance, MIN_VOLUME_DISTANCE);
    const float volume = 1.0f - VOLUME_DISTANCE_FACTOR / adjustedDistance;
    
    return std::clamp(volume, 0.0f, 1.0f);
}

void Enemy::UpdateMovement(float deltaTime, float mapWidth, float finishLineX) {
    m_isMoving = false;
    
    if (ShouldMoveRight(finishLineX, mapWidth)) {
        m_direction = EnemyDirection::Right;
        SetX(GetX() + m_moveSpeed * deltaTime);
        m_isMoving = true;
    } else if (ShouldMoveLeft(finishLineX)) {
        m_direction = EnemyDirection::Left;
        SetX(GetX() - m_moveSpeed * deltaTime);
        m_isMoving = true;
    }
}

void Enemy::HandlePlayerProximityJump(const Player& player) {
    if (m_isOnGround && IsPlayerInJumpRange(player)) {
        Jump();
    }
}

void Enemy::ExecuteAI(float deltaTime, float mapWidth, float finishLineX,
                     const Player& player, [[maybe_unused]] bool soundEnabled) {
    if (deltaTime <= 0.0f) {
        std::cerr << "Warning: Invalid deltaTime passed to Enemy::ExecuteAI" << std::endl;
        return;
    }
    
    // Update movement based on finish line position
    UpdateMovement(deltaTime, mapWidth, finishLineX);
    
    // Handle jumping when player is nearby
    HandlePlayerProximityJump(player);
}

void Enemy::UpdateAnimation(float deltaTime) {
    if (!m_isOnGround) {
        // In air (jumping) - use running frame 1
        m_currentFrame = AnimationFrame::Running1;
        m_animationTimer = 0.0f;
    } else if (m_isMoving) {
        // Running - cycle through running frames
        m_animationTimer += deltaTime;
        
        if (m_animationTimer >= ANIMATION_INTERVAL) {
            // Cycle through running frames 1, 2, 3
            switch (m_currentFrame) {
                case AnimationFrame::Running1:
                    m_currentFrame = AnimationFrame::Running2;
                    break;
                case AnimationFrame::Running2:
                    m_currentFrame = AnimationFrame::Running3;
                    break;
                case AnimationFrame::Running3:
                case AnimationFrame::Idle:
                default:
                    m_currentFrame = AnimationFrame::Running1;
                    break;
            }
            
            m_animationTimer = 0.0f;
        }
    } else {
        // Stopped - use idle frame
        m_currentFrame = AnimationFrame::Idle;
        m_animationTimer = 0.0f;
    }
}

void Enemy::Update(float deltaTime) {
    if (deltaTime <= 0.0f) return;
    
    UpdateAnimation(deltaTime);
}

void Enemy::Draw(std::int32_t textureResolution, 
                [[maybe_unused]] std::int32_t windowHeight, 
                [[maybe_unused]] std::int32_t windowWidth) const {
    if (m_textures.empty()) {
        std::cerr << "Warning: No textures available for enemy rendering" << std::endl;
        return;
    }
    
    const std::size_t frameIndex = static_cast<std::size_t>(m_currentFrame);
    if (frameIndex >= m_textures.size()) {
        std::cerr << "Warning: Invalid frame index " << frameIndex << 
                     " for enemy animation (max: " << m_textures.size() - 1 << ")" << std::endl;
        return;
    }
    
    // Handle texture flipping for direction
    const float textureWidth = (m_direction == EnemyDirection::Right) ? 
                              static_cast<float>(textureResolution) : 
                              -static_cast<float>(textureResolution);
    
    // Source rectangle (texture coordinates)
    const Rectangle sourceRect = {
        0.0f, 0.0f,
        textureWidth,
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
    
    // Draw the enemy texture
    DrawTexturePro(m_textures[frameIndex], sourceRect, destRect,
                   Vector2{0.0f, 0.0f}, 0.0f, WHITE);
}

} // namespace PlayAsGobo