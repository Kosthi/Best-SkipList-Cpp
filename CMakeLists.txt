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

if (EXISTS /usr/local/lib)
    link_directories(/usr/local/lib)
endif ()
if (EXISTS /usr/local/lib64)
    link_directories(/usr/local/lib64)
endif ()

include_directories(. ${PROJECT_SOURCE_DIR}/deps /usr/local/include)

add_subdirectory(deps)
add_subdirectory(test)
add_subdirectory(src)
