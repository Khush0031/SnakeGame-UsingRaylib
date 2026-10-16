#include <iostream>
#include <raylib.h>
#include "Constant.h"
#include "Game.h"

/*
==============================================================================
 main.cpp - Game Entry Point
==============================================================================
 This is the starting point of our C++ Raylib game.
 The structure follows the standard Game Loop architecture:

   1. Initialization  -> Create window, initialize audio, load resources
   2. The Game Loop   -> Runs repeatedly at 60 frames per second:
         a. Input Handling   (Handle keyboard/mouse events)
         b. Logic Update     (Move objects, check collisions, update score)
         c. Render / Drawing (Draw background, grid, snake, food, UI)
   3. Cleanup         -> Unload sounds/textures and close the window
 ==============================================================================

*/

int main(int argc, char* argv[]) {
    // ---------------------------------------------------------
    // Step 1: Initialization
    // ---------------------------------------------------------
    std::cout << "========================================" << std::endl;
    std::cout << "  Starting Retro Snake Game (Raylib C++)" << std::endl;
    std::cout << "========================================" << std::endl;

    // Create the graphical application window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Retro Snake Game");

    // Initialize audio device for sound playback
    InitAudioDevice();

    // Set maximum frame rate (60 FPS provides smooth rendering & steady frame pacing)
    SetTargetFPS(60);

    // Initialize our master game controller
    // This creates the snake, spawns initial food, loads sound effects, and reads high scores
    Game game;

    // ---------------------------------------------------------
    // Step 2: The Core Game Loop
    // ---------------------------------------------------------
    // WindowShouldClose() returns true if the user clicks the close 'X' button or presses ESC
    while (!WindowShouldClose()) {
        // [A] Process User Input (Arrow keys, WASD, Space, P, R)
        game.HandleInput();

        // [B] Update Game World (Move snake on tick interval, check collisions)
        game.Update();

        // [C] Render / Draw Frame to Screen
        BeginDrawing();
        
        game.Draw();

        EndDrawing();
    }

    // ---------------------------------------------------------
    // Step 3: Resource De-allocation & Cleanup
    // ---------------------------------------------------------
    // Note: The `game` destructor automatically unloads textures and sounds here (RAII).
    // We then close the audio device and destroy the game window.
    CloseAudioDevice();
    CloseWindow();

    std::cout << "[INFO] Game exited cleanly. Thank you for playing!" << std::endl;
    return 0;
}
