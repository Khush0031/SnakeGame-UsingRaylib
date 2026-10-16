#pragma once

#include <raylib.h>
#include <deque>
#include "Constant.h"

// Forward declaration of Snake to avoid circular dependency
class Snake;

/* ==============================================================================
 Food.h - Food Entity Declaration
 ==============================================================================
 Food spawns randomly on the grid.
 When the snake's head reaches the food's coordinate, the snake eats it!
 ==============================================================================

 */

class Food {

public:
    Vector2 position;     // Grid coordinate of the food (e.g. {5, 12})
    Texture2D texture;    // Load food image 
    bool hasTexture;      // True if image loaded successfully, false for fallback

    // Constructor: loads texture and places food on a free cell
    explicit Food(const Snake& snake);

    // Destructor: properly unloads GPU texture memory (RAII principle)
    ~Food();

    // Disable copy constructor and assignment to prevent duplicate texture unloads
    Food(const Food&) = delete;
    Food& operator=(const Food&) = delete;

    // Draw the food (either image or procedural fallback circle)
    void Draw() const;

    // Reposition food to a new random cell that does not overlap the snake
    void Respawn(const Snake& snake);

private:
    // Helper to find a random cell on the grid
    static Vector2 GenerateRandomCell();
};
