cmake_minimum_required(VERSION 3.18)
project(SmartHomeController)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- 1. Load Generated Sources ---
# This file is created by generate_sources.py before CMake runs
include(sources.cmake)

# --- 2. Find Dependencies ---
find_package(nlohmann_json 3.2.0 REQUIRED)
find_library(PAHO_MQTT_LIB paho-mqtt3c REQUIRED)

# --- 3. Create Executable ---
add_executable(controller ${SOURCES})

# --- 4. Include Directories ---
target_include_directories(controller PRIVATE include)

# --- 5. Link Libraries ---
target_link_libraries(controller PRIVATE
    nlohmann_json::nlohmann_json
    ${PAHO_MQTT_LIB}
)

# --- 6. Compiler Options ---
target_compile_options(controller PRIVATE -Wall -Wextra -pedantic)

# --- 7. Testing (Optional) ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test_sources.cmake")
    enable_testing()
    include(test_sources.cmake)
  
    find_package(GTest REQUIRED)
    
    # Force shared CRT
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(tests_runner ${TEST_SOURCES})
    target_include_directories(tests_runner PRIVATE include)
    target_link_libraries(tests_runner PRIVATE GTest::gtest_main)
    add_test(NAME all_tests COMMAND tests_runner)
endif()