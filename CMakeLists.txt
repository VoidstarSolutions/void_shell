cmake_minimum_required(VERSION 3.15.0)

project(VOID_SHELL LANGUAGES C)

add_subdirectory(third_party)

add_subdirectory(source)

if(EMBEDDED_COMPILER)
  message(STATUS "Embedded Compiler")
  add_subdirectory(embedded_demo)
else()
  message(STATUS "Desktop Compiler")
  add_subdirectory(pc_demo)
endif()
