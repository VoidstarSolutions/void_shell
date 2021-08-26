cmake_minimum_required(VERSION 3.20.0)

add_library(void_shell INTERFACE)
target_sources(
  void_shell PUBLIC void_command.c void_command.h void_shell.c void_shell.h
                    void_shell_utilities.c void_shell_utilities.h)

target_include_directories(void_shell INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(void_shell INTERFACE printf)
