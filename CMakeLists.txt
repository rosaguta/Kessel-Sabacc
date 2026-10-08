cmake_minimum_required(VERSION 4.3)

project(Kessel_Sabacc)

set(CMAKE_CXX_STANDARD 14)

add_executable(Kessel_Sabacc
        src/main.cpp
        src/Dealer.cpp
        src/Player.cpp
        src/Game.cpp
        include/Game.h
)

target_include_directories(Kessel_Sabacc PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)