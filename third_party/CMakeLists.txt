# make third_party an include directory
include_directories ( PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#Printf
add_library (printf printf/printf.c)
target_include_directories (printf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/printf)


# RTT
add_library (rtt RTT/SEGGER_RTT.c)
target_include_directories ( rtt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/RTT)