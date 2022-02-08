# Locate the log library
#
# This module defines the following variables:
#
# GLOG_LIBRARY the name of the library;
# GLOG_INCLUDE_DIR where to find log include files.
# GLOG_FOUND true if both the GLOG_LIBRARY and GLOG_INCLUDE_DIR have been found.

# search for include
set(GLOG_HEADER_SEARCH_DIRS
        ${CMAKE_SOURCE_DIR}/3rdparty/glog/build
        ${CMAKE_SOURCE_DIR}/3rdparty/glog/src
        )

find_path(GLOG_INCLUDE_DIR_BUILD NAMES glog/logging.h
        PATHS ${GLOG_HEADER_SEARCH_DIRS})

find_path(GLOG_INCLUDE_DIR_SOURCE NAMES glog/platform.h
        PATHS ${GLOG_HEADER_SEARCH_DIRS})

set(GLOG_INCLUDE_DIR
        ${GLOG_INCLUDE_DIR_BUILD}
        ${GLOG_INCLUDE_DIR_SOURCE}
        )

# search for the library
set(GLOG_LIB_SEARCH_DIRS
        ${CMAKE_SOURCE_DIR}/3rdparty/glog/build
        )
find_library(GLOG_LIBRARY NAMES glog
        PATHS ${GLOG_LIB_SEARCH_DIRS})

# set GLOG_FOUND
if(GLOG_INCLUDE_DIR AND GLOG_LIBRARY)
    set(GLOG_FOUND TRUE)
    message(STATUS "GLOG found: " ${GLOG_INCLUDE_DIR})
else()
    message("GLOG not found")
endif()

# set log package
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLOG DEFAULT_MSG
        GLOG_LIBRARY GLOG_INCLUDE_DIR
        )
