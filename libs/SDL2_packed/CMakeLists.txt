cmake_minimum_required(VERSION 3.14)

project(SDL2_packed)

# This is used to make findSDL2_* available to this script
set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/sdl2")

set(PREFIX ${CMAKE_CURRENT_SOURCE_DIR})

set(SDL2_PLATFORM_TARGET x86_64-w64-mingw32)

set(SDL2_MAIN_INCLUDE_DIR
    ${PREFIX}/SDL2-2.0.12/${SDL2_PLATFORM_TARGET}/include
    ${PREFIX}/SDL2-2.0.12/${SDL2_PLATFORM_TARGET}/include/SDL2
)

set(SDL2_MAIN_LINK_DIR
    ${PREFIX}/SDL2-2.0.12/${SDL2_PLATFORM_TARGET}/lib
)

set(SDL2_MAIN_LINK_LIB
    SDL2
)

set(SDL2_PACKED_INCLUDE_DIRS
    ${SDL2_MAIN_INCLUDE_DIR}
    ${PREFIX}/SDL2_image-2.0.4/${SDL2_PLATFORM_TARGET}/include
    ${PREFIX}/SDL2_ttf-2.0.15/${SDL2_PLATFORM_TARGET}/include
    PARENT_SCOPE
)

set(SDL2_PACKED_LINK_DIRS
    ${SDL2_MAIN_LINK_DIR}
    ${PREFIX}/SDL2_image-2.0.4/${SDL2_PLATFORM_TARGET}/lib
    ${PREFIX}/SDL2_ttf-2.0.15/${SDL2_PLATFORM_TARGET}/lib
    PARENT_SCOPE
)

set(SDL2_PACKED_LINK_LIBS
    ${SDL2_MAIN_LINK_LIB}
    SDL2_image
    SDL2_ttf
    PARENT_SCOPE
)

set(SDL2_PACKED_DLL_DIRS
    ${PREFIX}/SDL2-2.0.9/${SDL2_PLATFORM_TARGET}/bin
    ${PREFIX}/SDL2_image-2.0.4/${SDL2_PLATFORM_TARGET}/bin
    ${PREFIX}/SDL2_ttf-2.0.15/${SDL2_PLATFORM_TARGET}/bin
    PARENT_SCOPE
)

function(add_sdl2_libs TARGET_NAME)
    if(WIN32)
        target_link_directories(${TARGET_NAME} PRIVATE ${SDL2_PACKED_LINK_DIRS})
        target_link_libraries(${TARGET_NAME} PRIVATE ${SDL2_PACKED_LINK_LIBS})
        target_include_directories(${TARGET_NAME} PRIVATE ${SDL2_PACKED_INCLUDE_DIRS})

        # This copies over the dll's to the executable dir so we can run it on windows
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SDL2_PACKED_DLL_DIRS} $<TARGET_FILE_DIR:${TARGET_NAME}>)

        target_link_directories(${TARGET_NAME} PRIVATE ${SDL2_PACKED_LINK_DIRS})
        target_link_libraries(${TARGET_NAME} PRIVATE ${SDL2_PACKED_LINK_LIBS})
        target_include_directories(${TARGET_NAME} PRIVATE ${SDL2_PACKED_INCLUDE_DIRS})

        # This copies over the dll's to the executable dir so we can run it on windows
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SDL2_PACKED_DLL_DIRS} $<TARGET_FILE_DIR:${TARGET_NAME}>)

    else(WIN32)
        # on ubuntu we use the cmake modules to find the libraries and the header include dirs

        find_package(SDL2 REQUIRED)
        target_include_directories(${TARGET_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
        target_link_libraries(${TARGET_NAME} PRIVATE ${SDL2_LIBRARIES})

        find_package(SDL2_image REQUIRED)
        target_include_directories(${TARGET_NAME} PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
        target_link_libraries(${TARGET_NAME} PRIVATE ${SDL2_IMAGE_LIBRARIES})

        find_package(SDL2_ttf REQUIRED)
        target_include_directories(${TARGET_NAME} PRIVATE ${SDL2_TTF_INCLUDE_DIRS})
        target_link_libraries(${TARGET_NAME} PRIVATE ${SDL2_TTF_LIBRARIES})

    endif(WIN32)
endfunction()
