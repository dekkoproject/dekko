project(dekko)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(UBUNTU_PROJECT_TYPE "ClickApp" CACHE INTERNAL "Let Qtc know we're a click app")
set(UBUNTU_MANIFEST_PATH "click/dekko.json" CACHE INTERNAL "Tells QtCreator location and name of the manifest file")

cmake_minimum_required(VERSION 2.8.9)

cmake_policy(SET CMP0028 OLD)
if(POLICY CMP0043)
    cmake_policy(SET CMP0043 OLD)
endif()
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
if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    set(CMAKE_CXX_FLAGS "-O0 -g -Wall -Wsign-compare ${CMAKE_CXX_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "-O2 -Wall -Wsign-compare ${CMAKE_CXX_FLAGS} -s")
endif()

message(STATUS "CXXFLAGS ARE ${CMAKE_CXX_FLAGS}")
include(DekkoConfigOption)
dekko_option(CLICK_MODE "Installs to a contained location. From which a click package can be created" ON)
dekko_option(UPDATE_TRANSLATIONS "Updates Dekko's pot file during build" OFF)
dekko_option(USE_ONLINE_ACCOUNTS "Switches Dekko's account store to use Ubuntu online accounts" OFF)
dekko_option(WITH_TESTS "Build tests" OFF)
dekko_option(ENABLE_LOGGING "Enables logging" ON)
dekko_option(ENABLE_QML_DEBUG "Enables qml debugging" OFF)
dekko_option(UNCONFINED "Run dekko unconfined" OFF)

if(NOT ZLIB_FOUND)
    dekko_find_package(ZLIB "" "" "" "" WITH_ZLIB)
endif()

# If QMF_PLUGINS env var isn't set let's set the fallback directory
# assuming we are in a click configuration for now.
# for deb/snap installs we should use a wrapper script to launch dekko to
# set the environment up.
set(QMF_INSTALL_ROOT "../qmf/plugins5")

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

include(FindPkgConfig)

if(USE_ONLINE_ACCOUNTS)
pkg_search_module(ACCOUNTS_QT REQUIRED accounts-qt5)
if(NOT ACCOUNTS_QT_FOUND)
    MESSAGE(FATAL_ERROR "Could no find accounts-qt")
else()
    MESSAGE(STATUS "accounts-qt found: include dirs = ${ACCOUNTS_QT_INCLUDE_DIRS}")
endif()
INCLUDE_DIRECTORIES(${ACCOUNTS_QT_INCLUDE_DIRS})
LINK_DIRECTORIES(${ACCOUNTS_QT_INCLUDE_DIRS})
pkg_search_module(SIGNON_QT REQUIRED libsignon-qt5)
if(NOT SIGNON_QT_FOUND)
    MESSAGE(FATAL_ERROR "Could not find signon-qt5, have you got libaccounts-qt5-dev installed??")
else()
    MESSAGE(STATUS "signon-qt5 found: include dirs = ${SIGNON_QT_INCLUDE_DIRS}")
endif()
include_directories(${SIGNON_QT_INCLUDE_DIRS})
link_directories(${SIGNON_QT_INCLUDE_DIRS})
endif()

set(CMAKE_AUTOMOC True)

set(ACCOUNT_ICON_DIR ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps)
set(ACCOUNT_PROVIDER_DIR ${CMAKE_INSTALL_DATADIR}/accounts/providers)
set(ACCOUNT_SERVICE_DIR ${CMAKE_INSTALL_DATADIR}/accounts/services)
set(ACCOUNT_QML_PLUGIN_DIR ${CMAKE_INSTALL_DATADIR}/accounts/qml-plugins)


set(APP_VERSION 0.9)

if(CLICK_MODE)
    if(NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "/")
        set(CMAKE_INSTALL_PREFIX "/")
    endif()
    set(QT_IMPORTS_DIR lib/${ARCH_TRIPLET})
    set(BIN_DIR ${QT_IMPORTS_DIR}/bin)
    set(DATA_DIR ${CMAKE_INSTALL_PREFIX})
    set(DESKTOP_DIR ${DATA_DIR})
    set(ICON_DIR ${DATA_DIR})
    set(SOUND_DIR ${DATA_DIR}sounds)
else(CLICK_MODE)
    include(GNUInstallDirs)
    set(DEKKO_DESKTOP_ID "dekko.desktop")
endif(CLICK_MODE)

# Make available to all sub dirs
set(SUPER_MACRO_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/lib/3rdParty/SuperMacros)
set(ASYNC_TASKS_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/lib/3rdParty/async-tasks)

add_subdirectory(qmf)
add_subdirectory(lib)
add_subdirectory(app)
add_subdirectory(click)
add_subdirectory(plugins)
add_subdirectory(server)

feature_summary(FATAL_ON_MISSING_REQUIRED_PACKAGES DESCRIPTION "\n" WHAT ALL)

file(GLOB_RECURSE DEKKO_GLOBAL_QML_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.qml *.js qmldir
    icons/*svg
    *.qdoc *.qdocconf
)

add_custom_target(DekkoGlobalQml ALL SOURCES ${DEKKO_GLOBAL_QML_FILES})

