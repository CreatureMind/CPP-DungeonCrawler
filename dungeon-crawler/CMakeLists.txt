cmake_minimum_required(VERSION 3.16)
project(DungeonCrawler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CONFIGPARSER_DIR)
    if(DEFINED ENV{CONFIGPARSER_DIR})
        set(CONFIGPARSER_DIR "$ENV{CONFIGPARSER_DIR}")
    else()
        # Fallback default if nothing is provided
        set(CONFIGPARSER_DIR "../config-parser") 
    endif()
endif()

message(STATUS "Using config-parser located at: ${CONFIGPARSER_DIR}")

add_subdirectory("${CONFIGPARSER_DIR}" "${CMAKE_BINARY_DIR}/config-parser")

# ---- raylib 6.0 boiler plate ----
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        6.0
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)
# ----------------------------------

set(GAME_SOURCES
    src/main.cpp
    src/Combat.cpp
    src/Enemy.cpp
    src/Game.cpp
    src/Item.cpp
    src/Map.cpp
    src/Player.cpp
    src/Renderer.cpp
    src/InputHandler.cpp
)

add_executable(dungeon_crawler ${GAME_SOURCES})
target_include_directories(dungeon_crawler PRIVATE include)

target_link_libraries(dungeon_crawler PRIVATE 
    configparser    
    raylib          
)

add_custom_command(TARGET dungeon_crawler POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:dungeon_crawler>/data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:dungeon_crawler>/data"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:dungeon_crawler>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:dungeon_crawler>/assets"
    COMMENT "Copying data and assets to runtime folder"
)