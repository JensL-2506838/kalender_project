cmake_minimum_required(VERSION 3.10)
project(kalender_project C)

set(CMAKE_C_STANDARD 11)

add_executable(kalender
        workbench/calendar.c
        workbench/userInput.c
        workbench/workbench.c
        workbench/fileIO.c
        workbench/main.c
)
