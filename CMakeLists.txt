cmake_minimum_required(VERSION 3.21)
project(dbisLab C)

set(CMAKE_C_STANDARD 23)

add_executable(dbisLab main.c database.c stringoperations.c stringoperations.h fileoperations.c fileoperations.h)
