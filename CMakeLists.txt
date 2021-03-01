cmake_minimum_required(VERSION 3.15.0)

set(CMAKE_SYSTEM_NAME ARM)
# Version of the target system
set(CMAKE_SYSTEM_VERSION any)
# From the spec sheet above
set(CMAKE_SYSTEM_PROCESSOR cortex-m0+)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")


project(VOID_SHELL LANGUAGES C)

add_subdirectory (third_party)

add_subdirectory(source)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL
                                             "AppleClang")
  add_subdirectory(pc_demo)
else()
  add_subdirectory(embedded_demo)
endif()
