cmake_minimum_required (VERSION 3.15.0)

add_executable (void_shell_embedded_demo main.c startup.c)

set_target_properties(void_shell_embedded_demo PROPERTIES OUTPUT_NAME void_shell_demo.elf)

target_link_options(void_shell_embedded_demo PUBLIC
-Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/link.ld -ereset_handler --specs=nosys.specs
)

target_link_libraries(void_shell_embedded_demo PUBLIC void_shell rtt)