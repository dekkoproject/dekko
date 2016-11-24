find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)
find_package(Qt5Test 5.4 REQUIRED)

set(UNIT_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/unit)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${MACE_INCLUDES}
    ${SHARED_SRCS}
    ${UNIT_TEST_DIR}
    ${QUICK_FLUX_SRCS}
    ${MAZDB_SRCS}
)

file(GLOB_RECURSE ADDRESSBOOK_UNIT_TESTS
    ${CMAKE_CURRENT_SOURCE_DIR}/unit
    *.cpp
)

set(addressbookTests_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/runner.cpp
    ${ADDRESSBOOK_UNIT_TESTS}
)

add_definitions(-DADDRESSBOOK_QML_TESTS="${CMAKE_CURRENT_SOURCE_DIR}/qml")

add_executable(addressbook-tests ${addressbookTests_SRCS})
qt5_use_modules(addressbook-tests Core Quick Test)
target_link_libraries(addressbook-tests mace addressbook dekkohelpers mazdb)
