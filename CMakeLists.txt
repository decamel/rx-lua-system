cmake_minimum_required(VERSION 3.22)

cmake_policy(SET CMP0084 NEW)

project(ultron-server VERSION 1.0.0 LANGUAGES CXX)

# C++ Properties
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build unit tests" OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Libraries

# /* -------------------------------------------------------------------------- */
# /*                           SPDLog logging library                           */
# /* -------------------------------------------------------------------------- */
find_package(spdlog CONFIG REQUIRED)
# /* -------------------------------------------------------------------------- */
# /*                              ReactiveX library                             */
# /* -------------------------------------------------------------------------- */
find_package(rxcpp CONFIG REQUIRED)
# /* -------------------------------------------------------------------------- */
# /*                                Lua library                                 */
# /* -------------------------------------------------------------------------- */
find_package(Lua REQUIRED)
# /* -------------------------------------------------------------------------- */
# /*                               Boost library                                */
# /* -------------------------------------------------------------------------- */
find_package(Boost REQUIRED)
# /* -------------------------------------------------------------------------- */
# /*                                Json library                                */
# /* -------------------------------------------------------------------------- */
find_package(nlohmann_json CONFIG REQUIRED)

if(BUILD_TESTS)
    find_package(GTest CONFIG REQUIRED)
endif()

include(cmake/link_helpers.cmake)

# Own libraries
add_subdirectory(core)
add_subdirectory(tcp)
add_subdirectory(adapters)
add_subdirectory(util)

# Executable

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
    Boost::boost
    spdlog::spdlog
    rxcpp
    ${LUA_LIBRARIES}
    Ultron::Core
    Ultron::Tcp
    Ultron::Adapters
    Ultron::Util
)

target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})
