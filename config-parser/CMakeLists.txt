cmake_minimum_required(VERSION 3.16)
project(ConfigParser CXX)

set(LIB_SOURCES
    src/ConfigManager.cpp
    src/IniParser.cpp
    src/JsonParser.cpp
    src/ParserFactory.cpp
)

add_library(configparser STATIC ${LIB_SOURCES})

target_include_directories(configparser 
    PUBLIC include
    PRIVATE include/config
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tools/configtool.cpp")
    add_executable(configtool tools/configtool.cpp)
    target_link_libraries(configtool PRIVATE configparser)
endif()
