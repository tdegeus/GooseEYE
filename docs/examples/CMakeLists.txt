cmake_minimum_required(VERSION 3.1)

# define a project name
project(examples)

# set optimization level
set(CMAKE_BUILD_TYPE Release)

# set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set warnings on
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -g")

# enable optimisation
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DXTENSOR_USE_XSIMD=ON")

# assertions
if(NOT ASSERT)
  add_definitions(-DNDEBUG)
else()
  add_definitions(-DXTENSOR_ENABLE_ASSERT=ON)
endif()

# add current library
string(CONCAT GooseEYE "${CMAKE_CURRENT_SOURCE_DIR}" "/../../include/")
include_directories("${GooseEYE}")

# load xtensor-stack libraries
find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)

# add custom include paths
if(NOT "$ENV{INCLUDE_PATH}" STREQUAL "")
  string(REPLACE ":" ";" INCLUDE_LIST "$ENV{INCLUDE_PATH}")
  include_directories(${INCLUDE_LIST})
endif()

# create executables
file(GLOB APP_SOURCES *.cpp)
foreach(testsourcefile ${APP_SOURCES})
    string(REPLACE ".cpp" "" testname ${testsourcefile})
    get_filename_component(testname ${testname} NAME)
    add_executable(${testname} ${testsourcefile})
endforeach()