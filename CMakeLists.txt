cmake_minimum_required(VERSION 3.0)

# Basic settings
# ==============

project(GooseEYE)

option(BUILD_TESTS "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)

# Version
# =======

file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/include/GooseEYE/config.h" GooseEYE_version_defines
     REGEX "#define GOOSEEYE_VERSION_(MAJOR|MINOR|PATCH)")

foreach(ver ${GooseEYE_version_defines})
    if(ver MATCHES "#define GOOSEEYE_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(GOOSEEYE_VERSION_${CMAKE_MATCH_1}
            "${CMAKE_MATCH_2}"
            CACHE INTERNAL "")
    endif()
endforeach()

set(GOOSEEYE_VERSION ${GOOSEEYE_VERSION_MAJOR}.${GOOSEEYE_VERSION_MINOR}.${GOOSEEYE_VERSION_PATCH})

message(STATUS "Building GooseEYE v${GOOSEEYE_VERSION}")

# Set target
# ==========

find_package(xtensor REQUIRED)

add_library(GooseEYE INTERFACE)

target_include_directories(GooseEYE INTERFACE
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(GooseEYE INTERFACE xtensor)

# Installation
# ============

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/" DESTINATION include)

install(TARGETS GooseEYE EXPORT GooseEYE-targets)

install(
    EXPORT GooseEYE-targets
    FILE GooseEYETargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/GooseEYE")

set(_GOOSEEYE ${CMAKE_SIZEOF_VOID_P})
unset(CMAKE_SIZEOF_VOID_P)

write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/GooseEYEConfigVersion.cmake"
    VERSION ${GOOSEEYE_VERSION}
    COMPATIBILITY AnyNewerVersion)

set(CMAKE_SIZEOF_VOID_P ${_GOOSEEYE})

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/GooseEYEConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/GooseEYEConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/GooseEYE")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/GooseEYE.pc.in"
               "${CMAKE_CURRENT_BINARY_DIR}/GooseEYE.pc" @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/GooseEYE.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig/")

# Add builds
# ==========

include("GooseEYEConfig.cmake")

if(BUILD_TESTS)
    add_subdirectory(test)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(docs/examples)
endif()



