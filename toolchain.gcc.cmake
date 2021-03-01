# ATSAM toolchain for clang
# Toolchain files can be read multiple times during the project.
# https://www.microchip.com/wwwproducts/en/ATSAMV70Q20
set( CMAKE_SYSTEM_NAME ARM )

# From the spec sheet above
set( CMAKE_SYSTEM_PROCESSOR cortex-m0+ )

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# GCC target triple
SET(TARGET arm-arm-none-eabi)

# specify the cross compiler
SET(CMAKE_C_COMPILER_TARGET ${TARGET})
SET(CMAKE_C_COMPILER arm-none-eabi-gcc)
SET(CMAKE_ASM_COMPILER_TARGET ${TARGET})
SET(CMAKE_ASM_COMPILER arm-none-eabi-as)
