#include "Food.h"
#include "Snake.h"
#include <iostream>

/* ==============================================================================

 Food.cpp - Food Entity Implementation

==============================================================================

*/

namespace {
    // Helper to find existing asset path across working directories
    const char* ResolveAssetPath(const char* p1, const char* p2 = nullptr, const char* p3 = nullptr) {
        if (FileExists(p1)) return p1;
        if (p2 && FileExists(p2)) return p2;
        if (p3 && FileExists(p3)) return p3;
        return nullptr;
    }
}

Food::Food(const Snake& snake) : position{ 0, 0 }, texture{}, hasTexture(false) {
    // Try resolving food image asset from standard paths
    const char* path = ResolveAssetPath("assets/images/food.png", "Graphics/food.png", "../assets/images/food.png");
    
    if (path != nullptr) {
        Image image = LoadImage(path);
        if (image.data != nullptr) {
            texture = LoadTextureFromImage(image);
            UnloadImage(image); // Free CPU memory once transferred to GPU
            hasTexture = (texture.id != 0);
        }
    }

    if (!hasTexture) {
        std::cout << "[INFO] Food texture not found or failed to load. Using vector graphic fallback." << std::endl;
    }

    Respawn(snake);
}

Food::~Food() {
    // Clean up GPU texture memory
    if (hasTexture && texture.id != 0) {
        UnloadTexture(texture);
        texture.id = 0;
    }
}

Vector2 Food::GenerateRandomCell() {
    float x = static_cast<float>(GetRandomValue(0, CELL_COUNT - 1));
    float y = static_cast<float>(GetRandomValue(0, CELL_COUNT - 1));
    return Vector2{ x, y };
}

void Food::Respawn(const Snake& snake) {
    Vector2 candidate = GenerateRandomCell();

    // Loop until we find a cell that does NOT collide with the snake's body
    while (snake.CheckCollisionWithPoint(candidate)) {
        candidate = GenerateRandomCell();
    }

    position = candidate;
}

void Food::Draw() const {
    float pixelX = OFFSET + position.x * CELL_SIZE;
    float pixelY = OFFSET + position.y * CELL_SIZE;

    if (hasTexture) {
        // Draw the textured image scaled to fit the grid cell
        Rectangle sourceRect = { 0.0f, 0.0f, static_cast<float>(texture.width), static_cast<float>(texture.height) };
        Rectangle destRect = { pixelX, pixelY, static_cast<float>(CELL_SIZE), static_cast<float>(CELL_SIZE) };
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(texture, sourceRect, destRect, origin, 0.0f, WHITE);
    } else {
        // Fallback: draw a clean circular apple with a stem
        float centerX = pixelX + CELL_SIZE / 2.0f;
        float centerY = pixelY + CELL_SIZE / 2.0f;
        float radius = CELL_SIZE * 0.40f;

        // Apple body
        DrawCircle(static_cast<int>(centerX), static_cast<int>(centerY), radius, COLOR_ACCENT_RED);
        // Small highlight
        DrawCircle(static_cast<int>(centerX - 3), static_cast<int>(centerY - 3), radius * 0.25f, WHITE);
        // Stem
        DrawRectangle(static_cast<int>(centerX - 1), static_cast<int>(centerY - radius - 2), 2, 4, COLOR_PRIMARY_DARK);
    }
}
