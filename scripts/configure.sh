rm -rf build
cmake -DCMAKE_BINARY_DIR=./ -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=MinSizeRel -DCMAKE_TOOLCHAIN_FILE=toolchain.gcc.cmake -H./ -B./build -GNinja