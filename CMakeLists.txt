cmake_minimum_required(VERSION 3.15.0)

project(VOID_SHELL LANGUAGES C)

add_subdirectory (third_party)

add_subdirectory(source)
message(STATUS ${CMAKE_C_COMPILER_ID} )
if(CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL
                                             "AppleClang")
  message(STATUS "Desktop Compiler" )
  add_subdirectory(pc_demo)
else()
  message(STATUS "Embedded Compiler" )
  add_subdirectory(embedded_demo)
endif()
