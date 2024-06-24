# Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(RiskGame)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_BUILD_TYPE Debug)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files to the executable
add_executable(risk src/main.cpp src/game.cpp src/i_agent.cpp src/human_agent.cpp src/random_agent.cpp src/planning_agent.cpp src/console.cpp src/map.cpp src/territory.cpp)

# Link the SFML libraries
set(SFML_DIR ${CMAKE_SOURCE_DIR}/SFML-2.6.1/lib/cmake/SFML)
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)
target_link_libraries(risk sfml-graphics sfml-window sfml-system)