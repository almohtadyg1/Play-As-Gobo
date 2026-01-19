#include "Game.hpp"
#include <cassert>
#include <stdexcept>

namespace PlayAsGobo {

// Static color array definition
const std::array<Color, Game::COLOR_COUNT> Game::COLOR_OPTIONS = {{
    {0, 169, 212, 255}, BLACK, WHITE, GREEN, BLUE, YELLOW, ORANGE, PURPLE, BROWN,
    DARKGRAY, GRAY, LIGHTGRAY, PINK, MAGENTA, DARKGREEN, DARKBLUE,
    DARKPURPLE, DARKBROWN, RAYWHITE, GOLD, LIME, BEIGE, SKYBLUE, VIOLET
}};

Game::Game() 
    : m_currentWindowWidth(854)
    , m_currentWindowHeight(480)
    , m_mapWidth(0)
    , m_mapHeight(0) {
    
    try {
        // Configure window before creation
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(m_currentWindowWidth, m_currentWindowHeight, "Play as Gobo!");
        
        if (!IsWindowReady()) {
            throw std::runtime_error("Failed to initialize window");
        }

        InitAudioDevice();
        if (!IsAudioDeviceReady()) {
            CloseWindow();
            throw std::runtime_error("Failed to initialize audio device");
        }

        SetExitKey(KEY_NULL);

        // Update actual window dimensions
        m_currentWindowWidth = GetScreenWidth();
        m_currentWindowHeight = GetScreenHeight();
        m_mapWidth = static_cast<int>(m_currentWindowWidth * 1.5f);
        m_mapHeight = static_cast<int>(m_currentWindowHeight * 1.5f);

        // Set window icon
        if (Image icon = LoadImage("assets/img/Gobo/Gobo0.png"); icon.data != nullptr) {
            SetWindowIcon(icon);
            UnloadImage(icon);
        }

        // Initialize camera
        m_camera.offset = {m_currentWindowWidth / 2.0f, m_currentWindowHeight / 2.0f};
        m_camera.rotation = 0.0f;
        m_camera.zoom = 1.0f;

        // Load game assets
        if (!LoadAssets()) {
            throw std::runtime_error("Failed to load game assets");
        }

        // Setup background music
        PlayMusicStream(m_backgroundMusic);
        m_backgroundMusic.looping = true;

        SetTargetFPS(60);
        m_isInitialized = true;
        
    } catch (const std::exception& e) {
        std::cerr << "Game initialization error: " << e.what() << std::endl;
        UnloadAssets();
        
        if (IsAudioDeviceReady()) {
            CloseAudioDevice();
        }
        if (IsWindowReady()) {
            CloseWindow();
        }
        
        m_isInitialized = false;
    }
}

Game::~Game() {
    UnloadAssets();
    
    if (IsAudioDeviceReady()) {
        CloseAudioDevice();
    }
    if (IsWindowReady()) {
        CloseWindow();
    }
}

bool Game::LoadAssets() {
    auto loadTextureChecked = [](const char* path) -> std::optional<Texture2D> {
        if (Texture2D texture = LoadTexture(path); texture.id != 0) {
            return texture;
        }
        std::cerr << "Failed to load texture: " << path << std::endl;
        return std::nullopt;
    };

    auto loadSoundChecked = [](const char* path) -> std::optional<Sound> {
        if (Sound sound = LoadSound(path); sound.frameCount != 0) {
            return sound;
        }
        std::cerr << "Failed to load sound: " << path << std::endl;
        return std::nullopt;
    };

    auto loadMusicChecked = [](const char* path) -> std::optional<Music> {
        if (Music music = LoadMusicStream(path); music.frameCount != 0) {
            return music;
        }
        std::cerr << "Failed to load music: " << path << std::endl;
        return std::nullopt;
    };

    // Load player textures
    const std::array<const char*, 3> playerPaths = {
        "assets/img/Gobo/Gobo0.png",
        "assets/img/Gobo/Gobo1.png", 
        "assets/img/Gobo/Gobo2.png"
    };
    
    m_playerTextures.reserve(playerPaths.size());
    for (const char* path : playerPaths) {
        if (auto texture = loadTextureChecked(path)) {
            m_playerTextures.push_back(*texture);
        } else {
            return false;
        }
    }

    // Load enemy textures
    const std::array<const char*, 4> enemyPaths = {
        "assets/img/Juicy Boy's Brother/Juicy Boy's Brother0.png",
        "assets/img/Juicy Boy's Brother/Juicy Boy's Brother1.png",
        "assets/img/Juicy Boy's Brother/Juicy Boy's Brother2.png",
        "assets/img/Juicy Boy's Brother/Juicy Boy's Brother3.png"
    };
    
    m_enemyTextures.reserve(enemyPaths.size());
    for (const char* path : enemyPaths) {
        if (auto texture = loadTextureChecked(path)) {
            m_enemyTextures.push_back(*texture);
        } else {
            return false;
        }
    }

    // Load other textures
    if (auto groundTexture = loadTextureChecked("assets/img/Ground.png")) {
        m_groundTexture = *groundTexture;
    } else return false;

    if (auto finishTexture = loadTextureChecked("assets/img/FinishLine.png")) {
        m_finishLineTexture = *finishTexture;
    } else return false;

    // Load sounds
    const std::array<std::pair<const char*, Sound*>, 7> soundPaths = {{
        {"assets/audio/explosion.wav", &m_explosionSound},
        {"assets/audio/LoseSound.wav", &m_loseSound},
        {"assets/audio/HoverOnButtonSound.wav", &m_hoverButtonSound},
        {"assets/audio/OpenButtonSound.wav", &m_openButtonSound},
        {"assets/audio/BackButtonSound.wav", &m_backButtonSound},
        {"assets/audio/NO!.wav", &m_exitNoSound},
        {"assets/audio/Disappointing.wav", &m_exitDisappointingSound}
    }};

    for (const auto& [path, soundPtr] : soundPaths) {
        if (auto sound = loadSoundChecked(path)) {
            *soundPtr = *sound;
        } else {
            return false;
        }
    }

    // Load music
    const std::array<std::pair<const char*, Music*>, 2> musicPaths = {{
        {"assets/audio/Gobo's Run Sound.wav", &m_playerRunSound},
        {"assets/audio/music.mp3", &m_backgroundMusic}
    }};

    for (const auto& [path, musicPtr] : musicPaths) {
        if (auto music = loadMusicChecked(path)) {
            *musicPtr = *music;
        } else {
            return false;
        }
    }

    return true;
}

void Game::UnloadAssets() noexcept {
    // Unload textures
    for (auto& texture : m_playerTextures) {
        if (texture.id != 0) UnloadTexture(texture);
    }
    m_playerTextures.clear();

    for (auto& texture : m_enemyTextures) {
        if (texture.id != 0) UnloadTexture(texture);
    }
    m_enemyTextures.clear();

    if (m_groundTexture.id != 0) UnloadTexture(m_groundTexture);
    if (m_finishLineTexture.id != 0) UnloadTexture(m_finishLineTexture);

    // Unload sounds
    if (m_explosionSound.frameCount != 0) UnloadSound(m_explosionSound);
    if (m_loseSound.frameCount != 0) UnloadSound(m_loseSound);
    if (m_hoverButtonSound.frameCount != 0) UnloadSound(m_hoverButtonSound);
    if (m_openButtonSound.frameCount != 0) UnloadSound(m_openButtonSound);
    if (m_backButtonSound.frameCount != 0) UnloadSound(m_backButtonSound);
    if (m_exitNoSound.frameCount != 0) UnloadSound(m_exitNoSound);
    if (m_exitDisappointingSound.frameCount != 0) UnloadSound(m_exitDisappointingSound);

    // Unload music
    if (m_playerRunSound.frameCount != 0) UnloadMusicStream(m_playerRunSound);
    if (m_backgroundMusic.frameCount != 0) UnloadMusicStream(m_backgroundMusic);
}

void Game::CreateGrounds() {
    if (m_currentWindowWidth <= 0 || m_currentWindowHeight <= 0) {
        return;
    }
    
    const float groundHeight = GetGroundHeight();
    const float mainGroundY = m_currentWindowHeight - groundHeight;
    const float groundX = (m_currentWindowWidth - m_mapWidth) / 2.0f;
    
    auto mainGround = std::make_unique<Ground>(
        groundX, mainGroundY, static_cast<float>(m_mapWidth), 
        groundHeight, m_groundTexture
    );
    
    if (mainGround) {
        m_grounds.push_back(std::move(mainGround));
    }
}

float Game::GetGroundHeight() const noexcept {
    constexpr float GROUND_HEIGHT_PERCENT = 0.20f;
    constexpr float MIN_GROUND_HEIGHT = 60.0f;
    constexpr float MAX_GROUND_HEIGHT_PERCENT = 0.3f;
    
    const float groundHeight = m_currentWindowHeight * GROUND_HEIGHT_PERCENT;
    const float maxGroundHeight = m_currentWindowHeight * MAX_GROUND_HEIGHT_PERCENT;
    
    return Clamp(groundHeight, MIN_GROUND_HEIGHT, maxGroundHeight);
}

Vector2 Game::GetInputPosition() const {
    // Check for mouse input first
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        return GetMousePosition();
    }
    
    // Check for touch input
    if (GetTouchPointCount() > 0) {
        return GetTouchPosition(0);
    }
    
    return {-1.0f, -1.0f}; // No input detected
}

void Game::InitializeEntities() {
    // Clear existing entities
    m_player.reset();
    m_enemies.clear();
    m_grounds.clear();
    m_finishLine.reset();

    // Update map dimensions
    m_mapWidth = static_cast<int>(m_currentWindowWidth * 1.5f);
    m_mapHeight = static_cast<int>(m_currentWindowHeight * 1.5f);

    CreateGrounds();

    // Create finish line
    const float screenCenterX = m_currentWindowWidth / 2.0f;
    const float finishLineWidth = (m_finishLineTexture.id != 0) ? 
        m_finishLineTexture.width * FINISH_LINE_WIDTH : 200.0f;
    const float finishLineHeight = (m_finishLineTexture.id != 0) ? 
        static_cast<float>(m_finishLineTexture.height) : 50.0f;
    
    const float groundY = m_currentWindowHeight - GetGroundHeight();
    const float finishLineY = groundY - finishLineHeight;
    const float finishLineX = screenCenterX - finishLineWidth / 2.0f;
    
    m_finishLine = std::make_unique<FinishLine>(
        finishLineX, finishLineY, finishLineWidth, 
        finishLineHeight, m_finishLineTexture
    );
    
    // Create player
    const float playerRadius = (!m_playerTextures.empty() && m_playerTextures[0].id != 0) ? 
        static_cast<float>(m_playerTextures[0].width) : TEXTURE_RESOLUTION;
    
    const float playerStartX = screenCenterX;
    const float playerStartY = groundY - playerRadius - 50.0f;
    
    m_player = std::make_unique<Player>(
        playerStartX, playerStartY, playerRadius, 
        m_playerTextures, m_playerRunSound, START_TEXTURE_SCALE
    );
}

void Game::SpawnEnemies() {
    if (m_enemies.size() >= static_cast<std::size_t>(m_maxEnemies)) {
        return;
    }
    
    m_enemySpawnTimer += m_deltaTime;
    if (m_enemySpawnTimer < m_enemySpawnInterval) {
        return;
    }
    
    const float enemyRadius = (!m_enemyTextures.empty() && m_enemyTextures[0].id != 0) ? 
        m_enemyTextures[0].width * m_enemyScale : TEXTURE_RESOLUTION * m_enemyScale;
    
    std::unique_ptr<Enemy> enemy;
    const bool spawnFromLeft = (GenerateRandomInt(0, 1) == 0);
    
    if (spawnFromLeft) {
        enemy = std::make_unique<Enemy>(
            m_camera.target.x - m_currentWindowWidth/2.0f - 10.0f, 
            static_cast<float>(m_currentWindowHeight) / GenerateRandomInt(2, 4),
            enemyRadius, m_enemyTextures,
            200.0f, EnemyDirection::Right
        );
    } else {
        enemy = std::make_unique<Enemy>(
            m_camera.target.x + m_currentWindowWidth/2.0f + 10.0f, 
            static_cast<float>(m_currentWindowHeight) / GenerateRandomInt(2, 4),
            enemyRadius, m_enemyTextures,
            200.0f, EnemyDirection::Left
        );
    }

    if (enemy) {
        m_enemies.push_back(std::move(enemy));
    }

    m_enemySpawnTimer = 0.0f;
    m_enemySpawnInterval *= 0.75f; // Make spawning faster over time
}

void Game::ApplyGravity(Entity* entity) {
    if (!entity) return;
    
    entity->SetVelocityY(entity->GetVelocityY() + GRAVITY * m_deltaTime);
    entity->SetY((entity->GetY() - entity->GetRadius() + 
                 entity->GetVelocityY() * m_deltaTime) + entity->GetRadius());
}

CollisionInfo Game::GetGroundCollisionInfo(Entity* entity) const {
    CollisionInfo info{};
    if (!entity) return info;
    
    const Circle entityBounds = entity->GetBounds();

    for (const auto& ground : m_grounds) {
        if (!ground || !ground->CheckCollision(entityBounds)) {
            continue;
        }
        
        const Rectangle groundBounds = ground->GetBounds();
        
        // Calculate overlaps
        const float overlapLeft = (entityBounds.center.x + entityBounds.radius) - groundBounds.x;
        const float overlapRight = (groundBounds.x + groundBounds.width) - 
                                  (entityBounds.center.x - entityBounds.radius);
        const float overlapTop = (entityBounds.center.y + entityBounds.radius) - groundBounds.y;
        const float overlapBottom = (groundBounds.y + groundBounds.height) - 
                                   (entityBounds.center.y - entityBounds.radius);
        
        const float minOverlapX = std::min(overlapLeft, overlapRight);
        const float minOverlapY = std::min(overlapTop, overlapBottom);
        
        info.hasCollision = true;
        info.collidedGround = ground.get();
        
        if (minOverlapX < minOverlapY) {
            // X-axis collision
            if (overlapLeft < overlapRight) {
                info.side = CollisionSide::Left;
                info.penetrationDepth = overlapLeft;
            } else {
                info.side = CollisionSide::Right;
                info.penetrationDepth = overlapRight;
            }
        } else {
            // Y-axis collision
            if (overlapTop < overlapBottom) {
                info.side = CollisionSide::Top;
                info.penetrationDepth = overlapTop;
            } else {
                info.side = CollisionSide::Bottom;
                info.penetrationDepth = overlapBottom;
            }
        }
        
        break; // Return first collision found
    }
    
    return info;
}

CollisionSide Game::GetCollisionSide(Circle circle1, Circle circle2) const {
    const float deltaX = circle2.center.x - circle1.center.x;
    const float deltaY = circle2.center.y - circle1.center.y;
    
    if (std::abs(deltaX) > std::abs(deltaY) * m_gameHardness) {
        return (deltaX > 0) ? CollisionSide::Right : CollisionSide::Left;
    } else {
        return (deltaY > 0) ? CollisionSide::Bottom : CollisionSide::Top;
    }
}

void Game::HandleGroundCollision(Entity* entity) {
    if (!entity || entity->CanPhase()) {
        return;
    }
    
    const CollisionInfo collision = GetGroundCollisionInfo(entity);
    
    if (!collision.hasCollision) {
        if (entity->IsOnGround()) {
            entity->SetOnGround(false);
        }
        return;
    }

    switch (collision.side) {
        case CollisionSide::Top:
            if (entity->GetVelocityY() > 0) {
                entity->SetOnGround(true);
                entity->SetVelocityY(0.0f);
                entity->SetY(collision.collidedGround->GetY() - entity->GetRadius());
            }
            break;
            
        case CollisionSide::Left:
            entity->SetX(collision.collidedGround->GetX() - entity->GetRadius());
            break;
            
        case CollisionSide::Right:
            entity->SetX(collision.collidedGround->GetX() + 
                        collision.collidedGround->GetWidth() + entity->GetRadius());
            break;
            
        case CollisionSide::Bottom:
            if (entity->GetVelocityY() < 0) {
                entity->SetVelocityY(0.0f);
                entity->SetY(collision.collidedGround->GetY() + 
                           collision.collidedGround->GetHeight() + entity->GetRadius());
            }
            break;
            
        case CollisionSide::None:
            break;
    }
    
    // Safety check for player getting stuck in ground
    if (entity == m_player.get() && collision.hasCollision) {
        const float groundSurfaceY = collision.collidedGround->GetY();
        const float playerBottom = entity->GetY() + entity->GetRadius();
        
        if (playerBottom > groundSurfaceY) {
            entity->SetY(groundSurfaceY - entity->GetRadius());
            entity->SetVelocityY(0.0f);
            entity->SetOnGround(true);
        }
    }
}

void Game::HandleEnemyCollision(Enemy* enemy) {
    if (!enemy || !m_player) return;
    
    if (!CheckCollisionCircles(m_player->GetCenter(), m_player->GetRadius(),
                              enemy->GetCenter(), enemy->GetRadius())) {
        return;
    }
    
    const CollisionSide side = GetCollisionSide(m_player->GetBounds(), enemy->GetBounds());
    
    switch (side) {
    case CollisionSide::Right:
    case CollisionSide::Left:
        m_player->IncrementKillCount();
        if (m_player->GetSizeScale() < MAX_ENTITY_SCALE) {
            m_player->GrowLarger();
        } else if (!m_player->CanUseBomb()) {
            m_player->EnableBomb();
        }
        RemoveEnemy(enemy);
        break;
        
    case CollisionSide::Top:
        m_player->TakeDamage();
        enemy->Jump();
        break;
        
    default:
        break;
    }
}

void Game::HandleFinishLineCollision(Enemy* enemy) {
    if (!enemy || !m_player || !m_finishLine) return;
    
    if (CheckCollisionCircleRec(enemy->GetCenter(), enemy->GetRadius(), 
                               m_finishLine->GetBounds())) {
        m_player->ShrinkSize();
        RemoveEnemy(enemy);
    }
}

void Game::HandleEnemyUnderMap(Enemy* enemy) {
    if (!enemy) return;
    
    if (enemy->GetY() > m_currentWindowHeight) {
        RemoveEnemy(enemy);
    }
}

void Game::RemoveEnemy(Enemy* enemy) {
    const auto it = std::find_if(m_enemies.begin(), m_enemies.end(),
        [enemy](const std::unique_ptr<Enemy>& ptr) {
            return ptr.get() == enemy;
        });
    
    if (it != m_enemies.end()) {
        m_enemies.erase(it);
    }
}

int Game::GenerateRandomInt(int min, int max) {
    thread_local static std::random_device randomDevice;
    thread_local static std::mt19937 generator(randomDevice());
    
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

bool Game::AreColorsEqual(Color color1, Color color2) const noexcept {
    return color1.r == color2.r && color1.g == color2.g && 
           color1.b == color2.b && color1.a == color2.a;
}

void Game::CycleToNextBackgroundColor() {
    std::size_t currentIndex = 0;
    for (std::size_t i = 0; i < COLOR_COUNT; ++i) {
        if (AreColorsEqual(m_backgroundColor, COLOR_OPTIONS[i])) {
            currentIndex = i;
            break;
        }
    }
    
    currentIndex = (currentIndex + 1) % COLOR_COUNT;
    m_backgroundColor = COLOR_OPTIONS[currentIndex];
}

void Game::CycleToPreviousBackgroundColor() {
    std::size_t currentIndex = 0;
    for (std::size_t i = 0; i < COLOR_COUNT; ++i) {
        if (AreColorsEqual(m_backgroundColor, COLOR_OPTIONS[i])) {
            currentIndex = i;
            break;
        }
    }
    
    currentIndex = (currentIndex - 1 + COLOR_COUNT) % COLOR_COUNT;
    m_backgroundColor = COLOR_OPTIONS[currentIndex];
}

std::string Game::GetColorName(Color color) const {
    if (AreColorsEqual(color, {0, 169, 212, 255})) return "Sky Blue (Default)";
    if (AreColorsEqual(color, BLACK)) return "Black";
    if (AreColorsEqual(color, WHITE)) return "White";
    if (AreColorsEqual(color, GREEN)) return "Green";
    if (AreColorsEqual(color, BLUE)) return "Blue";
    if (AreColorsEqual(color, YELLOW)) return "Yellow";
    if (AreColorsEqual(color, ORANGE)) return "Orange";
    if (AreColorsEqual(color, PURPLE)) return "Purple";
    if (AreColorsEqual(color, BROWN)) return "Brown";
    if (AreColorsEqual(color, DARKGRAY)) return "Dark Gray";
    if (AreColorsEqual(color, GRAY)) return "Gray";
    if (AreColorsEqual(color, LIGHTGRAY)) return "Light Gray";
    if (AreColorsEqual(color, PINK)) return "Pink";
    if (AreColorsEqual(color, MAGENTA)) return "Magenta";
    if (AreColorsEqual(color, DARKGREEN)) return "Dark Green";
    if (AreColorsEqual(color, DARKBLUE)) return "Dark Blue";
    if (AreColorsEqual(color, DARKPURPLE)) return "Dark Purple";
    if (AreColorsEqual(color, DARKBROWN)) return "Dark Brown";
    if (AreColorsEqual(color, RAYWHITE)) return "Ray White";
    if (AreColorsEqual(color, GOLD)) return "Gold";
    if (AreColorsEqual(color, LIME)) return "Lime";
    if (AreColorsEqual(color, BEIGE)) return "Beige";
    if (AreColorsEqual(color, SKYBLUE)) return "Sky Blue";
    if (AreColorsEqual(color, VIOLET)) return "Violet";
    
    return "Custom";
}

void Game::HandleMainMenuInput() {
    const std::size_t menuButtonCount = 4;
    
    // Navigate menu options
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        if (m_soundEnabled) PlaySound(m_hoverButtonSound);
        m_selectedMainMenuOption = (m_selectedMainMenuOption == 0) ? 
            menuButtonCount - 1 : m_selectedMainMenuOption - 1;
    }
    
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        if (m_soundEnabled) PlaySound(m_hoverButtonSound);
        m_selectedMainMenuOption = (m_selectedMainMenuOption == menuButtonCount - 1) ? 
            0 : m_selectedMainMenuOption + 1;
    }
    
    // Select option
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (m_soundEnabled) PlaySound(m_openButtonSound);
        
        switch (m_selectedMainMenuOption) {
            case 0: // START GAME
                m_currentGameState = GameState::Playing;
                InitializeEntities();
                break;
            case 1: // CONTROLS
                m_currentGameState = GameState::Controls;
                break;
            case 2: // OPTIONS
                m_currentGameState = GameState::Options;
                break;
            case 3: // EXIT
                if (m_soundEnabled) PlaySound(m_exitNoSound);
                m_currentGameState = GameState::AskExit;
                break;
        }
    }
    
    // Quick exit with ESC
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (m_soundEnabled) PlaySound(m_exitNoSound);
        m_currentGameState = GameState::AskExit;
    }
}

void Game::HandleControlsMenuInput() {
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE) || 
        IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        if (m_soundEnabled) PlaySound(m_backButtonSound);
        m_currentGameState = GameState::MainMenu;
    }
}

void Game::HandleOptionsMenuInput() {
    const std::size_t optionsCount = 4;
    
    // Navigate menu options
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        if (m_soundEnabled) PlaySound(m_hoverButtonSound);
        m_selectedOptionsMenuOption = (m_selectedOptionsMenuOption == 0) ? 
            optionsCount - 1 : m_selectedOptionsMenuOption - 1;
    }
    
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        if (m_soundEnabled) PlaySound(m_hoverButtonSound);
        m_selectedOptionsMenuOption = (m_selectedOptionsMenuOption == optionsCount - 1) ? 
            0 : m_selectedOptionsMenuOption + 1;
    }
    
    // Modify selected option values
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        if (m_soundEnabled) PlaySound(m_openButtonSound);
        
        switch (m_selectedOptionsMenuOption) {
            case 0: // Max Enemies
                if (m_maxEnemies > 1) m_maxEnemies--;
                break;
            case 1: // Background Color
                CycleToPreviousBackgroundColor();
                break;
            case 2: // Music
                m_musicEnabled = !m_musicEnabled;
                break;
            case 3: // Sound Effects
                m_soundEnabled = !m_soundEnabled;
                break;
        }
    }
    
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        if (m_soundEnabled) PlaySound(m_openButtonSound);
        
        switch (m_selectedOptionsMenuOption) {
            case 0: // Max Enemies
                if (m_maxEnemies < 20) m_maxEnemies++;
                break;
            case 1: // Background Color
                CycleToNextBackgroundColor();
                break;
            case 2: // Music
                m_musicEnabled = !m_musicEnabled;
                break;
            case 3: // Sound Effects
                m_soundEnabled = !m_soundEnabled;
                break;
        }
    }
    
    // Go back to main menu
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (m_soundEnabled) PlaySound(m_backButtonSound);
        m_currentGameState = GameState::MainMenu;
    }
}

void Game::HandleGameOverMenuInput() {
    // Navigate menu options
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        if (m_soundEnabled) PlaySound(m_hoverButtonSound);
        m_selectedGameOverMenuOption = (m_selectedGameOverMenuOption == 0) ? 1 : 0;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        if (m_soundEnabled) PlaySound(m_hoverButtonSound);
        m_selectedGameOverMenuOption = (m_selectedGameOverMenuOption == 1) ? 0 : 1;
    }
    
    // Select option
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        if (m_soundEnabled) PlaySound(m_openButtonSound);
        if (m_selectedGameOverMenuOption == 0) { // Play Again
            RestartGame();
        } else { // Main Menu
            if (m_soundEnabled) PlaySound(m_backButtonSound);
            ResetGame();
            m_currentGameState = GameState::MainMenu;
        }
    }
    
    // Quick shortcuts
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (m_soundEnabled) PlaySound(m_backButtonSound);
        ResetGame();
        m_currentGameState = GameState::MainMenu;
    }
}

void Game::HandleExitMenuInput() {
    // Navigate menu options
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        m_selectedExitMenuOption = (m_selectedExitMenuOption == 0) ? 1 : 0;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        m_selectedExitMenuOption = (m_selectedExitMenuOption == 1) ? 0 : 1;
    }
    
    // Select option
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (m_selectedExitMenuOption == 0) { // "Yes!?" - Exit the game
            if (m_soundEnabled) PlaySound(m_exitDisappointingSound);
            m_shouldExit = true;
        } else { // "No!" - Go back to main menu
            m_shouldExit = false;
            if (m_soundEnabled) PlaySound(m_backButtonSound);
            m_currentGameState = GameState::MainMenu;
        }
    }
    
    // ESC key defaults to "Yes!?" behavior
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (m_soundEnabled) PlaySound(m_exitDisappointingSound);
        m_shouldExit = true;
    }
}

void Game::UpdateGame() {
    if (!m_player) return;
    
    m_player->HandleInput(m_deltaTime, m_grounds[0]->GetBounds(),
                         m_explosionManager, m_explosionSound, m_soundEnabled);

    // Enemy scaling and difficulty progression
    static float enemyBuffTimer = 0.0f;
    enemyBuffTimer += m_deltaTime;

    if (enemyBuffTimer >= 5.0f) {
        if (m_enemyScale < MAX_ENTITY_SCALE) {
            m_enemyScale *= 1.1f;
        } else if (m_gameHardness < MAX_GAME_HARDNESS) {
            m_gameHardness *= 1.1f;
        }
        enemyBuffTimer = 0.0f;
    }

    // Apply physics to player
    ApplyGravity(m_player.get());
    HandleGroundCollision(m_player.get());

    // Update explosions
    m_explosionManager.Update(m_deltaTime);

    // Check game over condition early
    if (m_player->GetRadius() <= TEXTURE_RESOLUTION) {
        SetGameOver();
        return;
    }

    // Collect enemies to remove (avoids repeated searches in RemoveEnemy)
    std::vector<size_t> enemiesToRemove;
    enemiesToRemove.reserve(m_enemies.size() / 4); // Reserve some space to avoid allocations

    // Update all enemies and check for removals in a single pass
    for (size_t i = 0; i < m_enemies.size(); ++i) {
        Enemy* enemy = m_enemies[i].get();
        if (!enemy) continue;

        // Check explosion damage
        if (m_explosionManager.CheckExplosionDamage(enemy->GetCenter(), enemy->GetRadius())) {
            m_player->IncrementKillCount();
            enemiesToRemove.push_back(i);
            continue; // Skip physics/collision for dead enemies
        }

        // Apply physics
        ApplyGravity(enemy);
        HandleGroundCollision(enemy);

        // Handle collisions
        HandleEnemyCollision(enemy);
        HandleFinishLineCollision(enemy);
        HandleEnemyUnderMap(enemy);
    }

    // Remove dead enemies in reverse order to maintain valid indices
    for (auto it = enemiesToRemove.rbegin(); it != enemiesToRemove.rend(); ++it) {
        m_enemies.erase(m_enemies.begin() + *it);
    }
}

void Game::ResetGame() {
    // Clear all game objects
    m_player.reset();
    m_enemies.clear();
    m_grounds.clear();
    m_finishLine.reset();
    
    // Reset game variables
    m_enemyScale = START_TEXTURE_SCALE;
    m_enemySpawnTimer = 0.0f;
    m_enemySpawnInterval = 4.0f;
    m_gameHardness = 0.5f;
}

void Game::RestartGame() {
    ResetGame();
    InitializeEntities();
    
    // Reset camera to player position
    if (m_player) {
        m_camera.target.x = m_player->GetX();
        
        const float groundBottom = static_cast<float>(m_currentWindowHeight);
        m_camera.target.y = groundBottom - m_camera.offset.y;
    }
    
    m_currentGameState = GameState::Playing;
}

void Game::SetGameOver() {
    if (m_soundEnabled) PlaySound(m_loseSound);
    m_selectedGameOverMenuOption = 0;
    m_currentGameState = GameState::GameOver;
}

void Game::DrawMainMenu() {
    // Implementation similar to original but with member variables
    DrawRectangle(0, 0, m_currentWindowWidth, m_currentWindowHeight, Fade(BLACK, 0.8f));
    
    const float centerX = m_currentWindowWidth / 2.0f;
    const float centerY = m_currentWindowHeight / 2.0f;
    const float minMargin = 20.0f;
    const float availableWidth = m_currentWindowWidth - (minMargin * 2);
    
    // Calculate responsive font sizes
    int titleFontSize = Clamp(m_currentWindowWidth / 15, 24, 80);
    int subtitleFontSize = Clamp(m_currentWindowWidth / 60, 12, 24);
    
    // Game Title
    const char* title = "PLAY AS GOBO";
    const int titleWidth = MeasureText(title, titleFontSize);
    
    if (titleWidth > availableWidth) {
        titleFontSize = static_cast<int>((titleFontSize * availableWidth) / titleWidth);
    }
    
    // Subtitle
    const char* subtitle = "By Almohtady Bellah";
    int subtitleWidth = MeasureText(subtitle, subtitleFontSize);
    
    if (subtitleWidth > availableWidth) {
        subtitleFontSize = static_cast<int>((subtitleFontSize * availableWidth) / subtitleWidth);
        subtitleWidth = MeasureText(subtitle, subtitleFontSize);
    }
    
    // Calculate button dimensions
    const float buttonHeight = Clamp(static_cast<float>(m_currentWindowHeight) / 15.0f, 35.0f, 60.0f);
    const float buttonSpacing = Clamp(static_cast<float>(m_currentWindowHeight) / 40.0f, 10.0f, 25.0f);
    const float titleSpacing = Clamp(static_cast<float>(m_currentWindowHeight) / 30.0f, 15.0f, 40.0f);
    
    const float totalMenuHeight = titleFontSize + subtitleFontSize + titleSpacing + 
                                 (4 * buttonHeight) + (3 * buttonSpacing);
    
    float menuStartY = centerY - (totalMenuHeight / 2.0f);
    if (menuStartY < minMargin) {
        menuStartY = minMargin;
    }
    if (menuStartY + totalMenuHeight > m_currentWindowHeight - minMargin) {
        menuStartY = m_currentWindowHeight - minMargin - totalMenuHeight;
    }
    
    // Draw title and subtitle
    DrawText(title, static_cast<int>(centerX - titleWidth/2), static_cast<int>(menuStartY), titleFontSize, GOLD);
    DrawText(subtitle, static_cast<int>(centerX - subtitleWidth/2), 
             static_cast<int>(menuStartY + titleFontSize + (titleSpacing / 2)), subtitleFontSize, YELLOW);
    
    // Create and draw buttons
    const float buttonWidth = Clamp(static_cast<float>(m_currentWindowWidth) / 4.0f, 150.0f, 300.0f);
    const float buttonStartY = menuStartY + titleFontSize + subtitleFontSize + titleSpacing;
    
    const std::array<const char*, 4> buttonTexts = {"START GAME", "CONTROLS", "OPTIONS", "EXIT"};
    
    for (std::size_t i = 0; i < buttonTexts.size(); ++i) {
        const Rectangle buttonBounds = {
            centerX - buttonWidth/2, 
            buttonStartY + i * (buttonHeight + buttonSpacing), 
            buttonWidth, 
            buttonHeight
        };
        
        const Color btnColor = (i == m_selectedMainMenuOption) ? LIME : DARKGRAY;
        DrawRectangleRounded(buttonBounds, 0.3f, 0, btnColor);
        
        const int textWidth = MeasureText(buttonTexts[i], static_cast<int>(buttonHeight * 0.75f));
        DrawText(buttonTexts[i],
                 static_cast<int>(buttonBounds.x + (buttonBounds.width - textWidth) / 2),
                 static_cast<int>(buttonBounds.y + (buttonBounds.height - buttonHeight * 0.75f) / 2),
                 static_cast<int>(buttonHeight * 0.75f),
                 WHITE);
    }
}

void Game::DrawControlsMenu() {
    DrawRectangle(0, 0, m_currentWindowWidth, m_currentWindowHeight, Fade(BLACK, 0.8f));
    
    const float centerX = m_currentWindowWidth / 2.0f;
    const float centerY = m_currentWindowHeight / 2.0f;
    const float minMargin = 20.0f;
    const float availableWidth = m_currentWindowWidth - (minMargin * 2);
    
    // Calculate responsive font sizes
    int titleFontSize = Clamp(m_currentWindowWidth / 15, 24, 60);
    int controlTextFontSize = Clamp(m_currentWindowWidth / 25, 16, 28);
    int backFontSize = Clamp(m_currentWindowWidth / 35, 12, 18);
    
    // Title
    const char* title = "Controls";
    const int titleWidth = MeasureText(title, titleFontSize);
    
    if (titleWidth > availableWidth) {
        titleFontSize = static_cast<int>((titleFontSize * availableWidth) / titleWidth);
    }
    
    // Calculate total height and positioning
    const float totalHeight = titleFontSize + (controlTextFontSize * 4) + backFontSize + 80;
    float menuStartY = centerY - (totalHeight / 2.0f);
    
    if (menuStartY < minMargin) {
        menuStartY = minMargin;
    }
    
    // Draw title
    DrawText(title, static_cast<int>(centerX - titleWidth/2), static_cast<int>(menuStartY), titleFontSize, RED);
    
    // Control instructions
    const std::array<const char*, 3> controls = {
        "Movement: Arrow Keys and W,A,S,D",
        "Bomb: Space",
        "End Game: Escape Key"
    };
    
    const float controlStartY = menuStartY + titleFontSize + 40;
    const float lineSpacing = controlTextFontSize + 10;
    
    for (std::size_t i = 0; i < controls.size(); ++i) {
        const int controlWidth = MeasureText(controls[i], controlTextFontSize);
        int displayFontSize = controlTextFontSize;
        
        if (controlWidth > availableWidth) {
            displayFontSize = static_cast<int>((controlTextFontSize * availableWidth) / controlWidth);
        }
        
        const int adjustedControlWidth = MeasureText(controls[i], displayFontSize);
        DrawText(controls[i], 
                static_cast<int>(centerX - adjustedControlWidth/2), 
                static_cast<int>(controlStartY + (i * lineSpacing)), 
                displayFontSize, 
                DARKGREEN);
    }
    
    // Back instruction
    const char* exitText = "Press Escape Key to Main Menu";
    int backTextWidth = MeasureText(exitText, backFontSize);
    
    if (backTextWidth > availableWidth) {
        backFontSize = static_cast<int>((backFontSize * availableWidth) / backTextWidth);
        backTextWidth = MeasureText(exitText, backFontSize);
    }
    
    DrawText(exitText, 
            static_cast<int>(centerX - backTextWidth/2),
            static_cast<int>(controlStartY + (3 * lineSpacing) + 40), 
            backFontSize, 
            GRAY);
}

void Game::DrawOptionsMenu() {
    DrawRectangle(0, 0, m_currentWindowWidth, m_currentWindowHeight, Fade(BLACK, 0.8f));
    
    const float centerX = m_currentWindowWidth / 2.0f;
    const float centerY = m_currentWindowHeight / 2.0f;
    const float minMargin = 20.0f;
    const float availableWidth = m_currentWindowWidth - (minMargin * 2);
    const float availableHeight = m_currentWindowHeight - (minMargin * 2);
    
    // Calculate responsive font sizes
    int titleFontSize = Clamp(static_cast<int>(std::min(m_currentWindowWidth / 15, m_currentWindowHeight / 20)), 18, 60);
    int optionFontSize = Clamp(static_cast<int>(std::min(m_currentWindowWidth / 25, m_currentWindowHeight / 35)), 12, 32);
    int valueFontSize = Clamp(static_cast<int>(std::min(m_currentWindowWidth / 30, m_currentWindowHeight / 40)), 10, 24);
    int instrFontSize = Clamp(static_cast<int>(std::min(m_currentWindowWidth / 40, m_currentWindowHeight / 50)), 10, 20);
    
    // Title
    const char* title = "Options";
    const int titleWidth = MeasureText(title, titleFontSize);
    
    if (titleWidth > availableWidth) {
        titleFontSize = static_cast<int>((titleFontSize * availableWidth) / titleWidth);
    }
    
    // Calculate table dimensions
    const float tableWidth = Clamp(availableWidth * 0.8f, 300.0f, 600.0f);
    const float leftColumnWidth = tableWidth * 0.45f;
    const float rightColumnWidth = tableWidth * 0.55f;
    const float tableStartX = centerX - (tableWidth / 2.0f);
    
    // Calculate button dimensions
    const float buttonSize = Clamp(optionFontSize * 1.2f, 20.0f, 35.0f);
    const int buttonFontSize = Clamp(static_cast<int>(buttonSize * 0.6f), 10, 18);
    
    // Calculate spacing
    float rowSpacing = Clamp(availableHeight / 25.0f, 15.0f, 40.0f);
    const float totalTableHeight = titleFontSize + (4 * std::max(static_cast<float>(optionFontSize), buttonSize)) + 
                                  (5 * rowSpacing) + instrFontSize + 60;
    
    // Adjust row spacing if content is too tall
    if (totalTableHeight > availableHeight) {
        rowSpacing = (availableHeight - titleFontSize - (4 * std::max(static_cast<float>(optionFontSize), buttonSize)) - 
                     instrFontSize - 60) / 5.0f;
        rowSpacing = std::max(rowSpacing, 8.0f);
    }
    
    float menuStartY = centerY - (totalTableHeight / 2.0f);
    if (menuStartY < minMargin) {
        menuStartY = minMargin;
    }
    
    // Draw title
    DrawText(title, static_cast<int>(centerX - titleWidth/2), static_cast<int>(menuStartY), titleFontSize, RED);
    
    float currentY = menuStartY + titleFontSize + rowSpacing;
    
    // Option data
    const std::array<const char*, 4> optionNames = {
        "Max Enemies:",
        "Background Color:",
        "Music:",
        "Sound Effects:"
    };
    
    // Draw table background
    DrawRectangle(static_cast<int>(tableStartX - 10), 
                  static_cast<int>(currentY - 10), 
                  static_cast<int>(tableWidth + 20),
                  static_cast<int>((4 * std::max(static_cast<float>(optionFontSize), buttonSize)) + 
                                  (3 * rowSpacing) + 40), 
                  Fade(DARKGRAY, 0.2f));
    
    for (std::size_t i = 0; i < optionNames.size(); ++i) {
        const Color optionColor = (i == m_selectedOptionsMenuOption) ? LIME : WHITE;
        const Color buttonColor = (i == m_selectedOptionsMenuOption) ? LIME : DARKGRAY;
        
        // Calculate font size to fit in column if needed
        int currentOptionFontSize = optionFontSize;
        const int optionNameWidth = MeasureText(optionNames[i], currentOptionFontSize);
        
        if (optionNameWidth > leftColumnWidth - 10) {
            currentOptionFontSize = static_cast<int>((currentOptionFontSize * (leftColumnWidth - 10)) / optionNameWidth);
        }
        
        // Draw option name (left column)
        DrawText(optionNames[i], 
                static_cast<int>(tableStartX), 
                static_cast<int>(currentY + (buttonSize - currentOptionFontSize) / 2), 
                currentOptionFontSize, 
                optionColor);
        
        // Calculate button layout in right column
        const float rightColumnX = tableStartX + leftColumnWidth;
        float controlSpacing = 5.0f;
        const float totalControlWidth = buttonSize + controlSpacing + 60 + controlSpacing + buttonSize;
        float controlStartX = rightColumnX + (rightColumnWidth - totalControlWidth) / 2.0f;
        
        // Adjust if controls don't fit
        if (totalControlWidth > rightColumnWidth) {
            controlStartX = rightColumnX;
            controlSpacing = 2.0f;
        }
        
        // Draw left arrow button
        const Rectangle leftButtonRect = { controlStartX, currentY, buttonSize, buttonSize };
        DrawRectangleRounded(leftButtonRect, 0.3f, 0, buttonColor);
        DrawText("<", 
                static_cast<int>(controlStartX + (buttonSize - MeasureText("<", buttonFontSize)) / 2), 
                static_cast<int>(currentY + (buttonSize - buttonFontSize) / 2), 
                buttonFontSize, 
                WHITE);
        
        // Draw right arrow button
        const float rightButtonX = controlStartX + buttonSize + controlSpacing + 60 + controlSpacing;
        const Rectangle rightButtonRect = { rightButtonX, currentY, buttonSize, buttonSize };
        DrawRectangleRounded(rightButtonRect, 0.3f, 0, buttonColor);
        DrawText(">", 
                static_cast<int>(rightButtonX + (buttonSize - MeasureText(">", buttonFontSize)) / 2), 
                static_cast<int>(currentY + (buttonSize - buttonFontSize) / 2), 
                buttonFontSize, 
                WHITE);
        
        // Draw current value in the center
        const float valueX = controlStartX + buttonSize + controlSpacing;
        const float valueY = currentY + (buttonSize - valueFontSize) / 2;
        
        DrawOptionValue(i, valueX, valueY, valueFontSize, optionColor, rightColumnX, rightColumnWidth, 
                       currentY, buttonSize);
        
        // Calculate row height
        const float rowHeight = (i == 1) ? std::max(static_cast<float>(optionFontSize), buttonSize) + 15 : 
                               std::max(static_cast<float>(optionFontSize), buttonSize);
        currentY += rowHeight + rowSpacing;
    }
    
    currentY += rowSpacing;
    
    // Draw instructions and exit text
    DrawOptionsInstructions(currentY, centerX, availableWidth, instrFontSize);
}

void Game::DrawGameOverMenu() {
    if (!m_player) return;
    
    DrawRectangle(0, 0, m_currentWindowWidth, m_currentWindowHeight, Fade(BLACK, 0.7f));
    
    const float centerX = m_currentWindowWidth / 2.0f;
    const float centerY = m_currentWindowHeight / 2.0f;
    const float minMargin = 20.0f;
    const float availableWidth = m_currentWindowWidth - (minMargin * 2);
    
    // Calculate responsive font sizes
    int titleFontSize = Clamp(m_currentWindowWidth / 12, 30, 80);
    const int statsFontSize = Clamp(m_currentWindowWidth / 20, 20, 40);
    const int buttonFontSize = Clamp(m_currentWindowWidth / 25, 16, 32);
    
    // Calculate button dimensions
    const float buttonWidth = Clamp(static_cast<float>(m_currentWindowWidth) / 3.0f, 200.0f, 400.0f);
    const float buttonHeight = Clamp(static_cast<float>(m_currentWindowHeight) / 15.0f, 35.0f, 60.0f);
    
    // Title
    const char* title = "GAME OVER";
    const int titleWidth = MeasureText(title, titleFontSize);
    
    if (titleWidth > availableWidth) {
        titleFontSize = static_cast<int>((titleFontSize * availableWidth) / titleWidth);
    }
    
    // Calculate positioning
    const float totalHeight = titleFontSize + statsFontSize + (2 * buttonHeight) + 120;
    float menuStartY = centerY - (totalHeight / 2.0f);
    
    if (menuStartY < minMargin) {
        menuStartY = minMargin;
    }
    
    // Draw title
    DrawText(title, static_cast<int>(centerX - titleWidth/2), static_cast<int>(menuStartY), titleFontSize, RED);
    
    // Player stats
    const std::string playerKillsStr = "Kills: " + std::to_string(m_player->GetKillCount());
    const int playerKillsWidth = MeasureText(playerKillsStr.c_str(), statsFontSize);
    DrawText(playerKillsStr.c_str(), 
            static_cast<int>(centerX - playerKillsWidth/2), 
            static_cast<int>(menuStartY + titleFontSize + 30), 
            statsFontSize, 
            DARKGREEN);
    
    // Create and draw buttons
    const float buttonStartY = menuStartY + titleFontSize + statsFontSize + 70;
    const std::array<const char*, 2> buttonTexts = {"PLAY AGAIN", "MAIN MENU"};
    
    for (std::size_t i = 0; i < buttonTexts.size(); ++i) {
        const Rectangle buttonBounds = {
            centerX - buttonWidth/2, 
            buttonStartY + i * (buttonHeight + 20), 
            buttonWidth, 
            buttonHeight
        };
        
        const Color btnColor = (i == m_selectedGameOverMenuOption) ? LIME : DARKGRAY;
        DrawRectangleRounded(buttonBounds, 0.3f, 0, btnColor);
        
        const int textWidth = MeasureText(buttonTexts[i], buttonFontSize);
        const Color textColor = (i == m_selectedGameOverMenuOption) ? BLACK : WHITE;
        
        DrawText(buttonTexts[i],
                static_cast<int>(buttonBounds.x + (buttonBounds.width - textWidth) / 2),
                static_cast<int>(buttonBounds.y + (buttonBounds.height - buttonFontSize) / 2),
                buttonFontSize,
                textColor);
    }
}

void Game::DrawExitMenu() {
    DrawRectangle(0, 0, m_currentWindowWidth, m_currentWindowHeight, Fade(BLACK, 0.8f));
    
    const float centerX = m_currentWindowWidth / 2.0f;
    const float centerY = m_currentWindowHeight / 2.0f;
    const float minMargin = 20.0f;
    const float availableWidth = m_currentWindowWidth - (minMargin * 2);
    
    // Calculate responsive font sizes
    int titleFontSize = Clamp(m_currentWindowWidth / 12, 30, 80);
    
    // Calculate button dimensions
    const float buttonWidth = Clamp(static_cast<float>(m_currentWindowWidth) / 4.0f, 120.0f, 200.0f);
    const float buttonHeight = Clamp(static_cast<float>(m_currentWindowHeight) / 12.0f, 40.0f, 70.0f);
    
    // Title
    const char* title = "Do you want to exit?";
    const int titleWidth = MeasureText(title, titleFontSize);
    
    if (titleWidth > availableWidth) {
        titleFontSize = static_cast<int>((titleFontSize * availableWidth) / titleWidth);
    }
    
    // Calculate positioning
    const float totalHeight = titleFontSize + (2 * buttonHeight) + 80;
    float menuStartY = centerY - (totalHeight / 2.0f);
    
    if (menuStartY < minMargin) {
        menuStartY = minMargin;
    }
    
    // Draw title
    DrawText(title, static_cast<int>(centerX - titleWidth/2), static_cast<int>(menuStartY), titleFontSize, RED);
    
    // Create and draw exit buttons
    const float buttonStartY = menuStartY + titleFontSize + 40;
    const float buttonSpacing = 30.0f;
    const std::array<const char*, 2> buttonTexts = {"Yes!?", "No!"};
    
    for (std::size_t i = 0; i < buttonTexts.size(); ++i) {
        const Rectangle buttonBounds = {
            centerX - buttonWidth/2, 
            buttonStartY + i * (buttonHeight + buttonSpacing), 
            buttonWidth, 
            buttonHeight
        };
        
        const Color btnColor = (i == m_selectedExitMenuOption) ? LIME : DARKGRAY;
        DrawRectangleRounded(buttonBounds, 0.3f, 0, btnColor);
        
        const int textWidth = MeasureText(buttonTexts[i], static_cast<int>(buttonHeight * 0.75f));
        const Color textColor = (i == m_selectedExitMenuOption) ? BLACK : WHITE;
        
        DrawText(buttonTexts[i],
                static_cast<int>(buttonBounds.x + (buttonBounds.width - textWidth) / 2),
                static_cast<int>(buttonBounds.y + (buttonBounds.height - buttonHeight * 0.75f) / 2),
                static_cast<int>(buttonHeight * 0.75f),
                textColor);
    }
}

// Helper method for drawing option values
void Game::DrawOptionValue(std::size_t optionIndex, float valueX, float valueY, int valueFontSize, 
                          Color optionColor, float rightColumnX, float rightColumnWidth, 
                          float currentY, float buttonSize) {
    switch (optionIndex) {
        case 0: { // Max Enemies
            const std::string maxEnemiesValue = std::to_string(m_maxEnemies);
            const int valueWidth = MeasureText(maxEnemiesValue.c_str(), valueFontSize);
            DrawText(maxEnemiesValue.c_str(), 
                    static_cast<int>(valueX + (60 - valueWidth) / 2), 
                    static_cast<int>(valueY), 
                    valueFontSize, 
                    optionColor);
            break;
        }
        case 1: { // Background Color
            const float previewSize = Clamp(buttonSize * 0.6f, 15.0f, 20.0f);
            const float previewY = currentY + (buttonSize - previewSize) / 2;
            
            DrawRectangle(static_cast<int>(rightColumnX + (rightColumnWidth - previewSize) / 2),
                         static_cast<int>(previewY), 
                         static_cast<int>(previewSize), 
                         static_cast<int>(previewSize), 
                         m_backgroundColor);
            DrawRectangleLines(static_cast<int>(rightColumnX + (rightColumnWidth - previewSize) / 2),
                              static_cast<int>(previewY), 
                              static_cast<int>(previewSize), 
                              static_cast<int>(previewSize), 
                              WHITE);
            
            // Color name below the buttons
            const std::string colorName = GetColorName(m_backgroundColor);
            const int colorNameFontSize = std::min(valueFontSize - 2, 
                                                  static_cast<int>(rightColumnWidth / colorName.length() * 1.2f));
            if (colorNameFontSize > 8) {
                const int colorNameWidth = MeasureText(colorName.c_str(), colorNameFontSize);
                DrawText(colorName.c_str(), 
                        static_cast<int>(rightColumnX + (rightColumnWidth - colorNameWidth) / 2), 
                        static_cast<int>(currentY + buttonSize + 3), 
                        colorNameFontSize, 
                        optionColor);
            }
            break;
        }
        case 2: { // Music
            const char* musicStatus = m_musicEnabled ? "ON" : "OFF";
            const Color musicStatusColor = m_musicEnabled ? GREEN : RED;
            const int statusWidth = MeasureText(musicStatus, valueFontSize);
            DrawText(musicStatus, 
                    static_cast<int>(valueX + (60 - statusWidth) / 2), 
                    static_cast<int>(valueY), 
                    valueFontSize, 
                    musicStatusColor);
            break;
        }
        case 3: { // Sound Effects
            const char* soundStatus = m_soundEnabled ? "ON" : "OFF";
            const Color soundStatusColor = m_soundEnabled ? GREEN : RED;
            const int statusWidth = MeasureText(soundStatus, valueFontSize);
            DrawText(soundStatus, 
                    static_cast<int>(valueX + (60 - statusWidth) / 2), 
                    static_cast<int>(valueY), 
                    valueFontSize, 
                    soundStatusColor);
            break;
        }
    }
}

// Helper method for drawing options instructions
void Game::DrawOptionsInstructions(float currentY, float centerX, float availableWidth, int instrFontSize) {
    // Instructions
    const char* instructions = "Use UP/DOWN to navigate, LEFT/RIGHT to change values";
    int instrWidth = MeasureText(instructions, instrFontSize);
    
    if (instrWidth > availableWidth) {
        instrFontSize = static_cast<int>((instrFontSize * availableWidth) / instrWidth);
        instrWidth = MeasureText(instructions, instrFontSize);
    }
    
    DrawText(instructions, 
            static_cast<int>(centerX - instrWidth/2), 
            static_cast<int>(currentY), 
            instrFontSize, 
            GRAY);
    
    // Back instruction
    const char* exitText = "Press Escape Key to Main Menu";
    int backFontSize = Clamp(static_cast<int>(std::min(m_currentWindowWidth / 35, m_currentWindowHeight / 50)), 10, 18);
    int backTextWidth = MeasureText(exitText, backFontSize);
    
    if (backTextWidth > availableWidth) {
        backFontSize = static_cast<int>((backFontSize * availableWidth) / backTextWidth);
        backTextWidth = MeasureText(exitText, backFontSize);
    }
    
    DrawText(exitText, 
            static_cast<int>(centerX - backTextWidth/2), 
            static_cast<int>(currentY + instrFontSize + 15), 
            backFontSize, 
            GRAY);
}

void Game::Run() {
    while (!WindowShouldClose() && m_currentGameState != GameState::Exit) {
        m_deltaTime = GetFrameTime();
        UpdateMusicStream(m_backgroundMusic);
        SetMusicVolume(m_backgroundMusic, m_musicVolume);
        
        // Update window dimensions
        const int newWidth = GetScreenWidth();
        const int newHeight = GetScreenHeight();
        const bool windowResized = (newWidth != m_currentWindowWidth || newHeight != m_currentWindowHeight);
        
        if (windowResized) {
            m_currentWindowWidth = newWidth;
            m_currentWindowHeight = newHeight;
            m_mapWidth = static_cast<int>(m_currentWindowWidth * 1.5f);
            m_mapHeight = static_cast<int>(m_currentWindowHeight * 1.5f);
            
            // Update camera offset
            m_camera.offset = {m_currentWindowWidth/2.0f, m_currentWindowHeight/2.0f};
            
            // Handle game-specific resize logic
            if (m_currentGameState == GameState::Playing && !m_grounds.empty()) {
                const float groundHeight = GetGroundHeight();
                const float groundY = m_currentWindowHeight - groundHeight;
                const float newGroundX = (m_currentWindowWidth - m_mapWidth) / 2.0f;
                
                m_grounds[0]->SetBounds({newGroundX, groundY, static_cast<float>(m_mapWidth), groundHeight});
                
                if (m_finishLine) {
                    const float finishLineWidth = (m_finishLineTexture.id != 0) ? 
                        m_finishLineTexture.width * FINISH_LINE_WIDTH : 200.0f;
                    const float finishLineHeight = (m_finishLineTexture.id != 0) ? 
                        static_cast<float>(m_finishLineTexture.height) : 50.0f;
                    
                    const float finishLineX = (m_currentWindowWidth / 2.0f) - (finishLineWidth / 2.0f);
                    const float finishLineY = groundY - finishLineHeight;
                    
                    m_finishLine->SetPosition(finishLineX, finishLineY);
                }
            }
        }

        // Handle music
        if (m_musicEnabled && !IsMusicStreamPlaying(m_backgroundMusic)) {
            ResumeMusicStream(m_backgroundMusic);
        } else if (!m_musicEnabled && IsMusicStreamPlaying(m_backgroundMusic)) {
            PauseMusicStream(m_backgroundMusic);
        }
        
        // State-specific updates
        switch (m_currentGameState) {
            case GameState::MainMenu:
                if (m_resetGame) {
                    ResetGame();
                    m_resetGame = false;
                }
                m_musicVolume = Lerp(m_musicVolume, 1.0f, 0.25f);
                HandleMainMenuInput();
                break;
                
            case GameState::Controls:
                HandleControlsMenuInput();
                break;
                
            case GameState::Options:
                HandleOptionsMenuInput();
                break;
                
            case GameState::Playing:
                if (m_player) {
                    if (IsKeyPressed(KEY_ESCAPE)) {
                        if (m_soundEnabled) PlaySound(m_backButtonSound);
                        m_currentGameState = GameState::MainMenu;
                        m_resetGame = true;
                    }
                    
                    m_musicVolume = Lerp(m_musicVolume, 0.25f, 0.25f);
                    
                    // Update camera to follow player
                    const float targetX = m_player->GetX();
                    const float groundBottom = static_cast<float>(m_currentWindowHeight);
                    const float targetY = groundBottom - m_camera.offset.y;
                    
                    // Clamp camera to map boundaries if needed
                    const float halfScreenWidth = m_currentWindowWidth / 2.0f;
                    const float groundLeft = m_grounds[0]->GetX();
                    const float groundRight = m_grounds[0]->GetX() + m_grounds[0]->GetWidth();
                    
                    float clampedTargetX = targetX;
                    if (m_grounds[0]->GetWidth() >= m_currentWindowWidth) {
                        const float minCameraX = groundLeft + halfScreenWidth;
                        const float maxCameraX = groundRight - halfScreenWidth;
                        clampedTargetX = Clamp(targetX, minCameraX, maxCameraX);
                    }
                    
                    m_camera.target.x = Lerp(m_camera.target.x, clampedTargetX, 0.1f);
                    m_camera.target.y = targetY;
                }
                
                SpawnEnemies();
                
                // Enemy AI
                for (auto& enemy : m_enemies) {
                    if (enemy && m_player) {
                        enemy->ExecuteAI(m_deltaTime, static_cast<float>(m_mapWidth), 
                                       m_finishLine->GetX() + m_finishLine->GetWidth()/2, 
                                       *m_player, m_soundEnabled);
                    }
                }
                
                UpdateGame();
                
                if (m_player) {
                    m_player->Update(m_deltaTime);
                    
                    // Keep player from falling below screen
                    if (m_player->GetY() > m_currentWindowHeight) {
                        m_player->SetY(static_cast<float>(m_currentWindowHeight) / 2.0f);
                    }
                }
                
                // Update enemies
                for (auto& enemy : m_enemies) {
                    if (enemy) {
                        enemy->Update(m_deltaTime);
                    }
                }
                break;
                
            case GameState::GameOver:
                m_musicVolume = 0.0f;
                HandleGameOverMenuInput();
                break;
                
            case GameState::AskExit:
                m_musicVolume = 0.0f;
                HandleExitMenuInput();
                if (m_shouldExit && !IsSoundPlaying(m_exitDisappointingSound)) {
                    m_currentGameState = GameState::Exit;
                }
                break;
                
            case GameState::Exit:
                break;
        }
        
        // Rendering
        if (m_currentGameState != GameState::Exit) {
            BeginDrawing();
            ClearBackground(m_backgroundColor);
            
            switch (m_currentGameState) {
                case GameState::MainMenu:
                    DrawMainMenu();
                    break;
                case GameState::Controls:
                    DrawControlsMenu();
                    break;
                case GameState::Options:
                    DrawOptionsMenu();
                    break;
                case GameState::AskExit:
                    DrawExitMenu();
                    break;
                case GameState::Playing:
                case GameState::GameOver:
                    // Game rendering with camera
                    BeginMode2D(m_camera);
                    
                    // Draw world objects
                    for (const auto& ground : m_grounds) {
                        if (ground) ground->Draw();
                    }

                    if (m_player) m_player->Draw(TEXTURE_RESOLUTION, m_currentWindowHeight, m_currentWindowWidth);
                    if (m_finishLine) m_finishLine->Draw();
                    
                    for (const auto& enemy : m_enemies) {
                        if (enemy) {
                            enemy->Draw(TEXTURE_RESOLUTION, m_currentWindowHeight, m_currentWindowWidth);
                        }
                    }

                    m_explosionManager.Draw();
                    
                    EndMode2D();

                    // Draw UI
                    if (m_player) {
                        const std::string playerKills = "Kills: " + std::to_string(m_player->GetKillCount());
                        int killsFontSize = 40;
                        const int killsWidth = MeasureText(playerKills.c_str(), killsFontSize);
                        killsFontSize = (killsWidth > m_currentWindowWidth/3) ? 
                                       (killsFontSize * (m_currentWindowWidth/3) / killsWidth) : killsFontSize;
                        DrawText(playerKills.c_str(), 20, 20, killsFontSize, MAROON);
                    }

                    if (m_currentGameState == GameState::GameOver) {
                        DrawGameOverMenu();
                    }
                    break;
                case GameState::Exit:
                    break;
            }

            EndDrawing();
        }
    }
}

} // namespace PlayAsGobo