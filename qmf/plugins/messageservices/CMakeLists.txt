set(MESSAGESERVICEPLUGINS_DIR "")
if(CLICK_MODE)
    set(MESSAGESERVICEPLUGINS_DIR ${QT_IMPORTS_DIR}/qmf/plugins5/messageservices)
endif()

add_subdirectory(imap)
add_subdirectory(pop)
add_subdirectory(qmfsettings)
add_subdirectory(smtp)
