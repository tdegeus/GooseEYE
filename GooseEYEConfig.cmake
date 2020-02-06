# GooseEYE cmake module
#
# This module sets the target:
#
#   GooseEYE
#
# In addition, it sets the following variables:
#
#   GooseEYE_FOUND - true if GooseEYE found
#   GooseEYE_VERSION - GooseEYE's version
#   GooseEYE_INCLUDE_DIRS - the directory containing GooseEYE headers
#
# The following support targets are defined to simplify things:
#
#   GooseEYE::compiler_warnings - enable compiler warnings
#   GooseEYE::assert - enable GooseEYE assertions
#   GooseEYE::debug - enable all assertions (slow)

include(CMakeFindDependencyMacro)

# Define target "GooseEYE"

if(NOT TARGET GooseEYE)
    include("${CMAKE_CURRENT_LIST_DIR}/GooseEYETargets.cmake")
    get_target_property(GooseEYE_INCLUDE_DIRS GooseEYE INTERFACE_INCLUDE_DIRECTORIES)
endif()

# Find dependencies

find_dependency(xtensor)

# Define support target "GooseEYE::compiler_warnings"

if(NOT TARGET GooseEYE::compiler_warnings)
    add_library(GooseEYE::compiler_warnings INTERFACE IMPORTED)
    if(MSVC)
        set_property(
            TARGET GooseEYE::compiler_warnings
            PROPERTY INTERFACE_COMPILE_OPTIONS
            /W4)
    else()
        set_property(
            TARGET GooseEYE::compiler_warnings
            PROPERTY INTERFACE_COMPILE_OPTIONS
            -Wall -Wextra -pedantic -Wno-unknown-pragmas)
    endif()
endif()

# Define support target "GooseEYE::assert"

if(NOT TARGET GooseEYE::assert)
    add_library(GooseEYE::assert INTERFACE IMPORTED)
    set_property(
        TARGET GooseEYE::assert
        PROPERTY INTERFACE_COMPILE_DEFINITIONS
        GOOSEEYE_ENABLE_ASSERT)
endif()

# Define support target "GooseEYE::debug"

if(NOT TARGET GooseEYE::debug)
    add_library(GooseEYE::debug INTERFACE IMPORTED)
    set_property(
        TARGET GooseEYE::debug
        PROPERTY INTERFACE_COMPILE_DEFINITIONS
        XTENSOR_ENABLE_ASSERT GOOSEEYE_ENABLE_ASSERT)
endif()
