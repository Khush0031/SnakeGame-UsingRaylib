#pragma once

#include <raylib.h>

/* ==============================================================================
 Constant.h - Game Configuration & Global Settings

 ==============================================================================

 In game development, keeping magic numbers out of your logic makes your code easy to configure, re-scale, and understand.==============================================================================
*/



// --- Grid & Dimension Settings ---
// The playfield is divided into a grid of cells (like graph paper).
constexpr int CELL_SIZE = 30;   // Size of each grid square in pixels (30x30 px)
constexpr int CELL_COUNT = 25;  // Total number of cells across width and height (25x25 grid)
constexpr int OFFSET = 75;      // Margin around the grid for UI (Score, Title, Borders)

// Total Window Width and Height
// Window size = 2 * margin + (cell count * cell size) = 2 * 75 + 25 * 30 = 900 px
constexpr int SCREEN_WIDTH = 2 * OFFSET + CELL_SIZE * CELL_COUNT;
constexpr int SCREEN_HEIGHT = 2 * OFFSET + CELL_SIZE * CELL_COUNT;

// --- Gameplay Timing ---
// Snake tick speed: how many seconds between each step of the snake.
// Lower value = faster snake. 0.15f is standard retro snake speed.
constexpr double INITIAL_MOVE_INTERVAL = 0.15;

// --- Game States ---
// State machine helps cleanly manage what to display and update at any moment.
enum class GameState {
    START_MENU, // Waiting for player to press SPACE / Arrow key to start
    RUNNING,    // Normal active gameplay
    PAUSED,     // Game paused by user
    GAME_OVER   // Snake collided with wall or itself
};

// --- Color Palette (Retro Game Boy Aesthetic) ---
// Raylib Color is a struct: { Red, Green, Blue, Alpha } (each 0 to 255)
const Color COLOR_BACKGROUND    = { 173, 204,  96, 255 }; // Light retro green
const Color COLOR_PRIMARY_DARK  = {  43,  51,  24, 255 }; // Dark olive green
const Color COLOR_GRID_LINE     = { 160, 190,  85, 255 }; // Subtle grid line tint
const Color COLOR_ACCENT_RED    = { 200,  50,  50, 255 }; // Fallback apple red
const Color COLOR_SNAKE_EYE     = { 240, 240, 240, 255 }; // White for snake eyes
const Color COLOR_SNAKE_PUPIL   = {  20,  20,  20, 255 }; // Black for pupils