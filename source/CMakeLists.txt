add_library (void_shell void_command.c void_shell.c void_shell_utilities.c)

target_include_directories (void_shell PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})