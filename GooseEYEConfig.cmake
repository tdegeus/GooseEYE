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
