# Locate the glad library
#
# This module defines the following variables:
#
# GLAD_LIBRARY the name of the library;
# GLAD_INCLUDE_DIR where to find glad include files.
# GLAD_FOUND true if both the GLAD_LIBRARY and GLAD_INCLUDE_DIR have been found.

# search for include
set(GLAD_HEADER_SEARCH_DIRS
    ${CMAKE_SOURCE_DIR}/3rdparty/glad/include
)
find_path(GLAD_INCLUDE_DIR NAMES glad/glad.h
    PATHS ${GLAD_HEADER_SEARCH_DIRS})

# search for the library
set(GLAD_LIB_SEARCH_DIRS
    ${CMAKE_SOURCE_DIR}/3rdparty/glad/build
)
find_library(GLAD_LIBRARY NAMES glad 
    PATHS ${GLAD_LIB_SEARCH_DIRS})

# set GLAD_FOUND
if(GLAD_INCLUDE_DIR AND GLAD_LIBRARY)
    set(GLAD_FOUND TRUE)
    message(STATUS "GLAD found: " ${GLAD_INCLUDE_DIR})
else()
    message("GLAD not found")
endif()

# set GLAD package
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLAD DEFAULT_MSG
    GLAD_LIBRARY GLAD_INCLUDE_DIR
)
