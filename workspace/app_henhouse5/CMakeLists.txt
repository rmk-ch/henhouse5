# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# set(BOARD hs5_c071rb)
set(BOARD hs5_wl55jc)
set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR}/../)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(henhouse5)

include_directories(${CMAKE_CURRENT_LIST_DIR}/includes)

target_sources(app PRIVATE
    src/errorcode.cpp
    src/outputpin.cpp
    src/button.cpp
    src/pwm.cpp
    src/door.cpp
    src/motor.cpp
    src/main.cpp
    )
