
cmake_minimum_required(VERSION 3.0)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(GooseEYE-examples)
    find_package(GooseEYE REQUIRED CONFIG)
    set(GOOSEEYE_INCLUDE_DIR ${GooseEYE_INCLUDE_DIRS})
endif()

option(WARNINGS "Show build warnings" ON)
option(ASSERT "Enable assertions" ON)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Catch2 REQUIRED)
find_package(xtensor REQUIRED)
find_package(xtl REQUIRED)

if(NOT ASSERT)
    add_definitions(-DNDEBUG)
else()
    add_definitions(-DXTENSOR_ENABLE_ASSERT=ON)
endif()

file(GLOB APP_SOURCES *.cpp)
foreach(testsourcefile ${APP_SOURCES})
    string(REPLACE ".cpp" "" testname ${testsourcefile})
    get_filename_component(testname ${testname} NAME)
    add_executable(${testname} ${testsourcefile})
    target_link_libraries(${testname} PUBLIC xtensor xtl)
    target_include_directories(${testname} PRIVATE "${GOOSEEYE_INCLUDE_DIR}")
    if(WARNINGS)
        if(MSVC)
            target_compile_options(${testname} PRIVATE /W4)
        else()
            target_compile_options(${testname} PRIVATE -Wall -Wextra -pedantic -Wno-unknown-pragmas)
        endif()
    endif()
endforeach()

