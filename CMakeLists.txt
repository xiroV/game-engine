cmake_minimum_required(VERSION 3.14)

project("game-engine")

set(CMAKE_CXX_STANDARD 17)


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Clang or gcc flags goes here

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -DWITH_TESTS=false")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # MS Visual Studio Compiler flags here

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Wall")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
endif()

# list all of your (non-test) cpp files (or c files) here
set(SOURCES
    src/Animation2D/Animation2D.cpp
    src/Input/Input.cpp
    src/Rendering/Rendering.cpp
    src/Engine.cpp
    src/Main.cpp
)

# list all of your test-related cpp files here
set(TEST_SOURCES
    src/Input/Input_test.cpp
    src/Rendering/Rendering_test.cpp
)

# include paths, this makes it possible to include (via #include) to any subpath of the src folder.
# This means that '#include "Animation2D/Animation2D.hpp"' in Main.cpp would be correctly resolved
# to the file src/Animation2D/Animation2D.hpp.
set(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/src
)

# asset folder
set(ASSET_DIR
    ${PROJECT_SOURCE_DIR}/assets
)

set(EXECUTABLE_NAME
    "engine"
)

set(TEST_EXECUTABLE_NAME
    "engine-test"
)

# creating executable
add_executable(${EXECUTABLE_NAME} ${SOURCES})
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${INCLUDE_DIRS})

# creating test executable
add_executable(${TEST_EXECUTABLE_NAME} ${TEST_SOURCES} ${SOURCES})
target_include_directories(${TEST_EXECUTABLE_NAME} PRIVATE ${INCLUDE_DIRS})

# setting up SDL2 libraries and headers for targets
add_subdirectory(libs/SDL2_packed)
add_sdl2_libs(${EXECUTABLE_NAME})
add_sdl2_libs(${TEST_EXECUTABLE_NAME})

# Assets copy
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_DIR} "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/assets")

# Assets copy test
add_custom_command(TARGET ${TEST_EXECUTABLE_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_DIR} "$<TARGET_FILE_DIR:${TEST_EXECUTABLE_NAME}>/assets")
