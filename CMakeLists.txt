cmake_minimum_required(VERSION 3.14)

project(RemoteLED)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include GoogleTest as an external project
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

enable_testing()

# Add remote_led_functions executable
add_executable(remote_led_functions
    test/UT_RemoteLED.cpp
    test/HardwareInterfaceMock.cpp
    src/RemoteLED.cpp)
target_link_libraries(remote_led_functions
                      PRIVATE GTest::gtest_main)

# Define UNIT_TEST_CPP for the include files
target_compile_definitions(remote_led_functions
                           PRIVATE UNIT_TEST_CPP)

# Include directories for GoogleTest
target_include_directories(remote_led_functions
                           PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR}
                           PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

# Include GoogleTest for discover tests
include(GoogleTest)
gtest_discover_tests(remote_led_functions)
