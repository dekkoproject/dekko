project(accounts-plugin-email C CXX)
cmake_minimum_required(VERSION 2.8.9)

set(CMAKE_INSTALL_DATADIR /usr/share/)

add_subdirectory(qml)
add_subdirectory(src)
add_subdirectory(data)
add_subdirectory(example)
add_subdirectory(tests)
