#pragma once

#include <deque>
#include <raylib.h>
#include <raymath.h>
#include "Constant.h"

/* ==============================================================================
 Snake.h - Snake Entity Declaration
==============================================================================
 The Snake is the player-controlled character.
 It consists of an ordered sequence of grid coordinates (segments).
==============================================================================

*/

class Snake {
public:
    // We use std::deque (double-ended queue) because:
    // 1. Moving forward adds a new head at the front: push_front() -> O(1)
    // 2. Trailing tail is removed from the back:       pop_back()   -> O(1)
    std::deque<Vector2> body;

    // Movement direction on the grid:
    // Right = {1, 0}, Left = {-1, 0}, Up = {0, -1}, Down = {0, 1}
    Vector2 direction;

    // Flag set to true when snake eats food so it grows on the next tick
    bool addSegment;

    // Constructor: sets up initial 3-segment snake
    Snake();

    // Draw snake body segments and head features (eyes)
    void Draw() const;

    // Advance snake by 1 cell in current direction
    void Update();

    // Reset snake back to initial 3 segments and starting position
    void Reset();

    // Check if a given coordinate is occupied by any part of the snake body
    bool CheckCollisionWithPoint(Vector2 point, bool ignoreHead = false) const;
};
