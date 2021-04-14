set( CMAKE_SYSTEM_NAME Generic )

# From the spec sheet above
set( CMAKE_SYSTEM_PROCESSOR arm )

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

set(EMBEDDED_COMPILER "arm-none-eabi-gcc")

# GCC target triple
SET(TARGET arm-arm-none-eabi)

# specify the cross compiler
SET(CMAKE_C_COMPILER_TARGET ${TARGET})
SET(CMAKE_C_COMPILER arm-none-eabi-gcc)
SET(CMAKE_ASM_COMPILER_TARGET ${TARGET})
SET(CMAKE_ASM_COMPILER arm-none-eabi-as)

string(APPEND CMAKE_C_FLAGS_INIT " -mcpu=cortex-m0plus -mtune=cortex-m0plus -mthumb -Wall -Wextra -Wpedantic -Werror")