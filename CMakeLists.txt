cmake_minimum_required(VERSION 3.10)
project(MCPricer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/Option.cpp
    src/BlackScholes.cpp
    src/MonteCarloEngine.cpp
    src/VolatilityPredictor.cpp
)

# Create library
add_library(mcpricer STATIC ${SOURCES})

# Create main executable
add_executable(pricing_demo examples/pricing_demo.cpp)
target_link_libraries(pricing_demo mcpricer)

# Enable testing
enable_testing()

# Add tests
add_executable(test_option tests/test_option.cpp)
target_link_libraries(test_option mcpricer)
add_test(NAME OptionTest COMMAND test_option)

add_executable(test_blackscholes tests/test_blackscholes.cpp)
target_link_libraries(test_blackscholes mcpricer)
add_test(NAME BlackScholesTest COMMAND test_blackscholes)

add_executable(test_montecarlo tests/test_montecarlo.cpp)
target_link_libraries(test_montecarlo mcpricer)
add_test(NAME MonteCarloTest COMMAND test_montecarlo)

add_executable(test_volatility tests/test_volatility.cpp)
target_link_libraries(test_volatility mcpricer)
add_test(NAME VolatilityTest COMMAND test_volatility)

# Installation
install(TARGETS mcpricer DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
install(TARGETS pricing_demo DESTINATION bin)

# Print configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
