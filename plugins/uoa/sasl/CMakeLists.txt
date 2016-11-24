include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/support
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/uoa
)

set(lib_SASLSOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/UOASaslPlugin.cpp
)

add_library(uoa-sasl SHARED ${lib_SASLSOURCES})
qt5_use_modules(uoa-sasl Core)
target_link_libraries(uoa-sasl qmfclient5 accounts-qt5 signon-qt5)

install(TARGETS uoa-sasl DESTINATION ${UOASERVICEPLUGINS_DIR})
