cmake_minimum_required(VERSION 3.20)
project(dbisLab C)

set(CMAKE_C_STANDARD 11)

add_executable(dbisLab main.c database.c stringoperations.c stringoperations.h fileoperations.c fileoperations.h)
