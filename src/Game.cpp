#include "Game.h"
#include <iostream>
#include <fstream>

/*
==============================================================================
 Game.cpp - Game Engine / Coordinator Implementation
==============================================================================

*/

namespace {
    // Helper to find existing sound assets
    const char* ResolveAudioPath(const char* p1, const char* p2 = nullptr, const char* p3 = nullptr) {
        if (FileExists(p1)) return p1;
        if (p2 && FileExists(p2)) return p2;
        if (p3 && FileExists(p3)) return p3;
        return nullptr;
    }
}

Game::Game() 
    : snake(),
      food(snake),
      state(GameState::START_MENU),
      score(0),
      highScore(0),
      eatSound{},
      wallSound{},
      hasEatSound(false),
      hasWallSound(false),
      lastUpdateTime(0.0),
      moveInterval(INITIAL_MOVE_INTERVAL),
      directionChangedThisTick(false)
{
    // Load high score from disk if available
    LoadHighScore();

    // Try loading audio files
    const char* eatPath = ResolveAudioPath("assets/audio/eat.mp3", "Sounds/eat.mp3", "../assets/audio/eat.mp3");
    if (eatPath != nullptr) {
        eatSound = LoadSound(eatPath);
        hasEatSound = (eatSound.frameCount > 0);
    }

    const char* wallPath = ResolveAudioPath("assets/audio/wall.mp3", "Sounds/wall.mp3", "../assets/audio/wall.mp3");
    if (wallPath != nullptr) {
        wallSound = LoadSound(wallPath);
        hasWallSound = (wallSound.frameCount > 0);
    }
}

Game::~Game() {
    // Release loaded sound effects
    if (hasEatSound && eatSound.frameCount > 0) {
        UnloadSound(eatSound);
    }
    if (hasWallSound && wallSound.frameCount > 0) {
        UnloadSound(wallSound);
    }
}

void Game::LoadHighScore() {
    std::ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
}

void Game::SaveHighScore() {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

bool Game::IsTickReady() {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= moveInterval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

void Game::Start() {
    state = GameState::RUNNING;
    lastUpdateTime = GetTime();
}

void Game::TogglePause() {
    if (state == GameState::RUNNING) {
        state = GameState::PAUSED;
    } else if (state == GameState::PAUSED) {
        state = GameState::RUNNING;
        lastUpdateTime = GetTime(); // Reset timer to prevent instant tick jump on unpause
    }
}

void Game::Restart() {
    snake.Reset();
    food.Respawn(snake);
    score = 0;
    moveInterval = INITIAL_MOVE_INTERVAL;
    directionChangedThisTick = false;
    state = GameState::RUNNING;
    lastUpdateTime = GetTime();
}

void Game::GameOver() {
    state = GameState::GAME_OVER;
    if (hasWallSound) {
        PlaySound(wallSound);
    }
    if (score > highScore) {
        highScore = score;
        SaveHighScore();
    }
}

void Game::HandleInput() {
    // Pause toggle
    if (IsKeyPressed(KEY_P)) {
        TogglePause();
        return;
    }

    // Handle Start & Restart via Spacebar
    if (IsKeyPressed(KEY_SPACE)) {
        if (state == GameState::START_MENU) {
            Start();
            return;
        } else if (state == GameState::GAME_OVER) {
            Restart();
            return;
        } else if (state == GameState::RUNNING || state == GameState::PAUSED) {
            TogglePause();
            return;
        }
    }

    if (state == GameState::GAME_OVER) {
        if (IsKeyPressed(KEY_R)) {
            Restart();
            return;
        }
    }

    // Direction controls (Arrow keys or WASD)
    // Anti-suicide guard: Snake cannot immediately reverse 180 degrees into its own body.
    // Also directionChangedThisTick prevents rapid double-clicks from turning twice before the tick.
    if (!directionChangedThisTick && (state == GameState::RUNNING || state == GameState::START_MENU)) {
        Vector2 currentDir = snake.direction;
        Vector2 targetDir = currentDir;
        bool inputRegistered = false;

        if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && currentDir.y != 1) {
            targetDir = { 0, -1 };
            inputRegistered = true;
        } else if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && currentDir.y != -1) {
            targetDir = { 0, 1 };
            inputRegistered = true;
        } else if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && currentDir.x != 1) {
            targetDir = { -1, 0 };
            inputRegistered = true;
        } else if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && currentDir.x != -1) {
            targetDir = { 1, 0 };
            inputRegistered = true;
        }

        if (inputRegistered) {
            snake.direction = targetDir;
            directionChangedThisTick = true;

            // If player was in START_MENU, pressing a direction key also starts the game
            if (state == GameState::START_MENU) {
                Start();
            }
        }
    }
}

void Game::Update() {
    if (state != GameState::RUNNING) {
        return;
    }

    // Advance snake at discrete tick intervals (e.g. every 0.15 seconds)
    if (IsTickReady()) {
        directionChangedThisTick = false; // Allow next directional input
        snake.Update();
        CheckCollisionWithFood();
        CheckCollisionWithEdges();
        CheckCollisionWithTail();
    }
}

void Game::CheckCollisionWithFood() {
    if (Vector2Equals(snake.body.front(), food.position)) {
        food.Respawn(snake);
        snake.addSegment = true;
        score++;

        if (hasEatSound) {
            PlaySound(eatSound);
        }

        if (score > highScore) {
            highScore = score;
            SaveHighScore();
        }

        // Slight progressive speed increase every 5 points (min 0.07s interval)
        if (score % 5 == 0 && moveInterval > 0.07) {
            moveInterval -= 0.005;
        }
    }
}

void Game::CheckCollisionWithEdges() {
    Vector2 head = snake.body.front();
    if (head.x < 0 || head.x >= CELL_COUNT || head.y < 0 || head.y >= CELL_COUNT) {
        GameOver();
    }
}

void Game::CheckCollisionWithTail() {
    Vector2 head = snake.body.front();
    // Check if head collides with any subsequent segment of the body
    if (snake.CheckCollisionWithPoint(head, /*ignoreHead=*/true)) {
        GameOver();
    }
}

void Game::DrawGrid() const {
    // Draw subtle grid dots or checkered cells for clean retro aesthetics
    for (int x = 0; x < CELL_COUNT; x++) {
        for (int y = 0; y < CELL_COUNT; y++) {
            float px = OFFSET + x * CELL_SIZE;
            float py = OFFSET + y * CELL_SIZE;
            
            // Draw subtle corner dot for each cell
            DrawCircle(static_cast<int>(px + CELL_SIZE / 2), static_cast<int>(py + CELL_SIZE / 2), 1.0f, COLOR_GRID_LINE);
        }
    }
}

void Game::DrawHeader() const {
    // Game Title
    DrawText("RETRO SNAKE", OFFSET, 25, 32, COLOR_PRIMARY_DARK);

    // High score in upper right
    const char* hiText = TextFormat("BEST: %03i", highScore);
    int hiWidth = MeasureText(hiText, 24);
    DrawText(hiText, OFFSET + CELL_SIZE * CELL_COUNT - hiWidth, 32, 24, COLOR_PRIMARY_DARK);
}

void Game::DrawFooter() const {
    // Current Score in lower left
    const char* scoreText = TextFormat("SCORE: %03i", score);
    DrawText(scoreText, OFFSET, OFFSET + CELL_SIZE * CELL_COUNT + 20, 28, COLOR_PRIMARY_DARK);

    // Controls reminder in lower right
    const char* controlsText = "[WASD / ARROWS] Move   [P] Pause";
    int ctrlWidth = MeasureText(controlsText, 18);
    DrawText(controlsText, OFFSET + CELL_SIZE * CELL_COUNT - ctrlWidth, OFFSET + CELL_SIZE * CELL_COUNT + 28, 18, COLOR_PRIMARY_DARK);
}

void Game::DrawOverlayMessage(const char* title, const char* subtitle) const {
    // Translucent dark backdrop
    float gridPixelSize = static_cast<float>(CELL_SIZE * CELL_COUNT);
    Rectangle playfield = { (float)OFFSET, (float)OFFSET, gridPixelSize, gridPixelSize };
    DrawRectangleRec(playfield, ColorAlpha(COLOR_BACKGROUND, 0.85f));

    // Centered Title Box
    int titleFontSize = 36;
    int titleWidth = MeasureText(title, titleFontSize);
    float centerX = OFFSET + gridPixelSize / 2.0f;
    float centerY = OFFSET + gridPixelSize / 2.0f;

    DrawText(title, static_cast<int>(centerX - titleWidth / 2.0f), static_cast<int>(centerY - 35), titleFontSize, COLOR_PRIMARY_DARK);

    if (subtitle != nullptr) {
        int subFontSize = 20;
        int subWidth = MeasureText(subtitle, subFontSize);
        DrawText(subtitle, static_cast<int>(centerX - subWidth / 2.0f), static_cast<int>(centerY + 15), subFontSize, COLOR_PRIMARY_DARK);
    }
}

void Game::Draw() const {
    // 1. Clear background
    ClearBackground(COLOR_BACKGROUND);

    // 2. Draw outer boundary frame
    Rectangle borderRect = {
        static_cast<float>(OFFSET - 4),
        static_cast<float>(OFFSET - 4),
        static_cast<float>(CELL_SIZE * CELL_COUNT + 8),
        static_cast<float>(CELL_SIZE * CELL_COUNT + 8)
    };
    DrawRectangleLinesEx(borderRect, 4, COLOR_PRIMARY_DARK);

    // 3. Draw grid dots
    DrawGrid();

    // 4. Draw food & snake
    food.Draw();
    snake.Draw();

    // 5. Draw UI headers & footers
    DrawHeader();
    DrawFooter();

    // 6. Draw state overlay banners if needed
    if (state == GameState::START_MENU) {
        DrawOverlayMessage("PRESS SPACE TO START", "Use Arrow Keys or WASD to control snake");
    } else if (state == GameState::PAUSED) {
        DrawOverlayMessage("GAME PAUSED", "Press [P] or [SPACE] to resume");
    } else if (state == GameState::GAME_OVER) {
        DrawOverlayMessage("GAME OVER!", "Press [SPACE] or [R] to Play Again");
    }
}
