mkdir -p build && cd build
cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=../arm_gcc_toolchain.cmake ..
