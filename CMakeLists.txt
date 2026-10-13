#minimum required version of CMake
cmake_minimum_required(VERSION 3.15)
project(MyRaylibGame LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional: Set the executable output directory to a clean 'bin' folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- FetchContent Setup for Raylib ---
include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        master # Or specify a version like "5.0"
)

# Optional optimization: disable building raylib examples to save compile time
set(BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(BUILD_GAMES OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(raylib)

# --- Define Executable Target ---
add_executable(${PROJECT_NAME} src/main.cpp)

# --- Link Raylib ---
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# --- Copy Assets and Highscore File ---
file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}")

if(EXISTS "${CMAKE_SOURCE_DIR}/highscore.txt")
    file(COPY "${CMAKE_SOURCE_DIR}/highscore.txt" DESTINATION "${CMAKE_BINARY_DIR}")
endif()