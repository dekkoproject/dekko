project(dekko)
set(CMAKE_GENERATOR "Unix Makefiles")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(UBUNTU_PROJECT_TYPE "ClickApp" CACHE INTERNAL "Let Qtc know we're a click app")
set(UBUNTU_MANIFEST_PATH "click/dekko.json" CACHE INTERNAL "Tells QtCreator location and name of the manifest file")

cmake_minimum_required(VERSION 3.0)

include(GNUInstallDirs)

include(DekkoConfigOption)
option(CLICK_MODE "Installs to a contained location. From which a click package can be created" ON)
option(SNAP_MODE "Configures libs and Installs to locations suitable for snaps" OFF)
option(UNITY8 "Adjusts client/server to work under confinement" OFF)
option(SERVER_AS_THREAD "Run the message server as a new thread instead of a QProcess" OFF)
option(WITH_TESTS "Build tests" OFF)
option(ENABLE_LOGGING "Enables logging" ON)
option(ENABLE_QML_DEBUG "Enables qml debugging" OFF)
option(UNCONFINED "Run dekko unconfined" OFF)
option(UPDATE_TRANSLATIONS "Updates Dekko's pot file during build" OFF)
option(USE_ONLINE_ACCOUNTS "EXPERIMENTAL: Switches Dekko's account store to use Ubuntu online accounts" OFF)
option(ENABLE_IMAP_SUPPORT "Enables dekko's IMAP plugin support" ON)
option(ENABLE_POP3_SUPPORT "EXPERIMENTAL: Enables dekko's POP3 plugin support" OFF)
option(ENABLE_UBUNTU_CONNECTIVITY "Enable support for unity8 connectivity api" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC True)


if(NOT ZLIB_FOUND)
    find_package(ZLIB)
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
        message(FATAL_ERROR "Could no find accounts-qt")
    else()
        message(STATUS "accounts-qt found: include dirs = ${ACCOUNTS_QT_INCLUDE_DIRS}")
    endif()
    include_directories(${ACCOUNTS_QT_INCLUDE_DIRS})
    link_directories(${ACCOUNTS_QT_INCLUDE_DIRS})
    pkg_search_module(SIGNON_QT REQUIRED libsignon-qt5)
    if(NOT SIGNON_QT_FOUND)
        message(FATAL_ERROR "Could not find signon-qt5, have you got libaccounts-qt5-dev installed??")
    else()
        message(STATUS "signon-qt5 found: include dirs = ${SIGNON_QT_INCLUDE_DIRS}")
    endif()
    include_directories(${SIGNON_QT_INCLUDE_DIRS})
    link_directories(${SIGNON_QT_INCLUDE_DIRS})
endif()


set(ACCOUNT_ICON_DIR ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps)
set(ACCOUNT_PROVIDER_DIR ${CMAKE_INSTALL_DATADIR}/accounts/providers)
set(ACCOUNT_SERVICE_DIR ${CMAKE_INSTALL_DATADIR}/accounts/services)
set(ACCOUNT_QML_PLUGIN_DIR ${CMAKE_INSTALL_DATADIR}/accounts/qml-plugins)

set(APP_VERSION 0.9.4)

if(CLICK_MODE)
    if(NOT "${CMAKE_INSTALL_PREFIX}" STREQUAL "/")
        set(CMAKE_INSTALL_PREFIX "/")
    endif()
    set(QT_IMPORTS_DIR lib/${ARCH_TRIPLET})
    set(BIN_DIR ${QT_IMPORTS_DIR}/bin)
    set(DATA_DIR ${CMAKE_INSTALL_PREFIX})
    set(DESKTOP_DIR ${DATA_DIR})
    set(ICON_DIR ${DATA_DIR})
    set(DEKKO_PLUGIN_DIR ${QT_IMPORTS_DIR}/Dekko/plugins)
    set(QMF_PLUGINS_DIR ${QT_IMPORTS_DIR}/qmf/plugins5)
    set(LIB_DIR ${QT_IMPORTS_DIR})
    set(QMF_LIB_DIR ${QT_IMPORTS_DIR})
endif(CLICK_MODE)

if(SNAP_MODE)
    set(CMAKE_INSTALL_PREFIX "/usr")
    set(QT_IMPORTS_DIR ${CMAKE_INSTALL_LIBDIR}/qt5/qml)
    set(BIN_DIR ${CMAKE_INSTALL_BINDIR})
    set(DATA_DIR ${CMAKE_INSTALL_DATADIR}/dekko)
    set(DESKTOP_DIR ${CMAKE_INSTALL_DATADIR}/applications)
    set(ICON_DIR ${DATA_DIR})
    set(DEKKO_PLUGIN_DIR ${CMAKE_INSTALL_LIBDIR}/Dekko/plugins)
    set(QMF_PLUGINS_DIR ${CMAKE_INSTALL_LIBDIR}/qmf/plugins5)
    set(LIB_DIR ${CMAKE_INSTALL_LIBDIR}/Dekko)
    set(QMF_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
endif(SNAP_MODE)

include(Upstream)
##################################################
# Fetch MazDB which is our leveldb based library #
##################################################
set(MAZ_DB_ARGS
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DLIB_DIR:STRING=${LIB_DIR}
    -DQT_IMPORTS_DIR:STRING=${QT_IMPORTS_DIR}
)

fetch_upstream(
    PROJ maz-db
    GIT_REPOSITORY http://code.dekkoproject.org/dekko-dev/maz-db.git
    GIT_TAG master
    CMAKE_ARGS ${MAZ_DB_ARGS}
)

add_subdirectory(${CMAKE_BINARY_DIR}/maz-db-src
    ${CMAKE_BINARY_DIR}/maz-db-build)


set(MAZDB_SRCS ${maz-db_SOURCE_DIR} ${maz-db_BINARY_DIR})

#####################################################
# Fetch our qt-messaginging-framework based library #
#####################################################
set(QMF_ARGS
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DQMF_LIB_DIR:STRING=${QMF_LIB_DIR}
    -DENABLE_LOGGING:BOOL=${ENABLE_LOGGING}
    -DUSE_ONLINE_ACCOUNTS:BOOL=${USE_ONLINE_ACCOUNTS}
    -DUNITY8:BOOL=${UNITY8}
    -DSNAP_MODE:BOOL=${SNAP_MODE}
    -DCLICK_MODE:BOOL=${CLICK_MODE}
    -DENABLE_UBUNTU_CONNECTIVITY:BOOL=${ENABLE_UBUNTU_CONNECTIVITY}
)

fetch_upstream(
    PROJ qmf
    GIT_REPOSITORY http://code.dekkoproject.org/dekko-dev/messagingframework.git
    GIT_TAG master
    CMAKE_ARGS ${QMF_ARGS}
)

add_subdirectory(${CMAKE_BINARY_DIR}/qmf-src
    ${CMAKE_BINARY_DIR}/qmf-build)

set(QMFCLIENT_INCLUDES ${qmf_SOURCE_DIR}/libraries/qmfclient)
set(QMFCLIENT_SUPPORT_INCLUDES ${qmf_SOURCE_DIR}/libraries/qmfclient/support)
set(QMFSERVER_INCLUDES ${qmf_SOURCE_DIR}/libraries/qmfmessageserver)

#######################################
# Fetch QuickFlux qml event framework #
#######################################
set(QUICK_FLUX_ARGS
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DQT_IMPORTS_DIR:STRING=${QT_IMPORTS_DIR}
)

fetch_upstream(
    PROJ quick-flux
    GIT_REPOSITORY http://code.dekkoproject.org/dekko-dev/quick-flux.git
    GIT_TAG master
    CMAKE_ARGS ${QUICK_FLUX_ARGS}
)

add_subdirectory(${CMAKE_BINARY_DIR}/quick-flux-src
    ${CMAKE_BINARY_DIR}/quick-flux-build)

###############################
# Fetch the SuperMacros       #
###############################
fetch_upstream(
    PROJ super-macros
    GIT_REPOSITORY http://code.dekkoproject.org/dekko-dev/super-macros.git
    GIT_TAG master
)

# Make available to all sub dirs
set(SUPER_MACRO_INCLUDES ${super-macros_SOURCE_DIR})

###############################
# Fetch our Notify qml plugin #
###############################
set(NOTIFY_ARGS
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DQT_IMPORTS_DIR:STRING=${QT_IMPORTS_DIR}
)

fetch_upstream(
    PROJ notify
    GIT_REPOSITORY http://code.dekkoproject.org/dekko-dev/notify.git
    GIT_TAG master
    CMAKE_ARGS ${NOTIFY_ARGS}
)

add_subdirectory(${CMAKE_BINARY_DIR}/notify-src
    ${CMAKE_BINARY_DIR}/notify-build)

##########################################################
# Fetch Controls plugin - StackView borrowed from QQC1.2 #
##########################################################
set(CONTROLS_ARGS
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DQT_IMPORTS_DIR:STRING=${QT_IMPORTS_DIR}
)

fetch_upstream(
    PROJ controls
    GIT_REPOSITORY http://code.dekkoproject.org/dekko-dev/controls.git
    GIT_TAG master
    CMAKE_ARGS ${CONTROLS_ARGS}
)

add_subdirectory(${CMAKE_BINARY_DIR}/controls-src
    ${CMAKE_BINARY_DIR}/controls-build)

######################################
# Fetch Plugger the plugin framework #
######################################
set(PLUGGER_ARGS
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DLIB_DIR:STRING=${LIB_DIR}
    -DQT_IMPORTS_DIR:STRING=${QT_IMPORTS_DIR}
)

fetch_upstream(
    PROJ plugger
    GIT_REPOSITORY http://code.dekkoproject.org/dekko-dev/plugger.git
    GIT_TAG master
    CMAKE_ARGS ${PLUGGER_ARGS}
)

add_subdirectory(${CMAKE_BINARY_DIR}/plugger-src
    ${CMAKE_BINARY_DIR}/plugger-build)

set(PLUGGER_INCLUDES ${plugger_SOURCE_DIR}/src)

###########################
# Setup libs and includes #
###########################
add_subdirectory(libs)
set(ACCOUNTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/accounts)
set(MAIL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/mail)
set(SETTINGS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/settings)
set(NETWORK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/network)
set(SHARED_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/libs/shared)

#######################
# Setup plugins       #
#######################
include(SubdirsHelper)
add_all_subdirectories(${CMAKE_CURRENT_SOURCE_DIR}/plugins)
add_subdirectory(app)
add_subdirectory(click)
add_subdirectory(server)

feature_summary(FATAL_ON_MISSING_REQUIRED_PACKAGES DESCRIPTION "\n" WHAT ALL)

file(GLOB_RECURSE DEKKO_GLOBAL_QML_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.qml *.js qmldir
    icons/*svg
    *.dox *.md
)

add_custom_target(DekkoGlobalQml ALL SOURCES ${DEKKO_GLOBAL_QML_FILES})
