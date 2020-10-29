
cmake_minimum_required(VERSION 3.16)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(GooseEYE-examples)
    find_package(GooseEYE REQUIRED CONFIG)
    set(GOOSEEYE_INCLUDE_DIR ${GooseEYE_INCLUDE_DIRS})
endif()

set(WARNINGS ON)
set(ASSERT OFF)
set(DEBUG OFF)
option(SIMD "Enable xsimd" ON)

set(CMAKE_BUILD_TYPE Release)

set(HIGHFIVE_USE_BOOST 0)
set(HIGHFIVE_USE_XTENSOR 1)

find_package(xtensor REQUIRED)
find_package(HighFive REQUIRED)

if(SIMD)
    find_package(xsimd REQUIRED)
endif()

file(GLOB APP_SOURCES *.cpp)

foreach(testsourcefile ${APP_SOURCES})

    string(REPLACE ".cpp" "" testname ${testsourcefile})
    get_filename_component(testname ${testname} NAME)

    add_executable(${testname} ${testsourcefile})

    target_link_libraries(${testname} PUBLIC
        GooseEYE
        xtensor::optimize
        HighFive)

    if(SIMD)
        target_link_libraries(${testname} INTERFACE xtensor::use_xsimd)
    endif()

    if(WARNINGS)
        target_link_libraries(${testname} PRIVATE GooseEYE::compiler_warnings)
    endif()

    if(ASSERT)
        target_link_libraries(${testname} PRIVATE GooseEYE::assert)
    endif()

    if(DEBUG)
        target_link_libraries(${testname} PRIVATE GooseEYE::debug)
    endif()

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${testname}.h5)
        configure_file(${testname}.h5 ${testname}.h5 COPYONLY)
    endif()

endforeach()

