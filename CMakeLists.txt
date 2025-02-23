cmake_minimum_required(VERSION 3.29)
project(mini_lsm)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_INSTALL_PREFIX)
    message(STATUS "CMAKE_INSTALL_PREFIX has been set as " ${CMAKE_INSTALL_PREFIX})
elseif (DEFINED ENV{CMAKE_INSTALL_PREFIX})
    set(CMAKE_INSTALL_PREFIX $ENV{CMAKE_INSTALL_PREFIX})
else ()
    set(CMAKE_INSTALL_PREFIX /tmp/${PROJECT_NAME})
endif ()
message(STATUS "Install target dir is " ${CMAKE_INSTALL_PREFIX})

set(CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/deps/3rd/usr/local;/usr/local;${CMAKE_PREFIX_PATH}")

if (EXISTS /usr/local/lib)
    link_directories(/usr/local/lib)
endif ()
if (EXISTS /usr/local/lib64)
    link_directories(/usr/local/lib64)
endif ()
if (EXISTS ${PROJECT_SOURCE_DIR}/deps/3rd/usr/local/lib)
    link_directories(${PROJECT_SOURCE_DIR}/deps/3rd/usr/local/lib)
endif ()

include_directories(. ${PROJECT_SOURCE_DIR}/deps ${PROJECT_SOURCE_DIR}/deps/3rd/usr/local/include /usr/local/include)

add_subdirectory(deps)
add_subdirectory(test)
add_subdirectory(src)

enable_testing()
