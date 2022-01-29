# Locate the glfw3 library
#
# This module defines the following variables:
#
# GLFW3_LIBRARY the name of the library;
# GLFW3_INCLUDE_DIR where to find glfw include files.
# GLFW3_FOUND true if both the GLFW3_LIBRARY and GLFW3_INCLUDE_DIR have been found.

# search for include
set(GLFW3_HEADER_SEARCH_DIRS
    ${CMAKE_SOURCE_DIR}/3rdparty/glfw/include
)
find_path(GLFW3_INCLUDE_DIR NAMES GLFW/glfw3.h
    PATHS ${GLFW3_HEADER_SEARCH_DIRS})

# search for the library
set(GLFW3_LIB_SEARCH_DIRS
    ${CMAKE_SOURCE_DIR}/3rdparty/glfw/build/src
)
find_library(GLFW3_LIBRARY NAMES glfw3 glfw 
    PATHS ${GLFW3_LIB_SEARCH_DIRS})

# set GLFW3_FOUND
if(GLFW3_INCLUDE_DIR AND GLFW3_LIBRARY)
    set(GLFW3_FOUND TRUE)
    message(STATUS "GLFW3 found: " ${GLFW3_INCLUDE_DIR})
else()
    message("GLFW3 not found")
endif()

# set GLFW3 package
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLFW3 DEFAULT_MSG
    GLFW3_LIBRARY GLFW3_INCLUDE_DIR
)
