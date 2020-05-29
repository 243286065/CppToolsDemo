
find_path(LIBGO_INCLUDE libgo.h HINTS "${PROJECT_SOURCE_DIR}/src/libgo/libgo/" "/usr/include" "/usr/local/include" "/opt/local/include" )

if (LIBGO_INCLUDE)
    set(LIBGO_FOUND TRUE)

    message( STATUS "Found lingo include at: ${LIBGO_INCLUDE}" )
else ()
    message( FATAL_ERROR "Failed to locate libgo dependency." )
endif ()