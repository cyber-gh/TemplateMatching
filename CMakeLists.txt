cmake_minimum_required(VERSION 3.12)
project(ProiectUnversitate C)

set(CMAKE_C_STANDARD 99)

add_executable(ProiectUnversitate main.c utility.h )
target_link_libraries(ProiectUnversitate m)

