add_library (printf printf/printf.c)

target_include_directories (printf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/printf)