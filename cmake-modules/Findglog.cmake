if (APPLE) 
    set(OPENSSL_ROOT_DIR /opt/homebrew/opt/openssl@3)
    set(GLOG_ROOT_DIR /opt/homebrew)
else()
    set(GLOG_ROOT_DIR /usr)
endif()

find_path(GLOG_INCLUDE_DIR
    NAMES logging.h
    HINTS ${GLOG_ROOT_DIR}/include/glog)
mark_as_advanced(GLOG_INCLUDE_DIR)

set(LIBRARY "libglog.so")
if (APPLE) 
    set(LIBRARY "libglog.dylib")
endif()

find_library(GLOG_LIBRARY
    NAMES ${LIBRARY}
    HINTS ${GLOG_ROOT_DIR}/lib)
mark_as_advanced(GLOG_LIBRARY)

set(GLOG_FOUND TRUE)
if (${GLOG_INCLUDE_DIR} MATCHES ".*-NOTFOUND")
set(GLOG_FOUND FALSE)
    message(FATAL_ERROR " GLOG_INCLUDE_DIR not found")
    return()
endif()

if (${GLOG_LIBRARY} MATCHES ".*-NOTFOUND")
    set(GLOG_FOUND FALSE)
    message(FATAL_ERROR " GLOG_LIBRARIES not found")
    return()
endif()

find_package(PkgConfig)
set(GLOG_INCLUDE_DIRS ${GLOG_INCLUDE_DIR})
set(GLOG_LIBRARIES ${GLOG_LIBRARY})

message(STATUS "Found GLOG: ${GLOG_LIBRARY}")