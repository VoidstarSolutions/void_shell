# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 3.15.0)
project (VOID_SHELL)

add_subdirectory (third_party)
add_subdirectory (source)
add_subdirectory (demo)