cmake_minimum_required(VERSION 3.15.0)

add_library(void_shell STATIC void_command.c void_command.h void_shell.c void_shell.h
                       void_shell_utilities.c void_shell_utilities.h)

target_include_directories(void_shell PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(void_shell PUBLIC printf)
