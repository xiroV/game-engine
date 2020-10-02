cmake_minimum_required(VERSION 3.14)

project("game-engine")

set(CMAKE_CXX_STANDARD 17)

# This is used to make findSDL2_* available to this script
set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/sdl2")

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
    ${CMAKE_PROJECT_NAME}
)

add_executable(${EXECUTABLE_NAME} ${SOURCES})
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${INCLUDE_DIRS})

if(WIN32)
    # On windows we use the included SDL2 libraries
    add_subdirectory(libs/SDL2_packed)

    target_link_directories(${EXECUTABLE_NAME} PRIVATE ${SDL2_PACKED_LINK_DIRS})
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${SDL2_PACKED_LINK_LIBS})
    target_include_directories(${EXECUTABLE_NAME} PRIVATE ${SDL2_PACKED_INCLUDE_DIRS})

    # This copies over the dll's to the executable dir so we can run it on windows
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SDL2_PACKED_DLL_DIRS} $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>)

else(WIN32)

    # on ubuntu we use the cmake modules to find the libraries and the header include dirs

    find_package(SDL2 REQUIRED)
    target_include_directories(${EXECUTABLE_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${SDL2_LIBRARIES})

    find_package(SDL2_image REQUIRED)
    target_include_directories(${EXECUTABLE_NAME} PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${SDL2_IMAGE_LIBRARIES})

    find_package(SDL2_ttf REQUIRED)
    target_include_directories(${EXECUTABLE_NAME} PRIVATE ${SDL2_TTF_INCLUDE_DIRS})
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${SDL2_TTF_LIBRARIES})

endif(WIN32)

# Assets copy
add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_DIR} "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/assets")
