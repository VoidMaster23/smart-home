cmake_minimum_required(VERSION 3.18)
project(SmartHomeController LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- 1. Find Qt Dependencies (CRITICAL STEP) ---
find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick)
qt_standard_project_setup()

# --- 2. Load Generated Sources ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/sources.cmake")
    include(sources.cmake)
else()
    message(FATAL_ERROR "sources.cmake not found! Run generate_sources.py first.")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/headers.cmake")
    include(headers.cmake)
else()
    message(FATAL_ERROR "headers.cmake not found! Run generate_sources.py first.")
endif()

# --- 3. Find Backend Dependencies ---
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(PahoMqttCpp REQUIRED)

find_library(PAHO_MQTT_LIB paho-mqtt3a REQUIRED)
find_path(PAHO_MQTT_C_INCLUDE_DIR MQTTAsync.h REQUIRED)

# --- 4. Create Executable (Qt Style) ---
qt_add_executable(controller MANUAL_FINALIZATION ${SOURCES} ${HEADERS})

# --- 5. Define QML Module ---
qt_add_qml_module(controller
    URI "SmartHome"
    VERSION 1.0
    QML_FILES
        src/Main.qml 
)

# --- 6. Include Directories ---
target_include_directories(controller PRIVATE src src/include ${PAHO_MQTT_C_INCLUDE_DIR})

# --- 7. Link Libraries ---
target_link_libraries(controller PRIVATE
    # Qt Libs
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    
    # Your Backend Libs
    nlohmann_json::nlohmann_json
    PahoMqttCpp::paho-mqttpp3
    ${PAHO_MQTT_LIB}
)

# --- 8. Compiler Options ---
target_compile_options(controller PRIVATE -Wall -Wextra -pedantic)

# --- 9. Finalize Qt Target ---
qt_finalize_executable(controller)

# --- 10. Testing (Optional) ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test_sources.cmake")
    enable_testing()
    include(test_sources.cmake)
    
    find_package(GTest REQUIRED)
    
    # Force shared CRT for GTest
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    
    # Note: FetchContent_MakeAvailable is usually used if you use FetchContent, 
    # but here you used find_package(GTest). Assuming find_package works:
    
    add_executable(tests_runner ${TEST_SOURCES})
    target_include_directories(tests_runner PRIVATE include)
    
    target_link_libraries(tests_runner PRIVATE 
        GTest::gtest_main
        Qt6::Core # Tests might need Qt Core if you test logic classes
    )
    
    add_test(NAME all_tests COMMAND tests_runner)
endif()