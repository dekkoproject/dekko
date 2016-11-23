project(plugger)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_GENERATOR "Unix Makefiles")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC True)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "-O0 -g -Wall -Wsign-compare ${CMAKE_CXX_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "-O2 -Wall -Wsign-compare ${CMAKE_CXX_FLAGS} -s")
endif()

message(STATUS "CXXFLAGS ARE ${CMAKE_CXX_FLAGS}")

include(GNUInstallDirs)

option(BUILD_EXAMPLE "Build the eample app" OFF)
mark_as_advanced(BUILD_EXAMPLE)

if(DEFINED LIB_DIR)
    message(STATUS "LIB_DIR already set. MazDB library will be installed to ${LIB_DIR}")
else()
    if(EXISTS "/etc/debian_version")
        set(LIB_DIR ${CMAKE_INSTALL_LIBDIR}/Dekko)
    else()
        set(LIB_DIR ${CMAKE_INSTALL_LIBDIR}/${CMAKE_LIBRARY_ARCHITECTURE}/Dekko)
    endif()

endif()

# Now setup qml plugin install paths
if(DEFINED QT_IMPORTS_DIR)
    message(STATUS "Qt import dir already set, install MazDB Quick Plugin to ${QT_IMPORTS_DIR}")
else()
    if(EXISTS "/etc/debian_version")
        set(QT_IMPORTS_DIR ${CMAKE_INSTALL_LIBDIR}/qt5/qml)
    else()
        set(QT_IMPORTS_DIR ${CMAKE_INSTALL_LIBDIR}/${CMAKE_LIBRARY_ARCHITECTURE}/qt5/qml)
    endif()
endif()

add_subdirectory(src)
if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()
