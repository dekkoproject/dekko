find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Quick 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfclient/support
    ${CMAKE_SOURCE_DIR}/qmf/libraries/qmfmessageserver
    ${CMAKE_SOURCE_DIR}/plugins/shared/src
    ${CMAKE_SOURCE_DIR}/lib/accounts
    ${CMAKE_SOURCE_DIR}/lib/settings
)

set(libMail_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/Folder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FolderList.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MessageFilterCollection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MessageSet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Message.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MessageList.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MailAddress.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ClientService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ClientServiceAction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MailboxSearch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SearchService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Attachments.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SenderIdentities.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MessageBuilder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SubmissionManager.cpp
)

add_library(mail SHARED ${libMail_SRCS})

qt5_use_modules(mail Core Quick)
target_link_libraries(mail qmfclient5 qmfmessageserver5 dekkohelpers accounts settings)

install(TARGETS mail DESTINATION ${LIB_DIR})
