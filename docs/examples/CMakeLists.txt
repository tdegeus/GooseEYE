
cmake_minimum_required(VERSION 3.0)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(GooseEYE-examples)
    find_package(GooseEYE REQUIRED CONFIG)
    set(GOOSEEYE_INCLUDE_DIR ${GooseEYE_INCLUDE_DIRS})
endif()

option(WARNINGS "Show build warnings" ON)
option(ASSERT "Enable assertions" ON)

file(GLOB APP_SOURCES *.cpp)

foreach(testsourcefile ${APP_SOURCES})

    string(REPLACE ".cpp" "" testname ${testsourcefile})
    get_filename_component(testname ${testname} NAME)

    add_executable(${testname} ${testsourcefile})

    target_link_libraries(${testname} PUBLIC
        GooseEYE
        xtensor::optimize
        xtensor::use_xsimd)

    if(WARNINGS)
        if(MSVC)
            target_compile_options(${testname} PRIVATE /W4)
        else()
            target_compile_options(${testname} PRIVATE -Wall -Wextra -pedantic -Wno-unknown-pragmas)
        endif()
    endif()

    if(NOT ASSERT)
        target_compile_definitions(${testname} PRIVATE NDEBUG)
    else()
        target_compile_definitions(${testname} PRIVATE XTENSOR_ENABLE_ASSERT GOOSEEYE_ENABLE_ASSERT)
    endif()

endforeach()

