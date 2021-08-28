
cmake_minimum_required(VERSION 3.0)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(GooseEYE-test)
    find_package(GooseEYE REQUIRED CONFIG)
endif()

set(ASSERT ON)
set(DEBUG ON)
option(SIMD "Enable xsimd" ON)
option(WARNINGS "Enable warnings" ON)

find_package(xtensor REQUIRED)
find_package(Catch2 REQUIRED)

if(SIMD)
    find_package(xsimd REQUIRED)
endif()

file(GLOB sources "*.cpp")

add_executable(main ${sources})

target_link_libraries(main PRIVATE
    Catch2::Catch2
    GooseEYE
    xtensor::optimize)

if(SIMD)
    target_link_libraries(main INTERFACE xtensor::use_xsimd)
endif()

if(WARNINGS)
    target_link_libraries(main PRIVATE GooseEYE::compiler_warnings)
endif()

if(ASSERT)
    target_link_libraries(main PRIVATE GooseEYE::assert)
endif()

if(DEBUG)
    target_link_libraries(main PRIVATE GooseEYE::debug)
endif()
