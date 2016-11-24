include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/support
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/uoa
)

set(lib_BEARERSOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/UOABearerPlugin.cpp
)

add_library(uoa-bearer SHARED ${lib_BEARERSOURCES})
qt5_use_modules(uoa-bearer Core)
target_link_libraries(uoa-bearer qmfclient5 accounts-qt5 signon-qt5)

install(TARGETS uoa-bearer DESTINATION ${UOASERVICEPLUGINS_DIR})
