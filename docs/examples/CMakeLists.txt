cmake_minimum_required(VERSION 3.1..3.21)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  project(GooseEYE)
  find_package(GooseEYE REQUIRED CONFIG)
  option(USE_ASSERT "${PROJECT_NAME}: Build with assertions" ON)
  option(USE_DEBUG "${PROJECT_NAME}: Build in debug mode" OFF)
  option(USE_SIMD "${PROJECT_NAME}: Build with hardware optimization" OFF)
endif()

set(MYPROJECT "${PROJECT_NAME}-examples")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

set(HIGHFIVE_USE_BOOST 0)
set(HIGHFIVE_USE_XTENSOR 1)

find_package(HighFive REQUIRED)
find_package(xtensor REQUIRED)

add_library(mytarget INTERFACE IMPORTED)

target_link_libraries(
  mytarget INTERFACE ${PROJECT_NAME} ${PROJECT_NAME}::compiler_warnings
                     HighFive)

if(USE_ASSERT)
  target_link_libraries(mytarget INTERFACE ${PROJECT_NAME}::assert)
  message(STATUS "Compiling ${MYPROJECT} with assertions")
endif()

if(USE_DEBUG)
  target_link_libraries(mytarget INTERFACE ${PROJECT_NAME}::debug)
  message(STATUS "Compiling ${MYPROJECT} in debug mode")
endif()

if(USE_SIMD)
  find_package(xtensor REQUIRED)
  find_package(xsimd REQUIRED)
  target_link_libraries(mytarget INTERFACE xtensor::use_xsimd xtensor::optimize)
  message(STATUS "Compiling ${MYPROJECT} with hardware optimization")
endif()

file(GLOB APP_SOURCES *.cpp)

foreach(mysource ${APP_SOURCES})
  string(REPLACE ".cpp" "" myexec ${mysource})
  get_filename_component(myexec ${myexec} NAME)
  string(PREPEND myexec "example_")
  add_executable(${myexec} ${mysource})
  target_link_libraries(${myexec} PRIVATE mytarget)
endforeach()
