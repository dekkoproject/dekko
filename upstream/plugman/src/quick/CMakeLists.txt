include_directories(
    ${PLUGIN_LIB_INC}
)

set(pluggerQuick_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/QuickPlugger.cpp
)

add_library(quick-plugger MODULE ${pluggerQuick_SRCS})
qt5_use_modules(quick-plugger Core Quick)
target_link_libraries(quick-plugger dekko-plugins)


set(PLUGGER_DIR ${QT_IMPORTS_DIR}/Plugger)

install(TARGETS quick-plugger DESTINATION ${PLUGGER_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qmldir DESTINATION ${PLUGGER_DIR})
