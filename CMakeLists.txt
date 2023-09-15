cmake_minimum_required(VERSION 3.5)

project(hydrometrie3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FindPkgConfig)
pkg_check_modules(CURLPP REQUIRED curlpp)

add_executable(hydrometrie3 main.cpp)

install(TARGETS hydrometrie3
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

target_link_libraries(hydrometrie3 gd -lm)
target_link_libraries(hydrometrie3 ${CURLPP_LDFLAGS})
