add_definitions(-DQMF_NO_MESSAGE_SERVICE_EDITOR -DPLUGIN_INTERNAL)
if(ENABLE_LOGGING)
    add_definitions(-DQMF_ENABLE_LOGGING)
endif()

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QMFCLIENT_DIR}
    ${QMFCLIENT_DIR}/support
)

set(libQMFSTORAGE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/qmfstoragemanager.cpp
)

add_library(qmfstoragemanager SHARED ${libQMFSTORAGE_SOURCES})
qt5_use_modules(qmfstoragemanager Core)
target_link_libraries(qmfstoragemanager qmfclient5)

install(TARGETS qmfstoragemanager DESTINATION ${CONTENTMANAGERPLUGINS_DIR})
