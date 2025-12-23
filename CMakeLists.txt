cmake_minimum_required(VERSION 3.21)
project(Match3Game VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if(ANDROID)
    set(PLATFORM_ANDROID TRUE)
elseif(IOS)
    set(PLATFORM_IOS TRUE)
else()
    set(PLATFORM_DESKTOP TRUE)
endif()

# SDL3 Configuration
# Add user's local install path to search paths
list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/.local")

if(PLATFORM_ANDROID)
    # Android uses SDL3 as a shared library
    find_library(SDL3_LIBRARY SDL3 REQUIRED)
    set(SDL3_LIBRARIES ${SDL3_LIBRARY})
elseif(PLATFORM_IOS)
    # iOS specific SDL3 setup
    find_library(SDL3_LIBRARY SDL3 REQUIRED)
    set(SDL3_LIBRARIES ${SDL3_LIBRARY})
else()
    # Desktop: Try to find SDL3 via find_package or pkg-config
    find_package(SDL3 QUIET)
    if(NOT SDL3_FOUND)
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(SDL3 IMPORTED_TARGET sdl3)
            if(SDL3_FOUND)
                set(SDL3_LIBRARIES PkgConfig::SDL3)
            endif()
        endif()
    else()
        set(SDL3_LIBRARIES SDL3::SDL3)
    endif()

    if(NOT SDL3_FOUND)
        message(FATAL_ERROR "SDL3 not found! Please build from source:\n"
                "  git clone https://github.com/libsdl-org/SDL.git SDL3 --depth 1\n"
                "  cd SDL3 && mkdir build && cd build\n"
                "  cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local\n"
                "  cmake --build . && cmake --install .")
    endif()
endif()

# Source files
set(GAME_SOURCES
    src/main.cpp
    src/Game.cpp
    src/Grid.cpp
    src/Gem.cpp
    src/Renderer.cpp
    src/InputHandler.cpp
)

set(GAME_HEADERS
    src/Game.h
    src/Grid.h
    src/Gem.h
    src/Renderer.h
    src/InputHandler.h
)

# Platform-specific configurations
if(PLATFORM_ANDROID)
    add_library(${PROJECT_NAME} SHARED ${GAME_SOURCES} ${GAME_HEADERS})
    include(android/android.cmake)
elseif(PLATFORM_IOS)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${GAME_SOURCES} ${GAME_HEADERS})
    include(ios/ios.cmake)
else()
    add_executable(${PROJECT_NAME} ${GAME_SOURCES} ${GAME_HEADERS})
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL3_LIBRARIES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Platform-specific settings
if(PLATFORM_ANDROID)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_ANDROID)
elseif(PLATFORM_IOS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_IOS)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_DESKTOP)
endif()

# Enable warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
