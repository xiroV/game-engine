 cmake_minimum_required(VERSION 3.12)

project(SDL2_packed)


set(PREFIX ${CMAKE_CURRENT_SOURCE_DIR})

set(SDL2_MODULE_FIND_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2 PARENT_SCOPE)

set(SDL2_MINGW_PLATFORM_TARGET x86_64-w64-mingw32)

set(SDL2_VC_PLATFORM_TARGET x64)

set(SDL2_MAIN_LIB SDL2-2.0.12)
set(SDL2_IMAGE_LIB SDL2_image-2.0.4)
set(SDL2_TTF_LIB SDL2_ttf-2.0.15)

set(SDL2_PACKED_MINGW_INCLUDE_DIRS
    ${PREFIX}/${SDL2_MAIN_LIB}/mingw/${SDL2_MINGW_PLATFORM_TARGET}/include
    ${PREFIX}/${SDL2_MAIN_LIB}/mingw/${SDL2_MINGW_PLATFORM_TARGET}/include/SDL2
    ${PREFIX}/${SDL2_IMAGE_LIB}/mingw/${SDL2_MINGW_PLATFORM_TARGET}/include
    ${PREFIX}/${SDL2_TTF_LIB}/mingw/${SDL2_MINGW_PLATFORM_TARGET}/include
    PARENT_SCOPE
)

set(SDL2_PACKED_MINGW_LINK_LIBS
    ${PREFIX}/${SDL2_MAIN_LIB}/mingw/${SDL2_MINGW_PLATFORM_TARGET}/lib/libSDL2.dll.a
    ${PREFIX}/${SDL2_IMAGE_LIB}/mingw/${SDL2_MINGW_PLATFORM_TARGET}/lib/libSDL2_image.dll.a
    ${PREFIX}/${SDL2_TTF_LIB}/mingw/${SDL2_MINGW_PLATFORM_TARGET}/lib/libSDL2_ttf.dll.a
    PARENT_SCOPE
)

set(SDL2_PACKED_MINGW_DLL_DIRS
    ${PREFIX}/${SDL2_MAIN_LIB}/mingw/${SDL2_MINGW_PLATFORM_TARGET}/bin
    ${PREFIX}/${SDL2_IMAGE_LIB}/mingw/${SDL2_MINGW_PLATFORM_TARGET}/bin
    ${PREFIX}/${SDL2_TTF_LIB}/mingw/${SDL2_MINGW_PLATFORM_TARGET}/bin
    PARENT_SCOPE
)

set(SDL2_PACKED_VC_LINK_LIBS
     ${PREFIX}/${SDL2_MAIN_LIB}/vc/lib/${SDL2_VC_PLATFORM_TARGET}/SDL2.lib
     ${PREFIX}/${SDL2_MAIN_LIB}/vc/lib/${SDL2_VC_PLATFORM_TARGET}/SDL2main.lib
     ${PREFIX}/${SDL2_TTF_LIB}/vc/lib/${SDL2_VC_PLATFORM_TARGET}/SDL2_ttf.lib
     ${PREFIX}/${SDL2_IMAGE_LIB}/vc/lib/${SDL2_VC_PLATFORM_TARGET}/SDL2_image.lib
     PARENT_SCOPE
)

set(SDL2_PACKED_VC_INCLUDE_DIRS
    ${PREFIX}/SDL2-2.0.12/vc/include
    ${PREFIX}/SDL2_ttf-2.0.15/vc/include
    PARENT_SCOPE
)

function(add_sdl2_libs TARGET_NAME)
    if(WIN32)
        if (MINGW)
            target_link_libraries(${TARGET_NAME} PRIVATE ${SDL2_PACKED_MINGW_LINK_LIBS})
            target_include_directories(${TARGET_NAME} PRIVATE ${SDL2_PACKED_MINGW_INCLUDE_DIRS})

            # This copies over the dll's to the executable dir so we can run it on windows
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${SDL2_PACKED_MINGW_DLL_DIRS} $<TARGET_FILE_DIR:${TARGET_NAME}>)
        endif(MINGW)
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
