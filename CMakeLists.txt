cmake_minimum_required(VERSION 3.10)
project(EnchantedClimber VERSION 1.0.0 LANGUAGES CXX)

# ─────────────────────────────────────────────
#  C++ Standard
# ─────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ─────────────────────────────────────────────
#  Compiler Options
# ─────────────────────────────────────────────
# Handled per-target to avoid global conflicts.

# ═══════════════════════════════════════════════════════════════
#  External library layout:
#
#  SDL3/
#    include/SDL3/SDL.h              → #include <SDL3/SDL.h>
#    lib/x64/SDL3.lib + SDL3.dll
#
#  SDL3_image/
#    include/SDL3_image/SDL_image.h  → #include <SDL3_image/SDL_image.h>
#    lib/x64/SDL3_image.lib + .dll
#
#  SDL3_ttf/
#    include/SDL3_ttf/SDL_ttf.h      → #include <SDL3_ttf/SDL_ttf.h>
#    lib/x64/SDL3_ttf.lib + .dll
#
#  SDL3_mixer/                       ← NOW SDL3 (replaced SDL2_mixer)
#    include/SDL3_mixer/SDL_mixer.h  → #include <SDL3_mixer/SDL_mixer.h>
#    lib/x64/SDL3_mixer.lib + .dll
# ═══════════════════════════════════════════════════════════════

set(SDL3_DIR        "${CMAKE_SOURCE_DIR}/external/SDL3")
set(SDL3_IMAGE_DIR  "${CMAKE_SOURCE_DIR}/external/SDL3_image")
set(SDL3_TTF_DIR    "${CMAKE_SOURCE_DIR}/external/SDL3_ttf")
set(SDL3_MIXER_DIR  "${CMAKE_SOURCE_DIR}/external/SDL3_mixer")

# x64 lib paths
set(SDL3_LIB        "${SDL3_DIR}/lib/x64")
set(SDL3_IMAGE_LIB  "${SDL3_IMAGE_DIR}/lib/x64")
set(SDL3_TTF_LIB    "${SDL3_TTF_DIR}/lib/x64")
set(SDL3_MIXER_LIB  "${SDL3_MIXER_DIR}/lib/x64")

# ─────────────────────────────────────────────
#  Collect all source files
# ─────────────────────────────────────────────
file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

# ─────────────────────────────────────────────
#  Executable
# ─────────────────────────────────────────────
add_executable(EnchantedClimber ${SOURCES})

# ─────────────────────────────────────────────
#  Compiler flags — build-type aware
# ─────────────────────────────────────────────
if(MSVC)
    target_compile_options(EnchantedClimber PRIVATE 
        /W4 
        $<$<CONFIG:Release>:/O2>
    )
else()
    target_compile_options(EnchantedClimber PRIVATE 
        -Wall -Wextra 
        $<$<CONFIG:Release>:-O2>
    )
endif()

# ─────────────────────────────────────────────
#  Include directories
#  Every library now follows the SDL3 subfolder
#  convention: #include <SDL3_mixer/SDL_mixer.h>
# ─────────────────────────────────────────────
target_include_directories(EnchantedClimber PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${SDL3_DIR}/include"           # → <SDL3/SDL.h>
    "${SDL3_IMAGE_DIR}/include"     # → <SDL3_image/SDL_image.h>
    "${SDL3_TTF_DIR}/include"       # → <SDL3_ttf/SDL_ttf.h>
    "${SDL3_MIXER_DIR}/include"     # → <SDL3_mixer/SDL_mixer.h>
)

# ─────────────────────────────────────────────
#  Link libraries
# ─────────────────────────────────────────────
target_link_libraries(EnchantedClimber PRIVATE
    "${SDL3_LIB}/SDL3.lib"
    "${SDL3_IMAGE_LIB}/SDL3_image.lib"
    "${SDL3_TTF_LIB}/SDL3_ttf.lib"
    "${SDL3_MIXER_LIB}/SDL3_mixer.lib"
)

# ─────────────────────────────────────────────
#  Copy DLLs next to executable after build
# ─────────────────────────────────────────────
set(ALL_DLLS
    "${SDL3_LIB}/SDL3.dll"
    "${SDL3_IMAGE_LIB}/SDL3_image.dll"
    "${SDL3_TTF_LIB}/SDL3_ttf.dll"
    "${SDL3_MIXER_LIB}/SDL3_mixer.dll"
)
foreach(DLL ${ALL_DLLS})
    add_custom_command(TARGET EnchantedClimber POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${DLL}" $<TARGET_FILE_DIR:EnchantedClimber>
    )
endforeach()

# ─────────────────────────────────────────────
#  Copy assets and data folders next to executable
# ─────────────────────────────────────────────
add_custom_command(TARGET EnchantedClimber POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:EnchantedClimber>/assets"
)
add_custom_command(TARGET EnchantedClimber POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/data"
    "$<TARGET_FILE_DIR:EnchantedClimber>/data"
)

message(STATUS "----------------------------------------")
message(STATUS "  EnchantedClimber configured!")
message(STATUS "  SDL3        : ${SDL3_LIB}/SDL3.lib")
message(STATUS "  SDL3_image  : ${SDL3_IMAGE_LIB}/SDL3_image.lib")
message(STATUS "  SDL3_ttf    : ${SDL3_TTF_LIB}/SDL3_ttf.lib")
message(STATUS "  SDL3_mixer  : ${SDL3_MIXER_LIB}/SDL3_mixer.lib")
message(STATUS "----------------------------------------")