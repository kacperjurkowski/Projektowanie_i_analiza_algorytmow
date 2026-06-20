cmake_minimum_required(VERSION 3.14)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(MojaGra CXX)

set(CMAKE_CXX_STANDARD 17)

# Ta sekcja instruuje CMake, by sam pobrał i skonfigurował Raylib z GitHuba
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.0 # Pobieramy stabilną wersję 5.0
)
FetchContent_MakeAvailable(raylib)

# Tworzenie pliku wykonywalnego z main.cpp
add_executable(${PROJECT_NAME} main.cpp)

# Automatycznie kopiuje folder 'assets' obok gotowego pliku .exe
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Łączenie Twojej gry z biblioteką Raylib
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)