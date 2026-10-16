#pragma once

#include <raylib.h>
#include <string>
#include "Constant.h"
#include "Snake.h"
#include "Food.h"

/* ==============================================================================
 Game.h - Game Engine / Coordinator Declaration
 ==============================================================================
 The Game class manages all entities, states, collision rules, scoring,
 audio effects, and user input.
 ==============================================================================

*/

class Game {
public:
    Snake snake;
    Food food;
    GameState state;

    int score;
    int highScore;

    // Audio resources
    Sound eatSound;
    Sound wallSound;
    bool hasEatSound;
    bool hasWallSound;

    // Movement timer and input buffering
    double lastUpdateTime;
    double moveInterval;
    bool directionChangedThisTick; // Prevents fast double-taps causing instant suicide

    Game();
    ~Game();

    // Disable copy to safeguard audio/texture handles
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // Main lifecycle methods called from the main loop
    void HandleInput();
    void Update();
    void Draw() const;

    // State transitions
    void Start();
    void TogglePause();
    void GameOver();
    void Restart();

private:
    // Helper checks
    bool IsTickReady();
    void CheckCollisionWithFood();
    void CheckCollisionWithEdges();
    void CheckCollisionWithTail();

    // High Score Persistence
    void LoadHighScore();
    void SaveHighScore();

    // UI Drawing Helpers
    void DrawHeader() const;
    void DrawFooter() const;
    void DrawGrid() const;
    void DrawOverlayMessage(const char* title, const char* subtitle) const;
};
