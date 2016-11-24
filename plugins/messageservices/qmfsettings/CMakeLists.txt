add_definitions(-DQMF_NO_MESSAGE_SERVICE_EDITOR)
if(ENABLE_LOGGING)
    add_definitions(-DQMF_ENABLE_LOGGING)
endif()

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QMFCLIENT_DIR}
    ${QMFCLIENT_DIR}/support
    ${QMFSERVER_DIR}
)

set(libQMFSETTINGS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/service.cpp
)

add_library(qmfsettings SHARED ${libQMFSETTINGS_SOURCES})
qt5_use_modules(qmfsettings Core Network)
target_link_libraries(qmfsettings qmfmessageserver5 qmfclient5)

install(TARGETS qmfsettings DESTINATION ${MESSAGESERVICEPLUGINS_DIR})
