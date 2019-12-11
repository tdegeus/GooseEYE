
cmake_minimum_required(VERSION 3.0)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(GooseEYE-test)
    find_package(GooseEYE REQUIRED CONFIG)
endif()

option(WARNINGS "Show build warnings" ON)
option(ASSERT "Enable assertions" ON)

find_package(Catch2 REQUIRED)
find_package(xtensor REQUIRED)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE
    Catch2::Catch2
    GooseEYE
    xtensor::optimize
    xtensor::use_xsimd)

if(WARNINGS)
    if(MSVC)
        target_compile_options(main PRIVATE /W4)
    else()
        target_compile_options(main PRIVATE -Wall -Wextra -pedantic -Wno-unknown-pragmas)
    endif()
endif()

if(NOT ASSERT)
    target_compile_definitions(main PRIVATE NDEBUG)
else()
    target_compile_definitions(main PRIVATE XTENSOR_ENABLE_ASSERT GOOSEEYE_ENABLE_ASSERT)
endif()
