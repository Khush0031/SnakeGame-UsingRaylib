#include "raylib.h"
#include <deque>
#include <iostream>
#include <raymath.h>

using namespace std;

Color green = {0, 255, 0, 255};
Color darkGreen = {0, 100, 0, 255};

float cellSize = 30.0f;
float cellCount = 25.0f;

const int screenWidth = cellSize * cellCount;
const int screenHeight = cellSize * cellCount;

double lastUpdateTime = 0.0;
bool eventTriggered(int interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval / 1000.0)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

bool ElementInDeque(const deque<Vector2> &d, const Vector2 &element)
{
    for (const auto &item : d)
    {
        if (Vector2Equals(item, element))
        {
            return true;
        }
    }
    return false;
}

class Food
{
public:
    Vector2 position;
    Texture2D texture;

    Food(const deque<Vector2> &snakeBody)
    {
        Image foodImage = LoadImage("../assets/images/food.png");
        texture = LoadTextureFromImage(foodImage);
        UnloadImage(foodImage);
        position = GenerateRandomPosition(snakeBody);
    }
    ~Food()
    {
        UnloadTexture(texture);
    }
    void Draw()
    {
        DrawTexture(texture, position.x * cellSize, position.y * cellSize, WHITE);
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    }

    Vector2 GenerateRandomPosition(const deque<Vector2> &snakeBody)
    {
        Vector2 randomPosition;
        do
        {
            randomPosition = GenerateRandomCell();
        } while (ElementInDeque(snakeBody, randomPosition));
        return randomPosition;
    }
};

class Snake
{

public:
    Snake() {}

    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};

    Vector2 direction = Vector2{1, 0}; // Initial direction to the right

    bool addSegment = false; // Flag to indicate if a new segment should be added

    void Draw()
    {
        for (const auto &pos : body)
        {
            Rectangle segment = {pos.x * cellSize, pos.y * cellSize, cellSize, cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void updateSnake()
    {
        // Move the snake by adding a new head in the direction of movement
        Vector2 newHead = {body.front().x + direction.x, body.front().y + direction.y};
        body.push_front(newHead);
        if (!addSegment)
        {
            body.pop_back(); // Remove the tail segment to maintain the length
        }
        else
        {
            addSegment = false;
        }
    }
};

class Game
{

public:
    Snake snake;
    Food food = Food(snake.body);

    bool running = true;

    void Draw()
    {
        food.Draw();
        snake.Draw();
    }

    void update()
    {

        if (running)
        {
            snake.updateSnake();
            checkCollision();
            checkCollisionWithEdge();
            checkCollisionWithSelf();
        }
    }

    void checkCollision()
    {
        if (Vector2Equals(snake.body.front(), food.position))
        {
            // Handle food collision (e.g., grow snake, reposition food)

            food.position = food.GenerateRandomPosition(snake.body);
            snake.addSegment = true; // Set the flag to add a new segment
        }
    }

    void checkCollisionWithEdge()
    {
        Vector2 head = snake.body.front();
        if (head.x < 0 || head.x >= cellCount || head.y < 0 || head.y >= cellCount)
        {
           GameOver();
        }
    }

    void checkCollisionWithSelf()
    {
        Vector2 head = snake.body.front();
        if(ElementInDeque(deque<Vector2>(snake.body.begin() + 1, snake.body.end()), head))
        {
            GameOver();
        }
    }

    void GameOver()
    {
        // Reset the game state
        snake.body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        snake.direction = Vector2{1, 0};
        food.position = food.GenerateRandomPosition(snake.body);

        running = false; // Set the running flag to false to indicate game over
    }
};

int main()
{

    InitWindow(screenWidth, screenHeight, "Snake Game - Raylib");
    SetTargetFPS(60);

    Game game;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        if (eventTriggered(100))
        {
            game.update();
        }

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y == 0)
        {
            game.snake.direction = Vector2{0, -1};
        }
        else if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y == 0)
        {
            game.snake.direction = Vector2{0, 1};
        }
        else if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x == 0)
        {
            game.snake.direction = Vector2{-1, 0};
        }
        else if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x == 0)
        {
            game.snake.direction = Vector2{1, 0};
        }else if (IsKeyPressed(KEY_SPACE) && !game.running)
        {
            game.running = true; // Restart the game
        }
        ClearBackground(green);
        game.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
