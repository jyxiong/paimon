# Locate the gflags library
#
# This module defines the following variables:
#
# GFLAGS_LIBRARY the name of the library;
# GFLAGS_INCLUDE_DIR where to find gflags include files.
# GFLAGS_FOUND true if both the GFLAGS_LIBRARY and GFLAGS_INCLUDE_DIR have been found.

# search for include
set(GFLAGS_HEADER_SEARCH_DIRS
        ${CMAKE_SOURCE_DIR}/3rdparty/gflags/_build/include
        )
find_path(GFLAGS_INCLUDE_DIR NAMES gflags/gflags.h
        PATHS ${GFLAGS_HEADER_SEARCH_DIRS})

# search for the library
set(GFLAGS_LIB_SEARCH_DIRS
        ${CMAKE_SOURCE_DIR}/3rdparty/gflags/_build/lib
        )
find_library(GFLAGS_LIBRARY NAMES gflags
        PATHS ${GFLAGS_LIB_SEARCH_DIRS})

# set GFLAGS_FOUND
if(GFLAGS_INCLUDE_DIR AND GFLAGS_LIBRARY)
    set(GFLAGS_FOUND TRUE)
    message(STATUS "GFLAGS found: " ${GFLAGS_INCLUDE_DIR})
else()
    message("GFLAGS not found")
endif()

# set gflags package
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GFLAGS DEFAULT_MSG
        GFLAGS_LIBRARY GFLAGS_INCLUDE_DIR
        )
