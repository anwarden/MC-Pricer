cmake_minimum_required(VERSION 3.12)
project(MC-Pricer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set compiler flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src/cpp/include)

# Source files
set(SOURCES
    src/cpp/src/Option.cpp
    src/cpp/src/EuropeanOption.cpp
    src/cpp/src/MonteCarloEngine.cpp
    src/cpp/src/VolatilityInterface.cpp
)

# Create library
add_library(mcpricer_lib STATIC ${SOURCES})

# Main executable
add_executable(mcpricer src/cpp/src/main.cpp)
target_link_libraries(mcpricer mcpricer_lib)

# Installation
install(TARGETS mcpricer DESTINATION bin)
install(TARGETS mcpricer_lib DESTINATION lib)
install(DIRECTORY src/cpp/include/ DESTINATION include/mcpricer)
