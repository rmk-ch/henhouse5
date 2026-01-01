# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)


# set(BOARD hs5_c071rb)
set(BOARD hs5_wl55jc)
set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR}/../)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(henhouse5)


include(FetchContent)
FetchContent_Declare(
    ecpp_static_vector
    GIT_REPOSITORY https://github.com/jmacheta/static_vector.git
    GIT_TAG main
    )
FetchContent_MakeAvailable(ecpp_static_vector)
# target_link_libraries(app PUBLIC ecpp_static_vector)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/_deps/ecpp_static_vector-src/include)

include_directories(${CMAKE_CURRENT_LIST_DIR}/includes)

target_sources(app PRIVATE
    src/errorcode.cpp
    src/thread.cpp
    src/outputpin.cpp
    src/inputpin.cpp
    src/pwm.cpp
    src/doorstate.cpp
    src/doorcontrol.cpp
    src/motor.cpp
    src/main.cpp
    )
