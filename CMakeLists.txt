cmake_minimum_required (VERSION 3.15.0)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project (VOID_SHELL)

add_compile_options(
    -Wall 
    -Wextra 
    -pedantic 
    -Werror
    )

add_subdirectory (third_party)

add_subdirectory (source)

message( ${CMAKE_CXX_COMPILER_ID} )
if( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR  
 CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  add_subdirectory ( pc_demo )
else()
  add_subdirectory( embedded_demo )
endif()

