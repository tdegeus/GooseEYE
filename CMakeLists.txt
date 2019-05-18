#==================================================================================================#
#                                                                                                  #
# (c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE       #
#                                                                                                  #
#==================================================================================================#

# initialization
# --------------

# required to specify the c++ standard
cmake_minimum_required(VERSION 3.0)

# required for install
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# options
option(PKGCONFIG "Build pkg-config" ON)
option(BUILD_TESTS "${PROJECT_NAME} test suite" OFF)

# project settings
# ----------------

# name
project(GooseEYE)

# file that contains the version information
set(parse_version include/GooseEYE/config.h)

# header files
set(headers
  include/GooseEYE/Ensemble.hpp
  include/GooseEYE/Ensemble_C2.hpp
  include/GooseEYE/Ensemble_S2.hpp
  include/GooseEYE/Ensemble_W2.hpp
  include/GooseEYE/Ensemble_mean.hpp
  include/GooseEYE/Ensemble_roughness.hpp
  include/GooseEYE/EnsembleVariance.hpp
  include/GooseEYE/EnsembleVariance_roughness.hpp
  include/GooseEYE/GooseEYE.h
  include/GooseEYE/GooseEYE.hpp
  include/GooseEYE/clusters.hpp
  include/GooseEYE/config.h
  include/GooseEYE/detail.h
  include/GooseEYE/detail.hpp
  include/GooseEYE/dummy_circles.hpp
  include/GooseEYE/kernel.hpp
  include/GooseEYE/path.hpp
)

# automatically parse the version number
file(READ "${parse_version}" version)
string(REGEX MATCH "define[ \t]+GOOSEEYE_WORLD_VERSION[ \t]+([0-9]+)" _ "${version}")
set(world_version "${CMAKE_MATCH_1}")
string(REGEX MATCH "define[ \t]+GOOSEEYE_MAJOR_VERSION[ \t]+([0-9]+)" _ "${version}")
set(major_version "${CMAKE_MATCH_1}")
string(REGEX MATCH "define[ \t]+GOOSEEYE_MINOR_VERSION[ \t]+([0-9]+)" _ "${version}")
set(minor_version "${CMAKE_MATCH_1}")
set(GOOSEEYE_VERSION ${world_version}.${major_version}.${minor_version})

# print information to screen
message(STATUS "Building ${PROJECT_NAME} v${GOOSEEYE_VERSION}")

# paths
# -----

set(GOOSEEYE_ROOT_DIR        "${CMAKE_INSTALL_PREFIX}")
set(GOOSEEYE_INCLUDE_DIR     "${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_DIR}")
set(INCLUDE_INSTALL_DIR      "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
set(CMAKEPACKAGE_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")
set(PKGCONFIG_INSTALL_DIR    "${CMAKE_INSTALL_DATADIR}/pkgconfig")
set(fcmake                   "GooseEYEConfig.cmake")
set(fpkg                     "GooseEYE.pc")

# configure CMake
# ---------------

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/GooseEYEConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/GooseEYEConfig.cmake
  PATH_VARS GOOSEEYE_INCLUDE_DIR GOOSEEYE_ROOT_DIR
  INSTALL_DESTINATION ${CMAKEPACKAGE_INSTALL_DIR}
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# install/build
# -------------

# build test cases
if(BUILD_TESTS)
  add_subdirectory(test)
endif()

# disable pkg-config for native Windows builds
if(WIN32 OR CMAKE_HOST_SYSTEM_NAME MATCHES Windows)
  option(PKGCONFIG "Build pkg-config ${fpkg} file" OFF)
endif()

# pkg-config
if(PKGCONFIG)
  configure_file(${fpkg}.in ${fpkg} @ONLY)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${fpkg} DESTINATION ${PKGCONFIG_INSTALL_DIR})
endif()

# CMake
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${fcmake} DESTINATION ${CMAKEPACKAGE_INSTALL_DIR})

# header files
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${headers} DESTINATION ${INCLUDE_INSTALL_DIR})

# print information to screen
# ---------------------------

message(STATUS "")
message(STATUS "+---------------------------------------------------------------------------------")
message(STATUS "|")
message(STATUS "| Use 'make install' to install in")
message(STATUS "|   ${CMAKE_INSTALL_PREFIX}")
if(BUILD_TESTS)
  message(STATUS "|")
  message(STATUS "| Use 'make' and './test/test' to run the tests")
endif()
message(STATUS "|")
message(STATUS "| To specify a custom directory call")
message(STATUS "|   cmake /path/to/${PROJECT_NAME} -DCMAKE_INSTALL_PREFIX=yourprefix")
message(STATUS "|")
message(STATUS "| For custom paths, add the following line to your '~/.bashrc'")
message(STATUS "|   export PKG_CONFIG_PATH=${CMAKE_INSTALL_PREFIX}/share/pkgconfig:$PKG_CONFIG_PATH")
message(STATUS "|")
message(STATUS "+---------------------------------------------------------------------------------")
message(STATUS "")
