project(dekko)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(UBUNTU_PROJECT_TYPE "ClickApp" CACHE INTERNAL "Let Qtc know were a click app")
set(UBUNTU_MANIFEST_PATH "click/manifest.json" CACHE INTERNAL "Tells QtCreator location and name of the manifest file")

cmake_minimum_required(VERSION 2.8.9)
# POSITION_INDEPENDENT_CODE is only available on cmake 2.8.9 or later
# Add needed flags for supported compilers which simulate POSITION_INDEPENDENT_CODE property
if(CMAKE_VERSION VERSION_LESS "2.8.9")
    if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    else()
        message(FATAL_ERROR "Needs GNU or Clang C++ compiler or CMake 2.8.9 (or later)")
    endif()
else()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# First check the compiler is up to the task
include(FindCXXFeatures)
if(NOT CXXFeatures_auto_FOUND)
    message(SEND_ERROR "Your compiler doesn't support C++11's auto")
endif()
if(NOT CXXFeatures_static_assert_FOUND)
    message(SEND_ERROR "Your compiler doesn't support C++11's static_assert")
endif()
if(NOT CXXFeatures_alignof_FOUND)
    if(NOT CMAKE_COMPILER_IS_GNUCXX AND NOT MSVC)
        message(SEND_ERROR "Your compiler doesn't support C++11's alignof and it also isn't gcc or MSVC. Either would work.")
    endif()
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS "-O2 -g -Wall -Wsign-compare ${CMAKE_CXX_FLAGS}")

include(DekkoConfigOption)
dekko_option(CLICK_MODE "Installs to a contained location. From which a click package can be created" ON)
dekko_option(UPDATE_TRANSLATIONS "Updates Dekko's pot file during build" OFF)
dekko_option(USE_ONLINE_ACCOUNTS "Switches Dekko's account store to use Ubuntu online accounts" OFF)
dekko_option(WITH_TESTS "Build tests" OFF)
dekko_option(ENABLE_LOGGING "Enables logging" ON)

if(NOT ZLIB_FOUND)
    dekko_find_package(ZLIB "" "" "" "" WITH_ZLIB)
endif()
#if(WITH_ZLIB)
#    include_directories(${ZLIB_INCLUDE_DIR})
#else()
#    message(FATAL_ERROR " zlib required for IMAP COMPRESS. Check you have 'zlib' installed")
#endif()

set(QMF_INSTALL_ROOT "/home/dan/test-qmf-install")

find_program(DPKG_ARCHITECTURE_EXECUTABLE dpkg-architecture)
if(DPKG_ARCHITECTURE_EXECUTABLE)
    execute_process(
        COMMAND ${DPKG_ARCHITECTURE_EXECUTABLE} -qDEB_HOST_MULTIARCH
            OUTPUT_VARIABLE ARCH_TRIPLET
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        # Get the build host arch and test it is armhf, since
        # CMAKE_LIBRARY_ARCHITECTURE and CMAKE_SYSTEM_PROCESSOR don't contain
        # any value inside the click chroot?? query the DEB_HOST_ARCH
        COMMAND ${DPKG_ARCHITECTURE_EXECUTABLE} -qDEB_HOST_ARCH
            OUTPUT_VARIABLE HOST_ARCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    message(FATAL_ERROR "Ubuntu click mode requires dpkg-architecture be installed, stopping build")
endif()

set(CMAKE_AUTOMOC True)

feature_summary(FATAL_ON_MISSING_REQUIRED_PACKAGES DESCRIPTION "\n" WHAT ALL)
add_subdirectory(qmf)
add_subdirectory(app)
add_subdirectory(click)
add_subdirectory(plugins)
add_subdirectory(server)
