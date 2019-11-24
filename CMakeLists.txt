
cmake_minimum_required(VERSION 3.0)

# Options
# =======

project(GooseEYE)

option(BUILD_TESTS "${PROJECT_NAME} Build tests" OFF)
option(BUILD_EXAMPLES "${PROJECT_NAME} Build examples" OFF)

# Internals
# =========

set(GOOSEEYE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Version
# =======

file(
    STRINGS "${GOOSEEYE_INCLUDE_DIR}/GooseEYE/config.h" gooseeye_version_defines
    REGEX "#define GOOSEEYE_VERSION_(MAJOR|MINOR|PATCH)")

foreach(ver ${gooseeye_version_defines})
    if(ver MATCHES "#define GOOSEEYE_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(GOOSEEYE_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif()
endforeach()

set(GOOSEEYE_VERSION
    ${GOOSEEYE_VERSION_MAJOR}.${GOOSEEYE_VERSION_MINOR}.${GOOSEEYE_VERSION_PATCH})

message(STATUS "Building GooseEYE v${GOOSEEYE_VERSION}")

# Build
# =====

set(GOOSEEYE_HEADERS
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/Ensemble.hpp
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/Ensemble_C2.hpp
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/Ensemble_L.hpp
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/Ensemble_S2.hpp
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/Ensemble_W2.hpp
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/Ensemble_heightheight.hpp
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/Ensemble_mean.hpp
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/GooseEYE.h
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/GooseEYE.hpp
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/clusters.hpp
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/config.h
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/detail.h
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/detail.hpp
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/dilate.hpp
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/dummy_circles.hpp
    ${GOOSEEYE_INCLUDE_DIR}/GooseEYE/kernel.hpp)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(
    ${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${GOOSEEYE_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>)

# Installation
# ============

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets)

export(
    EXPORT ${PROJECT_NAME}-targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")

install(
    FILES ${GOOSEEYE_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

set(GOOSEEYE_CMAKECONFIG_INSTALL_DIR
    "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE
    STRING "install path for ${PROJECT_NAME}Config.cmake")

configure_package_config_file(
    "GooseEYEConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${GOOSEEYE_CMAKECONFIG_INSTALL_DIR})

set(_GOOSEEYE_CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})

unset(CMAKE_SIZEOF_VOID_P)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${GOOSEEYE_VERSION}
    COMPATIBILITY AnyNewerVersion)

set(CMAKE_SIZEOF_VOID_P ${_GOOSEEYE_CMAKE_SIZEOF_VOID_P})

install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION ${GOOSEEYE_CMAKECONFIG_INSTALL_DIR})

install(
    EXPORT ${PROJECT_NAME}-targets
    FILE "${PROJECT_NAME}Targets.cmake"
    DESTINATION ${GOOSEEYE_CMAKECONFIG_INSTALL_DIR})

configure_file(
    "GooseEYE.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
     @ONLY)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig/")

# Add builds
# ==========

if(BUILD_TESTS)
    add_subdirectory(test)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(docs/examples)
endif()



