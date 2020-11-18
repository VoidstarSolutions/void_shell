

add_executable (void_shell_embedded_demo main.c startup.c)

set_target_properties(void_shell_embedded_demo PROPERTIES OUTPUT_NAME void_shell_demo.elf)

target_compile_options(void_shell_embedded_demo PUBLIC 
-mcpu=cortex-m0plus
-mtune=cortex-m0plus
-fdata-sections
-ffunction-sections
-fno-builtin
-flto
-O3
)

target_link_options(void_shell_embedded_demo PUBLIC
-Wl,-e_start
-mcpu=cortex-m0plus
-mtune=cortex-m0plus
--specs=nosys.specs
-mthumb
-fno-builtin
-fdata-sections
-ffunction-sections
-Xlinker -Map=output.map
-Wl,-flto
-Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/link.ld
-O
)

target_link_libraries (void_shell_embedded_demo LINK_PUBLIC void_shell printf rtt)
