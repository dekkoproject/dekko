project(dekko-notify)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_GENERATOR "Unix Makefiles")

option(NOTIFY_TESTS "Enable notify tests" OFF)
mark_as_advanced(NOTIFY_TESTS)

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

set(NOTIFY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(notify_SRCS
    ${NOTIFY_PATH}/NotifyQueue.qml
    ${NOTIFY_PATH}/Notification.qml
    ${NOTIFY_PATH}/NotificationBase.qml
    ${NOTIFY_PATH}/Notify.qml
    ${NOTIFY_PATH}/NotifyConfiguration.qml
    ${NOTIFY_PATH}/qmldir
    ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
)

add_custom_target(NotifySrcs ALL SOURCES ${notify_SRCS})

if(DEFINED QT_IMPORTS_DIR)
    message(STATUS "Qt imports dir already set. Installing Notify to ${QT_IMPORTS_DIR}")
else()
    if(EXISTS "/etc/debian_version")
        set(QT_IMPORTS_DIR ${CMAKE_INSTALL_LIBDIR}/qt5/qml)
    else()
        set(QT_IMPORTS_DIR ${CMAKE_INSTALL_LIBDIR}/${CMAKE_LIBRARY_ARCHITECTURE}/qt5/qml)
    endif()
endif()

install(
    FILES ${notify_SRCS}
    DESTINATION ${QT_IMPORTS_DIR}/Dekko/Notify
)

set(example_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/example/notify-example.qml
    ${CMAKE_CURRENT_SOURCE_DIR}/example/CustomNotification.qml
)

add_custom_target(ExampleSrcs ALL SOURCES ${example_SRCS})

add_custom_target("run" /usr/bin/qmlscene ${CMAKE_SOURCE_DIR}/example/notify-example.qml)

if(NOTIFY_TESTS)
    enable_testing()
endif()
