
cmake_minimum_required(VERSION 3.0)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(GooseEYE-test)
    find_package(GooseEYE REQUIRED CONFIG)
endif()

option(WARNINGS "Show build warnings" ON)
option(ASSERT "Enable assertions" ON)

find_package(Catch2 REQUIRED)

file(GLOB sources "*.cpp")

add_executable(main ${sources})

target_link_libraries(main PRIVATE
    Catch2::Catch2
    GooseEYE
    xtensor::optimize
    xtensor::use_xsimd)

if(WARNINGS)
    target_link_libraries(main PRIVATE GooseEYE::compiler_warnings)
endif()

if(ASSERT)
    target_link_libraries(main PRIVATE GooseEYE::debug)
endif()
