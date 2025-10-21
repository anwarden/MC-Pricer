add_library(pricer
  ../src/option.cpp
  ../src/black_scholes.cpp
  ../src/monte_carlo_pricer.cpp
  ../src/utils.cpp
)

target_include_directories(pricer PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(pricer PRIVATE -Wall -Wextra -Wpedantic)

add_executable(pricer_demo main.cpp)
target_link_libraries(pricer_demo PRIVATE pricer)
