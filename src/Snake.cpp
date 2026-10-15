#include "Snake.h"

/* ==============================================================================
 Snake.cpp - Snake Entity Implementation
 ==============================================================================


*/


Snake::Snake() {
    Reset();
}

void Snake::Reset() {
    // Start with 3 segments near the center-left moving right
    body = {
        Vector2{ 6, 9 }, // Head
        Vector2{ 5, 9 }, // Body
        Vector2{ 4, 9 }  // Tail
    };
    direction = Vector2{ 1, 0 }; // Moving right initially
    addSegment = false;
}

void Snake::Update() {
    // 1. Calculate the new head position by adding the direction vector
    Vector2 newHead = Vector2Add(body.front(), direction);

    // 2. Insert the new head at the beginning of the queue
    body.push_front(newHead);

    // 3. If snake just ate food, leave the tail intact (it grows by 1 segment!)
    //    Otherwise, remove the last segment so total length stays the same.
    if (addSegment) {
        addSegment = false;
    } else {
        body.pop_back();
    }
}

void Snake::Draw() const {
    if (body.empty()) return;

    // Draw each body segment as a rounded rectangle
    for (size_t i = 0; i < body.size(); i++) {
        float pixelX = OFFSET + body[i].x * CELL_SIZE;
        float pixelY = OFFSET + body[i].y * CELL_SIZE;

        Rectangle segmentRect = {
            pixelX,
            pixelY,
            static_cast<float>(CELL_SIZE),
            static_cast<float>(CELL_SIZE)
        };

        // Draw rounded rectangle: roundness = 0.45, segments = 6
        DrawRectangleRounded(segmentRect, 0.45f, 6, COLOR_PRIMARY_DARK);
    }

    // --- Draw Eyes on the Head ---
    // Having eyes adds personality and shows the player which way the snake is facing!
    Vector2 head = body.front();
    float headPixelX = OFFSET + head.x * CELL_SIZE;
    float headPixelY = OFFSET + head.y * CELL_SIZE;

    // Eye offsets relative to head cell depending on direction
    Vector2 eye1 = { headPixelX, headPixelY };
    Vector2 eye2 = { headPixelX, headPixelY };
    float eyeRadius = 3.0f;
    float pupilRadius = 1.5f;

    if (direction.x == 1) { // Facing Right
        eye1 = { headPixelX + CELL_SIZE * 0.75f, headPixelY + CELL_SIZE * 0.28f };
        eye2 = { headPixelX + CELL_SIZE * 0.75f, headPixelY + CELL_SIZE * 0.72f };
    } else if (direction.x == -1) { // Facing Left
        eye1 = { headPixelX + CELL_SIZE * 0.25f, headPixelY + CELL_SIZE * 0.28f };
        eye2 = { headPixelX + CELL_SIZE * 0.25f, headPixelY + CELL_SIZE * 0.72f };
    } else if (direction.y == -1) { // Facing Up
        eye1 = { headPixelX + CELL_SIZE * 0.28f, headPixelY + CELL_SIZE * 0.25f };
        eye2 = { headPixelX + CELL_SIZE * 0.72f, headPixelY + CELL_SIZE * 0.25f };
    } else { // Facing Down
        eye1 = { headPixelX + CELL_SIZE * 0.28f, headPixelY + CELL_SIZE * 0.75f };
        eye2 = { headPixelX + CELL_SIZE * 0.72f, headPixelY + CELL_SIZE * 0.75f };
    }

    // Draw eye whites
    DrawCircleV(eye1, eyeRadius, COLOR_SNAKE_EYE);
    DrawCircleV(eye2, eyeRadius, COLOR_SNAKE_EYE);

    // Draw eye pupils
    DrawCircleV(eye1, pupilRadius, COLOR_SNAKE_PUPIL);
    DrawCircleV(eye2, pupilRadius, COLOR_SNAKE_PUPIL);
}

bool Snake::CheckCollisionWithPoint(Vector2 point, bool ignoreHead) const {
    size_t startIndex = ignoreHead ? 1 : 0;
    for (size_t i = startIndex; i < body.size(); i++) {
        if (Vector2Equals(body[i], point)) {
            return true;
        }
    }
    return false;
}
