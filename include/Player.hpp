#pragma once

#include "Entity.hpp"
#include "Explosion.hpp"
#include <vector>
#include <cmath>
#include <cstdint>

namespace PlayAsGobo {

class Player : public Entity {
public:
    // Constructor
    Player(float x, float y, float radius,
           const std::vector<Texture2D>& playerTextures,
           const Music& walkSound, float scale, float speed = 200.0f);
    
    // Disable copy operations (players should be unique)
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;
    
    // Enable move operations
    Player(Player&&) = default;
    Player& operator=(Player&&) = default;
    
    // Destructor
    ~Player() override = default;
    
    // Game state getters
    [[nodiscard]] std::int32_t GetKillCount() const noexcept { return m_killCount; }
    [[nodiscard]] bool CanUseBomb() const noexcept { return m_canUseBomb; }
    [[nodiscard]] float GetSizeScale() const noexcept { return m_sizeScale; }
    [[nodiscard]] bool IsMoving() const noexcept { return m_isMoving; }
    [[nodiscard]] float GetMoveSpeed() const noexcept { return m_moveSpeed; }
    [[nodiscard]] const Music& GetWalkSound() const noexcept { return m_walkSound; }
    
    // Game actions
    void IncrementKillCount() noexcept { ++m_killCount; }
    void EnableBomb() noexcept { m_canUseBomb = true; }
    void TakeDamage();
    void GrowLarger();
    void ShrinkSize();
    void ResetToOriginalSize() noexcept;
    
    // Input and game logic
    void HandleInput(float deltaTime, const Rectangle& groundBounds,
                    ExplosionManager& explosionManager,
                    const Sound& explosionSound, bool soundEnabled);

    void SetWalkSound(const Music& sound) noexcept { m_walkSound = sound; }
    
    // Override virtual methods from Entity
    void Update(float deltaTime) override;
    void Draw(std::int32_t textureResolution, 
             std::int32_t windowHeight, 
             std::int32_t windowWidth) const override;

private:
    // Constants
    static constexpr float DEFAULT_MOVE_SPEED = 200.0f;
    static constexpr float DAMAGE_SCALE_FACTOR = 0.75f;
    static constexpr float GROWTH_SCALE_FACTOR = 1.2f;
    static constexpr float SHRINK_SCALE_FACTOR = 1.1f;
    static constexpr float BOMB_RADIUS_REDUCTION = 0.75f;
    static constexpr float ANIMATION_INTERVAL = 0.2f;
    static constexpr float BOMB_TEXT_PULSE_SPEED = 4.0f;
    static constexpr float BOMB_GLOW_OPACITY = 0.8f;
    
    // Animation frame indices
    enum class AnimationFrame : std::uint8_t {
        Standing = 0,
        Walking = 1,
        Idle = 2
    };
    
    // Member variables
    std::vector<Texture2D> m_textures;
    Music m_walkSound;
    float m_moveSpeed;
    float m_originalRadius;
    float m_sizeScale;
    float m_animationTimer{0.0f};
    std::int32_t m_killCount{0};
    AnimationFrame m_currentFrame{AnimationFrame::Standing};
    bool m_isMoving{false};
    bool m_canUseBomb{false};
    
    // Private helper methods
    void UpdateAnimation(float deltaTime);
    void HandleMovementInput(float deltaTime, const Rectangle& groundBounds, bool soundEnabled);
    void HandleBombInput(ExplosionManager& explosionManager, const Sound& explosionSound, bool soundEnabled);
    void UpdateRadius() noexcept;
    void DrawBombIndicator(std::int32_t windowHeight) const;
    void ValidateTextures() const;
    void ValidateScale(float scale) const;
    void ValidateSpeed(float speed) const;
};

} // namespace PlayAsGobo