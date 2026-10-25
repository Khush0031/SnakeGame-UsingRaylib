# Retro Snake (raylib + C++)

A small grid-based Snake game written in C++17 with [raylib](https://www.raylib.com/). It features keyboard controls, pause/restart states, sound effects, a saved high score.

## Requirements

- CMake 3.16 or newer.
- A C++17-capable compiler and its platform build tools:
  - Windows: Visual Studio Build Tools or Visual Studio with the C++ workload.
  - macOS: Xcode Command Line Tools.
  - Linux: a C++ compiler and the platform's usual development tools.
- Git, and internet access for the first configure/build. CMake downloads raylib 5.5 through `FetchContent`; subsequent builds can use the downloaded dependency.

## Build and run

Run commands from the project root (the directory containing `CMakeLists.txt`).

### Windows (PowerShell)

```powershell
cmake -S . -B build
cmake --build build --config Release
Set-Location build
.\Release\rayLib-SnakeGame.exe
```

For a single-configuration generator such as MinGW Makefiles, the executable may be at `build\rayLib-SnakeGame.exe` instead of `build\Release\rayLib-SnakeGame.exe`.

### macOS and Linux

```sh
cmake -S . -B build
cmake --build build --config Release
cd build
./rayLib-SnakeGame
```

### What the build commands do

1. `cmake -S . -B build` configures the project in `build`, selects the available compiler/generator, and fetches raylib 5.5 if it is not already available.
2. `cmake --build build --config Release` compiles the game and raylib in Release mode. `--config Release` is relevant to multi-configuration generators such as Visual Studio and is harmless for common single-configuration generators.
3. Run the executable from `build`. This makes the copied `assets` directory available at the relative paths used by the game. The high-score file is also read from and written to the current working directory.

The executable target is named `rayLib-SnakeGame`. To choose a different build directory, replace `build` consistently in the commands.

## Controls

| Key(s) | Action |
| --- | --- |
| Arrow keys or `W` / `A` / `S` / `D` | Steer the snake |
| `Space` | Start from the menu, pause/resume during play, or restart after game over |
| `P` | Pause or resume |
| `R` | Restart after game over |
| `Esc` or the window close button | Exit |

Direction changes are accepted at most once per movement tick. Immediate 180-degree turns are rejected, preventing a direct turn into the snake's body.

## Architecture

The game uses a conventional real-time game loop with a small set of classes. `main.cpp` owns the application lifecycle; `Game` coordinates game state and rules; `Snake` and `Food` own their respective gameplay/rendering behavior. Shared configuration and the state enum live in `Constants.h`.

```text
main.cpp
  └── Game
      ├── Snake
      └── Food ── references Snake for safe food placement

Constants.h
  ├── window/grid dimensions and movement timing
  ├── palette
  └── GameState enum
```

### Application lifecycle — `src/main.cpp`

1. Initializes the raylib window and audio device, then sets the display frame rate to 60 FPS.
2. Constructs one `Game` instance.
3. Repeats input handling, game updates, and drawing until raylib reports that the window should close.
4. Destroys the game (releasing its loaded resources) before closing the audio device and window.

The loop runs once per rendered frame. Snake movement itself is not tied to those frames; it is advanced by the timer in `Game`.

### Game coordination — `src/Game.h` and `src/Game.cpp`

`Game` owns the `Snake` and `Food`, score values, state, loaded sound effects, and movement timer. It coordinates:

- **Input:** Handles menu start, pause/resume, restart, and directional input.
- **State transitions:** `START_MENU` → `RUNNING` → `PAUSED` or `GAME_OVER`; restarting resets the snake, food, score, and speed.
- **Fixed-step updates:** While running, advances the snake when the elapsed time reaches `moveInterval`. The initial interval is 0.15 seconds.
- **Rules:** Checks food collection, wall bounds, and self-collision after each movement.
- **Scoring and difficulty:** Eating adds one point and one segment. Every five points, movement speeds up by reducing the interval by 0.005 seconds, down to a 0.07-second minimum.
- **Presentation:** Draws the background, grid, entities, score/header, and state-specific overlays.
- **High-score persistence:** Reads `highscore.txt` at startup and writes it when the score exceeds the previous best.

### Snake entity — `src/Snake.h` and `src/Snake.cpp`

The snake body is an ordered `std::deque<Vector2>` of grid coordinates: the head is at the front and the tail at the back. A movement step inserts the next head cell at the front and removes the tail. When food has been eaten, `addSegment` keeps the tail for one step, increasing the body length.

The class also resets the starting position, draws the body and direction-aware eyes, and tests whether a grid coordinate intersects the body. The collision helper can skip the head for self-collision checks.

### Food entity — `src/Food.h` and `src/Food.cpp`

Food chooses random grid coordinates until it finds a cell not occupied by the snake. It draws `assets/images/food.png` when the texture can be loaded; otherwise it draws a simple apple using raylib primitives.

### Shared settings — `src/Constants.h`

This header defines grid size, pixel dimensions, margins, initial movement interval, palette, and the `GameState` enum. The window dimensions are computed from the grid: two 75-pixel margins plus 25 cells × 30 pixels, giving a 900 × 900 window.

### Resource ownership

`Game` releases loaded sounds and `Food` releases its texture in their destructors. Copying these resource-owning objects is disabled to avoid duplicate unloads. The audio device and window are initialized and closed in `main.cpp`.

## Project layout

```text
.
├── CMakeLists.txt
├── assets
│   ├── audio
│   │   ├── eat.mp3
│   │   └── wall.mp3
│   └── images
│       └── food.png
├── src
│   ├── Constants.h
│   ├── Food.cpp
│   ├── Food.h
│   ├── Game.cpp
│   ├── Game.h
│   ├── Snake.cpp
│   ├── Snake.h
│   └── main.cpp
└── build                 # Generated by CMake; not source code
```

## Assets and saved data

- Food image: `assets/images/food.png`.
- Eating sound: `assets/audio/eat.mp3`.
- Wall/game-over sound: `assets/audio/wall.mp3`.
- High score: `highscore.txt`, relative to the process's current working directory.

`CMakeLists.txt` copies the `assets` directory into the build directory during configuration. It also copies a root-level `highscore.txt` if one exists at configure time. The game checks a few relative asset locations and continues without optional visuals/audio if loading fails; food has a built-in graphic fallback. For predictable asset and score-file locations, launch the program with the build directory as its working directory.

## CMake configuration notes

- Requires CMake 3.16 and enables C and C++ languages.
- Requires C++17.
- Fetches raylib tag `5.5` with a shallow Git checkout.
- Disables raylib examples and games to avoid building unrelated targets.
- Builds `src/main.cpp`, `src/Game.cpp`, `src/Snake.cpp`, and `src/Food.cpp` into the game executable and links it to raylib.

To reconfigure after changing CMake settings, run `cmake -S . -B build` again. To rebuild without reconfiguring, run `cmake --build build --config Release`.

