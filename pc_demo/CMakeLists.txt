

add_executable (void_shell_pc_demo main.c )

set_target_properties(void_shell_pc_demo PROPERTIES OUTPUT_NAME void_shell_demo.elf)


target_link_libraries (void_shell_pc_demo LINK_PUBLIC void_shell printf )
