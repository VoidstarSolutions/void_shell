rm -rf build
cmake -DCMAKE_BINARY_DIR=./ -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=MinSizeRel -H./ -B./build -GNinja
cmake --build ./build --config MinSizeRel --target all
