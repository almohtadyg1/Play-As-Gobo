#pragma once

#include "raylib.h"
#include "raymath.h"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Ground.hpp"
#include "FinishLine.hpp"
#include "Explosion.hpp"

#include <vector>
#include <algorithm>
#include <random>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <array>
#include <cstdint>

namespace PlayAsGobo {

enum class CollisionSide : std::uint8_t {
    None,
    Left,
    Right,
    Top,
    Bottom
};

struct CollisionInfo {
    bool hasCollision{false};
    CollisionSide side{CollisionSide::None};
    float penetrationDepth{0.0f};
    Ground* collidedGround{nullptr};
};

enum class GameState : std::uint8_t {
    MainMenu,
    Playing,
    GameOver,
    Controls,
    Options,
    AskExit,
    Exit
};

struct Button {
    Rectangle bounds;
    const char* text;
    Color textColor;
    Color buttonColor;
    Color hoverColor;
    float fontSize;

    Button(Rectangle boundsRect, const char* buttonText, Color textCol, 
           Color buttonCol, Color hoverCol, float fontSz = 0.0f)
        : bounds(boundsRect)
        , text(buttonText)
        , textColor(textCol)
        , buttonColor(buttonCol)
        , hoverColor(hoverCol)
        , fontSize(fontSz == 0.0f ? boundsRect.height * 0.75f : fontSz) {
    }
};

class Game {
public:
    Game();
    ~Game();
    
    // Disable copy and move operations for RAII safety
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;
    
    [[nodiscard]] bool IsInitialized() const noexcept { return m_isInitialized; }
    void Run();

private:
    // Constants
    static constexpr float START_TEXTURE_SCALE = 2.0f;
    static constexpr float MAX_ENTITY_SCALE = 4.0f;
    static constexpr int FINISH_LINE_WIDTH = 7;
    static constexpr int TEXTURE_RESOLUTION = 16;
    static constexpr float MAX_GAME_HARDNESS = 1.0f;
    static constexpr float GRAVITY = 900.0f;
    static constexpr std::size_t COLOR_COUNT = 25;
    
    static const std::array<Color, COLOR_COUNT> COLOR_OPTIONS;

    // Window state
    int m_currentWindowWidth;
    int m_currentWindowHeight;
    int m_mapWidth;
    int m_mapHeight;
    bool m_isInitialized{false};

    // Game state
    bool m_shouldExit{false};
    GameState m_currentGameState{GameState::MainMenu};
    bool m_resetGame{false};
    float m_deltaTime{0.0f};
    float m_gameHardness{0.5f};
    Camera2D m_camera{};
    
    // Audio settings
    bool m_musicEnabled{true};
    bool m_soundEnabled{true};
    float m_musicVolume{1.0f};
    
    // UI state
    std::size_t m_selectedMainMenuOption{0};
    std::size_t m_selectedOptionsMenuOption{0};
    std::size_t m_selectedGameOverMenuOption{0};
    std::size_t m_selectedExitMenuOption{0};
    
    // Game settings
    int m_maxEnemies{5};
    Color m_backgroundColor{0, 169, 212, 255};
    
    // Enemy spawning
    float m_enemyScale{START_TEXTURE_SCALE};
    float m_enemySpawnTimer{0.0f};
    float m_enemySpawnInterval{4.0f};
    
    // Assets
    std::vector<Texture2D> m_playerTextures;
    std::vector<Texture2D> m_enemyTextures;
    Texture2D m_groundTexture{};
    Texture2D m_finishLineTexture{};
    
    // Audio assets
    Sound m_explosionSound{};
    Sound m_loseSound{};
    Sound m_hoverButtonSound{};
    Sound m_openButtonSound{};
    Sound m_backButtonSound{};
    Sound m_exitNoSound{};
    Sound m_exitDisappointingSound{};
    Music m_playerRunSound{};
    Music m_backgroundMusic{};

    // Game objects
    std::unique_ptr<Player> m_player;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Ground>> m_grounds;
    std::unique_ptr<FinishLine> m_finishLine;
    ExplosionManager m_explosionManager;
    
    // Private methods - Asset management
    [[nodiscard]] bool LoadAssets();
    void UnloadAssets() noexcept;
    
    // Private methods - Game logic
    void InitializeEntities();
    void SpawnEnemies();
    void UpdateGame();
    void ResetGame();
    void RestartGame();
    void SetGameOver();
    
    // Private methods - Physics
    void ApplyGravity(Entity* entity);
    void HandleGroundCollision(Entity* entity);
    void HandleEnemyCollision(Enemy* enemy);
    void HandleFinishLineCollision(Enemy* enemy);
    void HandleEnemyUnderMap(Enemy* enemy);
    void RemoveEnemy(Enemy* enemy);
    
    // Private methods - World generation
    void CreateGrounds();
    [[nodiscard]] float GetGroundHeight() const noexcept;
    
    // Private methods - Collision detection
    [[nodiscard]] CollisionInfo GetGroundCollisionInfo(Entity* entity) const;
    [[nodiscard]] CollisionSide GetCollisionSide(Circle circle1, Circle circle2) const;
    
    // Private methods - Input handling
    [[nodiscard]] Vector2 GetInputPosition() const;
    void HandleMainMenuInput();
    void HandleControlsMenuInput();
    void HandleOptionsMenuInput();
    void HandleGameOverMenuInput();
    void HandleExitMenuInput();
    
    // Private methods - Rendering
    void DrawMainMenu();
    void DrawControlsMenu();
    void DrawOptionsMenu();
    void DrawGameOverMenu();
    void DrawExitMenu();
    void DrawOptionsInstructions(float currentY, float centerX,
                            float availableWidth, int instrFontSize);
    void DrawOptionValue(std::size_t optionIndex, float valueX, float valueY, int valueFontSize, 
                          Color optionColor, float rightColumnX, float rightColumnWidth, 
                          float currentY, float buttonSize);
    
    // Private methods - Options handling
    [[nodiscard]] bool AreColorsEqual(Color color1, Color color2) const noexcept;
    void CycleToNextBackgroundColor();
    void CycleToPreviousBackgroundColor();
    [[nodiscard]] std::string GetColorName(Color color) const;
    
    // Private methods - Utility
    [[nodiscard]] int GenerateRandomInt(int min, int max);
    template<typename T>
    [[nodiscard]] constexpr T Clamp(T value, T min, T max) const noexcept {
        return (value < min) ? min : (value > max) ? max : value;
    }
};

} // namespace PlayAsGobo