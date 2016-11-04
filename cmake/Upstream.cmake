set(_UpstreamProjectDir "${CMAKE_CURRENT_LIST_DIR}")

include(CMakeParseArguments)

function(fetch_upstream)

    set(options QUIET)
    set(singleValueArgs 
        PROJ 
        PREFIX 
        DOWNLOAD_DIR 
        SOURCE_DIR 
        BINARY_DIR 
        CONFIGURE_COMMAND 
        BUILD_COMMAND 
        INSTALL_COMMAND 
        TEST_COMMAND
    )
    set(multiValueArgs "")
    
    cmake_parse_arguments(
        UPSTREAM 
        "${options}" 
        "${singleValueArgs}" 
        "${multiValueArgs}" 
        ${ARGN}
    )
    
    if (UPSTREAM_QUIET)
        set(OUTPUT_QUIET "OUTPUT_QUIET")
    else()
        unset(OUTPUT_QUIET)
        message(STATUS "Fetching upstream: ${UPSTREAM_PROJ}")
    endif()
    
    if(NOT UPSTREAM_PREFIX)
        set(UPSTREAM_PREFIX "${CMAKE_BINARY_DIR}")
    endif()
    
    if(NOT UPSTREAM_DOWNLOAD_DIR)
        set(UPSTREAM_DOWNLOAD_DIR "${UPSTREAM_PREFIX}/${UPSTREAM_PROJ}-repo")
    endif()
    
    # Ensure the caller can know where to find the source and build directories
    if (NOT UPSTREAM_SOURCE_DIR)
        set(UPSTREAM_SOURCE_DIR "${UPSTREAM_PREFIX}/${UPSTREAM_PROJ}-src")
    endif()
    if (NOT UPSTREAM_BINARY_DIR)
        set(UPSTREAM_BINARY_DIR "${UPSTREAM_PREFIX}/${UPSTREAM_PROJ}-build")
    endif()
    set(${UPSTREAM_PROJ}_SOURCE_DIR "${UPSTREAM_SOURCE_DIR}" PARENT_SCOPE)
    set(${UPSTREAM_PROJ}_BINARY_DIR "${UPSTREAM_BINARY_DIR}" PARENT_SCOPE)
    
    # Create and build a separate CMake project to carry out the download.
    # If we've already previously done these steps, they will not cause
    # anything to be updated, so extra rebuilds of the project won't occur.
    configure_file("${_UpstreamProjectDir}/Upstream.CMakeLists.cmake.in"
                   "${UPSTREAM_DOWNLOAD_DIR}/CMakeLists.txt")
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                    ${OUTPUT_QUIET}
                    WORKING_DIRECTORY "${UPSTREAM_DOWNLOAD_DIR}"
    )
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
                    ${OUTPUT_QUIET}
                    WORKING_DIRECTORY "${UPSTREAM_DOWNLOAD_DIR}"
    )
    
endfunction()
    
