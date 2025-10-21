cmake_minimum_required(VERSION 3.10)
project(ai_mc_pricer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Options
option(BUILD_TESTS "Build tests/demo" ON)

# include dirs
include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
