mkdir -p build && cd build
cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=../arm-gcc-toolchain.cmake ..
cmake --build . --config Release
