find_package(Qt5Core 5.4 REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${SHARED_SRCS}
    ${CMAKE_SOURCE_DIR}/src
)

set(example_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)


add_executable(example ${example_SRCS})
qt5_use_modules(example Core Quick)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/plugins DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Main.qml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(example dekko-plugins)


set(service_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/NotifySendService.cpp
)

add_library(notify-service SHARED ${service_SRCS})
qt5_use_modules(notify-service Core)
target_link_libraries(notify-service dekko-plugins)
set_target_properties(notify-service PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/plugins)


