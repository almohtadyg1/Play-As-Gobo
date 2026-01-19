#pragma once

#include "Entity.hpp"
#include <vector>
#include <cstdint>

namespace PlayAsGobo {

// Forward declarations
class Player;

enum class EnemyDirection : std::uint8_t {
    Right,
    Left
};

class Enemy : public Entity {
public:
    // Constructor
    Enemy(float x, float y, float radius,
          const std::vector<Texture2D>& enemyTextures,
          float speed = 200.0f, 
          EnemyDirection initialDirection = EnemyDirection::Right);
    
    // Disable copy operations (enemies should be unique)
    Enemy(const Enemy&) = delete;
    Enemy& operator=(const Enemy&) = delete;
    
    // Enable move operations
    Enemy(Enemy&&) = default;
    Enemy& operator=(Enemy&&) = default;
    
    // Destructor
    ~Enemy() override = default;
    
    // State getters
    [[nodiscard]] EnemyDirection GetDirection() const noexcept { return m_direction; }
    [[nodiscard]] bool IsMoving() const noexcept { return m_isMoving; }
    [[nodiscard]] float GetMoveSpeed() const noexcept { return m_moveSpeed; }
    [[nodiscard]] std::int32_t GetCurrentFrame() const noexcept { return static_cast<std::int32_t>(m_currentFrame); }
    
    // Actions
    void FlipDirection() noexcept;
    void SetDirection(EnemyDirection direction) noexcept { m_direction = direction; }
    void SetMoveSpeed(float speed);
    
    // AI and behavior
    void ExecuteAI(float deltaTime, float mapWidth, float finishLineX,
                  const Player& player, bool soundEnabled);
    
    // Override virtual methods from Entity
    void Update(float deltaTime) override;
    void Draw(std::int32_t textureResolution, 
             std::int32_t windowHeight, 
             std::int32_t windowWidth) const override;

private:
    // Constants
    static constexpr float DEFAULT_MOVE_SPEED = 200.0f;
    static constexpr float MIN_MOVE_SPEED = 1.0f;
    static constexpr float MAX_MOVE_SPEED = 1000.0f;
    static constexpr float ANIMATION_INTERVAL = 0.1f;
    static constexpr float PLAYER_DETECTION_RANGE = 200.0f;
    static constexpr float VOLUME_DISTANCE_FACTOR = 1000.0f;
    static constexpr float MIN_VOLUME_DISTANCE = 1.0f;
    
    // Animation frame indices
    enum class AnimationFrame : std::uint8_t {
        Idle = 0,
        Running1 = 1,
        Running2 = 2,
        Running3 = 3
    };
    
    // Member variables
    std::vector<Texture2D> m_textures;
    float m_moveSpeed;
    float m_animationTimer{0.0f};
    AnimationFrame m_currentFrame{AnimationFrame::Idle};
    EnemyDirection m_direction;
    bool m_isMoving{false};
    
    // Private helper methods
    void ValidateTextures() const;
    void ValidateSpeed(float speed) const;
    void UpdateAnimation(float deltaTime);
    void UpdateMovement(float deltaTime, float mapWidth, float finishLineX);
    void HandlePlayerProximityJump(const Player& player);
    [[nodiscard]] bool ShouldMoveRight(float finishLineX, float mapWidth) const noexcept;
    [[nodiscard]] bool ShouldMoveLeft(float finishLineX) const noexcept;
    [[nodiscard]] bool IsPlayerInJumpRange(const Player& player) const noexcept;
    [[nodiscard]] float CalculateVolumeByDistance(const Player& player) const noexcept;
};

} // namespace PlayAsGobo