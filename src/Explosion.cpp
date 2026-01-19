#include "Explosion.hpp"
#include <stdexcept>
#include <algorithm>
#include <random>
#include <cmath>
#include <iostream>

namespace PlayAsGobo {

// Explosion class implementation
Explosion::Explosion(const Sound& explosionSound)
    : m_sound(explosionSound) {
    m_particles.reserve(m_particleCount);
}

void Explosion::ValidateDuration(float duration) const {
    constexpr float MIN_DURATION = 0.1f;
    constexpr float MAX_DURATION = 10.0f;
    
    if (duration < MIN_DURATION || duration > MAX_DURATION) {
        throw std::invalid_argument("Explosion duration must be between " + 
                                  std::to_string(MIN_DURATION) + " and " + 
                                  std::to_string(MAX_DURATION) + " seconds");
    }
}

void Explosion::ValidateRadius(float radius) const {
    constexpr float MIN_RADIUS = 1.0f;
    constexpr float MAX_RADIUS = 1000.0f;
    
    if (radius < MIN_RADIUS || radius > MAX_RADIUS) {
        throw std::invalid_argument("Explosion radius must be between " + 
                                  std::to_string(MIN_RADIUS) + " and " + 
                                  std::to_string(MAX_RADIUS));
    }
}

void Explosion::ValidateParticleCount(std::int32_t count) const {
    if (count < MIN_PARTICLE_COUNT || count > MAX_PARTICLE_COUNT) {
        throw std::invalid_argument("Particle count must be between " + 
                                  std::to_string(MIN_PARTICLE_COUNT) + " and " + 
                                  std::to_string(MAX_PARTICLE_COUNT));
    }
}

float Explosion::GenerateRandomFloat(float min, float max) const {
    thread_local static std::random_device randomDevice;
    thread_local static std::mt19937 generator(randomDevice());
    
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

std::int32_t Explosion::GenerateRandomInt(int min, int max) const {
    thread_local static std::random_device randomDevice;
    thread_local static std::mt19937 generator(randomDevice());
    
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

void Explosion::Start(Vector2 position, bool soundEnabled) {
    m_position = position;
    m_timer = 0.0f;
    m_isActive = true;
    
    // Play explosion sound
    if (soundEnabled && m_sound.frameCount > 0) {
        PlaySound(m_sound);
    }
    
    CreateParticles();
}

void Explosion::Stop() noexcept {
    m_isActive = false;
}

void Explosion::Reset() noexcept {
    m_timer = 0.0f;
    m_isActive = false;
    m_particles.clear();
}

void Explosion::CreateParticles() {
    m_particles.clear();
    m_particles.reserve(m_particleCount);
    
    const float angleStep = (2.0f * PI) / m_particleCount;
    
    for (std::int32_t i = 0; i < m_particleCount; ++i) {
        const float angle = i * angleStep + GenerateRandomFloat(-0.2f, 0.2f); // Add some randomness
        const float speed = GenerateRandomFloat(MIN_PARTICLE_SPEED, MAX_PARTICLE_SPEED);
        const float life = GenerateRandomFloat(MIN_PARTICLE_LIFE, MAX_PARTICLE_LIFE);
        const float size = GenerateRandomFloat(MIN_PARTICLE_SIZE, MAX_PARTICLE_SIZE);
        
        const Vector2 velocity = {
            std::cos(angle) * speed,
            std::sin(angle) * speed
        };
        
        m_particles.emplace_back(m_position, velocity, life, WHITE, size);
    }
}

Color Explosion::CalculateParticleColor(float lifeRatio) const noexcept {
    // Color progression: white -> yellow -> orange -> red -> maroon
    Color color;
    
    if (lifeRatio > 0.8f) {
        color = WHITE;
    } else if (lifeRatio > 0.6f) {
        color = YELLOW;
    } else if (lifeRatio > 0.4f) {
        color = ORANGE;
    } else if (lifeRatio > 0.2f) {
        color = RED;
    } else {
        color = MAROON;
    }
    
    // Apply alpha based on life ratio
    color.a = static_cast<unsigned char>(255 * lifeRatio);
    
    return color;
}

void Explosion::UpdateParticle(Particle& particle, float deltaTime) {
    if (particle.life <= 0.0f) return;
    
    // Update position
    particle.position.x += particle.velocity.x * deltaTime;
    particle.position.y += particle.velocity.y * deltaTime;
    
    // Apply physics
    particle.velocity.y += GRAVITY_ACCELERATION * deltaTime; // Gravity
    particle.velocity.x *= AIR_RESISTANCE; // Air resistance
    particle.velocity.y *= AIR_RESISTANCE;
    
    // Update life
    particle.life -= deltaTime;
    
    // Update visual properties
    const float lifeRatio = std::max(0.0f, particle.life / particle.maxLife);
    particle.color = CalculateParticleColor(lifeRatio);
    particle.size = particle.initialSize * lifeRatio;
}

void Explosion::UpdateParticles(float deltaTime) {
    for (auto& particle : m_particles) {
        UpdateParticle(particle, deltaTime);
    }
}

void Explosion::Update(float deltaTime) {
    if (!m_isActive || deltaTime <= 0.0f) return;
    
    m_timer += deltaTime;
    
    UpdateParticles(deltaTime);
    
    // Check if explosion should end
    if (m_timer >= m_maxDuration) {
        m_isActive = false;
    }
}

float Explosion::GetRadius() const noexcept {
    if (!m_isActive) return 0.0f;
    
    const float progress = std::min(m_timer / m_maxDuration, 1.0f);
    return progress * m_maxRadius;
}

float Explosion::GetDamageRadius() const noexcept {
    return m_isActive ? GetRadius() : 0.0f;
}

float Explosion::GetProgress() const noexcept {
    if (!m_isActive) return 1.0f;
    
    return std::min(m_timer / m_maxDuration, 1.0f);
}

bool Explosion::IsInDamagePhase() const noexcept {
    return m_isActive;
}

void Explosion::DrawExplosionCore() const {
    const float radius = GetRadius();
    const float progress = GetProgress();
    const float alpha = 1.0f - progress;
    
    // Main explosion circle (expanding and fading)
    const Color explosionColor = {255, 100, 0, static_cast<unsigned char>(255 * alpha)};
    DrawCircle(static_cast<int>(m_position.x), static_cast<int>(m_position.y), 
              radius, explosionColor);
    
    // Inner bright circle (only during early phase)
    if (m_timer < DAMAGE_PHASE_DURATION) {
        const float innerAlpha = 1.0f - (m_timer / DAMAGE_PHASE_DURATION);
        const Color innerColor = {255, 255, 255, static_cast<unsigned char>(255 * innerAlpha)};
        const float innerRadius = radius * 0.5f;
        
        DrawCircle(static_cast<int>(m_position.x), static_cast<int>(m_position.y), 
                  innerRadius, innerColor);
    }
}

void Explosion::DrawParticles() const {
    for (const auto& particle : m_particles) {
        if (particle.life > 0.0f) {
            DrawCircle(static_cast<int>(particle.position.x), 
                      static_cast<int>(particle.position.y), 
                      particle.size, particle.color);
        }
    }
}

void Explosion::Draw() const {
    if (!m_isActive) return;
    
    DrawExplosionCore();
    DrawParticles();
}

void Explosion::SetMaxDuration(float duration) {
    ValidateDuration(duration);
    m_maxDuration = duration;
}

void Explosion::SetMaxRadius(float radius) {
    ValidateRadius(radius);
    m_maxRadius = radius;
}

void Explosion::SetParticleCount(std::int32_t count) {
    ValidateParticleCount(count);
    m_particleCount = count;
    m_particles.reserve(count);
}

// ExplosionManager class implementation
void ExplosionManager::ValidateMaxExplosions(std::size_t maxCount) const {
    if (maxCount < MIN_MAX_EXPLOSIONS || maxCount > MAX_MAX_EXPLOSIONS) {
        throw std::invalid_argument("Max explosions must be between " + 
                                  std::to_string(MIN_MAX_EXPLOSIONS) + " and " + 
                                  std::to_string(MAX_MAX_EXPLOSIONS));
    }
}

Explosion* ExplosionManager::FindInactiveExplosion() noexcept {
    for (auto& explosion : m_explosions) {
        if (explosion && !explosion->IsActive()) {
            return explosion.get();
        }
    }
    return nullptr;
}

void ExplosionManager::CreateNewExplosion(Vector2 position, const Sound& explosionSound, bool soundEnabled) {
    // Remove old explosions if we're at the limit
    if (m_explosions.size() >= m_maxExplosions) {
        CleanupInactiveExplosions();
    }
    
    // Only create new explosion if we still have room
    if (m_explosions.size() < m_maxExplosions) {
        auto explosion = std::make_unique<Explosion>(explosionSound);
        explosion->Start(position, soundEnabled);
        m_explosions.push_back(std::move(explosion));
    } else {
        std::cerr << "Warning: Max explosion limit (" << m_maxExplosions 
                  << ") reached. Skipping explosion creation." << std::endl;
    }
}

void ExplosionManager::CleanupInactiveExplosions() {
    m_explosions.erase(
        std::remove_if(m_explosions.begin(), m_explosions.end(),
                      [](const std::unique_ptr<Explosion>& explosion) {
                          return !explosion || !explosion->IsActive();
                      }),
        m_explosions.end()
    );
}

void ExplosionManager::CreateExplosion(Vector2 position, const Sound& explosionSound, bool soundEnabled) {
    // Try to reuse an inactive explosion
    if (Explosion* inactiveExplosion = FindInactiveExplosion()) {
        inactiveExplosion->Start(position, soundEnabled);
        return;
    }
    
    // Create new explosion if no inactive ones available
    CreateNewExplosion(position, explosionSound, soundEnabled);
}

void ExplosionManager::Update(float deltaTime) {
    if (deltaTime <= 0.0f) return;
    
    for (auto& explosion : m_explosions) {
        if (explosion) {
            explosion->Update(deltaTime);
        }
    }
}

void ExplosionManager::Draw() const {
    for (const auto& explosion : m_explosions) {
        if (explosion) {
            explosion->Draw();
        }
    }
}

void ExplosionManager::Clear() noexcept {
    m_explosions.clear();
}

bool ExplosionManager::CheckExplosionDamage(Vector2 position, float radius) const noexcept {
    for (const auto& explosion : m_explosions) {
        if (!explosion || !explosion->IsInDamagePhase()) {
            continue;
        }
        
        const float distance = Vector2Distance(explosion->GetPosition(), position);
        const float totalRadius = explosion->GetDamageRadius() + radius;
        if (distance < totalRadius) {
            return true;
        }
    }
    return false;
}

std::vector<Vector2> ExplosionManager::GetActiveExplosionPositions() const {
    std::vector<Vector2> positions;
    positions.reserve(m_explosions.size());
    
    for (const auto& explosion : m_explosions) {
        if (explosion && explosion->IsActive()) {
            positions.push_back(explosion->GetPosition());
        }
    }
    
    return positions;
}

std::size_t ExplosionManager::GetActiveExplosionCount() const noexcept {
    return std::count_if(m_explosions.begin(), m_explosions.end(),
                        [](const std::unique_ptr<Explosion>& explosion) {
                            return explosion && explosion->IsActive();
                        });
}

bool ExplosionManager::HasActiveExplosions() const noexcept {
    return std::any_of(m_explosions.begin(), m_explosions.end(),
                      [](const std::unique_ptr<Explosion>& explosion) {
                          return explosion && explosion->IsActive();
                      });
}

void ExplosionManager::SetMaxExplosions(std::size_t maxCount) {
    ValidateMaxExplosions(maxCount);
    m_maxExplosions = maxCount;
    
    // Cleanup if we now have too many explosions
    if (m_explosions.size() > m_maxExplosions) {
        CleanupInactiveExplosions();
        
        // If still too many, remove oldest ones
        while (m_explosions.size() > m_maxExplosions) {
            m_explosions.erase(m_explosions.begin());
        }
    }
}

void ExplosionManager::ReserveExplosions(std::size_t count) {
    if (count <= m_maxExplosions) {
        m_explosions.reserve(count);
    }
}

} // namespace PlayAsGobo