cmake_minimum_required(VERSION 3.16)
project(Retro-Snake-Game LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable raylib's internal examples and tests to speed up building
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 1. Fetch raylib via Git Repository and Tag
include(FetchContent)

FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG        6.0  # Use the latest stable release of raylib
  GIT_SHALLOW    TRUE  # ONLY downloads the specific tag without full project history (Faster & avoids git bugs)
)

FetchContent_MakeAvailable(raylib)

# 2. Create your executable target
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Game.cpp
    src/Snake.cpp
    src/Food.cpp
)

# 3. Link against raylib 
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# 4. Copy assets and highscore files to the build folder
file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}")

if(EXISTS "${CMAKE_SOURCE_DIR}/highscore.txt")
    file(COPY "${CMAKE_SOURCE_DIR}/highscore.txt" DESTINATION "${CMAKE_BINARY_DIR}")
endif()
