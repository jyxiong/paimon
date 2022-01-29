# locate the glm library.
#
# This module defines the following variables:
#
# GLM_INCLUDE_DIR - where to find glm/glm.hpp
# GLM_FOUND - if the library was successfully located

# search for include
set(GLM_HEADER_SEARCH_DIRS
    ${CMAKE_SOURCE_DIR}/3rdparty/glm
)
find_path(GLM_INCLUDE_DIR NAMES glm/glm.hpp
    PATHS ${GLM_HEADER_SEARCH_DIRS})

# set GLM_FOUND
if(GLM_INCLUDE_DIR)
    set(GLM_FOUND TRUE)
    message(STATUS "GLM found: " ${GLM_INCLUDE_DIR})
else()
    message("GLM not found")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLM DEFAULT_MSG
    GLM_INCLUDE_DIR
)
