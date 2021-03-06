cmake_minimum_required (VERSION 3.15.0)


#Printf
add_library (printf printf/printf.c printf/printf.h)
target_include_directories (printf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/printf)

if(EMBEDDED_COMPILER)
# RTT
add_library (rtt RTT/SEGGER_RTT.c)
target_include_directories ( rtt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/RTT)
endif()