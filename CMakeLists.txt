############################################################################################

cmake_minimum_required(VERSION 3.28)

############################################################################################

project(task_06_05 VERSION 1.0 LANGUAGES CXX)

############################################################################################

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_STANDARD_REQUIRED True)

############################################################################################

set(Boost_USE_STATIC_LIBS    ON)

set(Boost_USE_STATIC_RUNTIME ON)

############################################################################################

find_package(Boost 1.85 REQUIRED COMPONENTS filesystem system)

############################################################################################

include_directories(${Boost_INCLUDE_DIRS})

############################################################################################

# library ver 1: exports test()

add_library            (test_v1 SHARED 06_05_lib_v1.cpp)

target_compile_options (test_v1 PRIVATE -Wall -Wextra -Wpedantic -O0)

############################################################################################

# library ver 2: exports test() with a different implementation

add_library            (test_v2 SHARED 06_05_lib_v2.cpp)

target_compile_options (test_v2 PRIVATE -Wall -Wextra -Wpedantic -O0)

############################################################################################

add_executable             (06_05 06_05.cpp)

target_compile_options     (06_05 PRIVATE -Wall -Wextra -Wpedantic -O0)

target_link_libraries      (06_05 PRIVATE ${Boost_LIBRARIES} dl)

############################################################################################
