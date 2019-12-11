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

include(CMakeFindDependencyMacro)
find_dependency(xtensor)

if(NOT TARGET GooseEYE)
    include("${CMAKE_CURRENT_LIST_DIR}/GooseEYETargets.cmake")
    get_target_property(GooseEYE_INCLUDE_DIRS GooseEYE INTERFACE_INCLUDE_DIRECTORIES)
endif()

if(NOT TARGET GooseEYE::compiler_warnings)
    add_library(GooseEYE::compiler_warnings INTERFACE IMPORTED)
    if(MSVC)
        target_compile_options(GooseEYE::compiler_warnings INTERFACE
            /W4)
    else()
        target_compile_options(GooseEYE::compiler_warnings INTERFACE
            -Wall
            -Wextra
            -pedantic
            -Wno-unknown-pragmas)
    endif()
endif()

if(NOT TARGET GooseEYE::assert)
    add_library(GooseEYE::assert INTERFACE IMPORTED)
    target_compile_definitions(GooseEYE::assert INTERFACE GOOSEEYE_ENABLE_ASSERT)
endif()

if(NOT TARGET GooseEYE::debug)
    add_library(GooseEYE::debug INTERFACE IMPORTED)
    target_compile_definitions(GooseEYE::debug INTERFACE
        XTENSOR_ENABLE_ASSERT
        GOOSEEYE_ENABLE_ASSERT)
endif()
