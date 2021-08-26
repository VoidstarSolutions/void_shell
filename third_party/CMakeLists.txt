cmake_minimum_required(VERSION 3.20.0)

# Printf
add_library(printf INTERFACE)
target_sources(printf PUBLIC printf/printf.c printf/printf.h)
target_include_directories(printf INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/printf)

if(EMBEDDED_COMPILER)
  # RTT
  add_library(rtt INTERFACE)
  target_sources(rtt PUBLIC RTT/SEGGER_RTT.c)
  target_include_directories(rtt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/RTT)
endif()
