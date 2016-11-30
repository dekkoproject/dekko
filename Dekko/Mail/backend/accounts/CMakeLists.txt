find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QMFCLIENT_INCLUDES}
    ${QMFCLIENT_SUPPORT_INCLUDES}
    ${QMFSERVER_INCLUDES}
    ${SHARED_SRCS}
)

set(libAccount_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/Account.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AccountConfiguration.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Accounts.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AccountValidator.cpp
)


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/usr/lib")

add_library(accounts SHARED ${libAccount_SRCS})
qt5_use_modules(accounts Core Quick)
target_link_libraries(accounts qmfclient5 qmfmessageserver5 dekkohelpers)

install(TARGETS accounts LIBRARY DESTINATION ${LIB_DIR})
