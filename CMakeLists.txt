cmake_minimum_required(VERSION 3.10)
project(kalender_project C)

set(CMAKE_C_STANDARD 11)

add_executable(kalender
        workbench/calender.c
        workbench/userinput.c
        workbench/workbench.c
        workbench/fileIO.c
        workbench/main.c
)
