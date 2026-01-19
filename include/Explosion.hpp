#pragma once

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <cstdint>
#include <memory>

namespace PlayAsGobo {

class Explosion {
public:
    // Constructor
    explicit Explosion(const Sound& explosionSound);
    
    // Disable copy operations for performance
    Explosion(const Explosion&) = delete;
    Explosion& operator=(const Explosion&) = delete;
    
    // Enable move operations
    Explosion(Explosion&&) = default;
    Explosion& operator=(Explosion&&) = default;
    
    // Destructor
    ~Explosion() = default;
    
    // State management
    void Start(Vector2 position, bool soundEnabled);
    void Stop() noexcept;
    void Reset() noexcept;
    
    // Updates and rendering
    void Update(float deltaTime);
    void Draw() const;
    
    // Getters
    [[nodiscard]] bool IsActive() const noexcept { return m_isActive; }
    [[nodiscard]] Vector2 GetPosition() const noexcept { return m_position; }
    [[nodiscard]] float GetRadius() const noexcept;
    [[nodiscard]] float GetDamageRadius() const noexcept;
    [[nodiscard]] float GetProgress() const noexcept;
    [[nodiscard]] bool IsInDamagePhase() const noexcept;
    
    // Configuration
    void SetMaxDuration(float duration);
    void SetMaxRadius(float radius);
    void SetParticleCount(std::int32_t count);

private:
    // Constants
    static constexpr float DEFAULT_MAX_DURATION = 1.5f;
    static constexpr float DEFAULT_MAX_RADIUS = 80.0f;
    static constexpr float DAMAGE_PHASE_DURATION = 0.3f;
    static constexpr float GRAVITY_ACCELERATION = 200.0f;
    static constexpr float AIR_RESISTANCE = 0.98f;
    static constexpr std::int32_t DEFAULT_PARTICLE_COUNT = 30;
    static constexpr std::int32_t MIN_PARTICLE_COUNT = 5;
    static constexpr std::int32_t MAX_PARTICLE_COUNT = 100;
    static constexpr float MIN_PARTICLE_SPEED = 100.0f;
    static constexpr float MAX_PARTICLE_SPEED = 250.0f;
    static constexpr float MIN_PARTICLE_LIFE = 0.8f;
    static constexpr float MAX_PARTICLE_LIFE = 1.5f;
    static constexpr float MIN_PARTICLE_SIZE = 3.0f;
    static constexpr float MAX_PARTICLE_SIZE = 8.0f;
    
    // Particle structure
    struct Particle {
        Vector2 position{0.0f, 0.0f};
        Vector2 velocity{0.0f, 0.0f};
        float life{0.0f};
        float maxLife{0.0f};
        Color color{WHITE};
        float size{0.0f};
        float initialSize{0.0f};
        
        Particle() = default;
        Particle(Vector2 pos, Vector2 vel, float lifespan, Color col, float sz)
            : position(pos), velocity(vel), life(lifespan), maxLife(lifespan), 
              color(col), size(sz), initialSize(sz) {}
    };
    
    // Member variables
    Vector2 m_position{0.0f, 0.0f};
    Sound m_sound{};
    std::vector<Particle> m_particles;
    float m_timer{0.0f};
    float m_maxDuration{DEFAULT_MAX_DURATION};
    float m_maxRadius{DEFAULT_MAX_RADIUS};
    std::int32_t m_particleCount{DEFAULT_PARTICLE_COUNT};
    bool m_isActive{false};
    
    // Private helper methods
    void ValidateDuration(float duration) const;
    void ValidateRadius(float radius) const;
    void ValidateParticleCount(std::int32_t count) const;
    void CreateParticles();
    void UpdateParticles(float deltaTime);
    void UpdateParticle(Particle& particle, float deltaTime);
    [[nodiscard]] Color CalculateParticleColor(float lifeRatio) const noexcept;
    void DrawExplosionCore() const;
    void DrawParticles() const;
    [[nodiscard]] float GenerateRandomFloat(float min, float max) const;
    [[nodiscard]] std::int32_t GenerateRandomInt(int min, int max) const;
};

class ExplosionManager {
public:
    // Constructor
    ExplosionManager() = default;
    
    // Disable copy operations
    ExplosionManager(const ExplosionManager&) = delete;
    ExplosionManager& operator=(const ExplosionManager&) = delete;
    
    // Enable move operations
    ExplosionManager(ExplosionManager&&) = default;
    ExplosionManager& operator=(ExplosionManager&&) = default;
    
    // Destructor
    ~ExplosionManager() = default;
    
    // Explosion management
    void CreateExplosion(Vector2 position, const Sound& explosionSound, bool soundEnabled);
    void Update(float deltaTime);
    void Draw() const;
    void Clear() noexcept;
    
    // Damage detection
    [[nodiscard]] bool CheckExplosionDamage(Vector2 position, float radius) const noexcept;
    [[nodiscard]] std::vector<Vector2> GetActiveExplosionPositions() const;
    
    // State queries
    [[nodiscard]] std::size_t GetActiveExplosionCount() const noexcept;
    [[nodiscard]] std::size_t GetTotalExplosionCount() const noexcept { return m_explosions.size(); }
    [[nodiscard]] bool HasActiveExplosions() const noexcept;
    
    // Configuration
    void SetMaxExplosions(std::size_t maxCount);
    void ReserveExplosions(std::size_t count);

private:
    // Constants
    static constexpr std::size_t DEFAULT_MAX_EXPLOSIONS = 50;
    static constexpr std::size_t MIN_MAX_EXPLOSIONS = 1;
    static constexpr std::size_t MAX_MAX_EXPLOSIONS = 500;
    
    // Member variables
    std::vector<std::unique_ptr<Explosion>> m_explosions;
    std::size_t m_maxExplosions{DEFAULT_MAX_EXPLOSIONS};
    
    // Private helper methods
    void ValidateMaxExplosions(std::size_t maxCount) const;
    [[nodiscard]] Explosion* FindInactiveExplosion() noexcept;
    void CreateNewExplosion(Vector2 position, const Sound& explosionSound, bool soundEnabled);
    void CleanupInactiveExplosions();
};

} // namespace PlayAsGobo