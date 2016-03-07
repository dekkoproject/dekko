set(CONTENTMANAGERPLUGINS_DIR "")
if(CLICK_MODE)
    set(CONTENTMANAGERPLUGINS_DIR ${QT_IMPORTS_DIR}/qmf/plugins5/contentmanagers)
endif()

add_subdirectory(qmfstoragemanager)
