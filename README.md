# TensorFlow Lite on DE1-SoC

Cross-compilation project for running TensorFlow Lite on the DE1-SoC board (ARM Cortex-A9).

## Overview

This project demonstrates how to cross-compile TensorFlow Lite for ARM architecture and run MobileNetV1 image classification on the DE1-SoC FPGA board.

## Hardware Target

- **Board**: Terasic DE1-SoC
- **Processor**: ARM Cortex-A9 (dual-core @ 925 MHz)
- **Architecture**: ARMv7-A with NEON
- **OS**: Ubuntu 18.04 (ARM)
- **SD Card Image**: `c5soc_opencl_lxde_fpga_reconfigurable_20201027.img`
  - Source: [thinkoco/c5soc_opencl](https://github.com/thinkoco/c5soc_opencl)

## Cross-Compilation Environment

- **Host OS**: Windows with Cygwin
- **Development Tools**: Intel Embedded Design Suite (EDS) 20.1
- **Target OS**: Ubuntu 18.04
- **Toolchain**: arm-linux-gnueabihf-gcc 7.5.0 (included in Intel EDS)
- **Build System**: CMake 3.10+ with Ninja
- **C++ Standard**: C++14

> **Note**: This project was developed and tested on Windows using Cygwin and the Intel FPGA Embedded Design Suite 20.1, which includes the ARM cross-compilation toolchain.

## Prerequisites

1. ARM cross-compilation toolchain (arm-linux-gnueabihf-gcc 7.5.0)
2. CMake 3.10 or higher
3. Ninja build system
4. Pre-built TensorFlow Lite static libraries for ARM

## Project Structure

```
.
├── tflite.cpp              # Main inference code
├── CMakeLists.txt          # Build configuration
├── arm-gcc-toolchain.cmake # Cross-compilation toolchain file
├── model.tflite            # MobileNetV1 model
├── lib/                    # Pre-built ARM libraries
│   ├── libtensorflow-lite.a
│   └── _deps/              # Dependencies (cpuinfo, ruy, fft2d, etc.)
├── tensorflow/             # TensorFlow Lite headers
└── flatbuffers/            # FlatBuffers headers
```

## Build Instructions

### 1. Clone the Repository

```bash
git clone https://github.com/yazan6546/Tflite-on-DE1-SoC.git
cd Tflite-on-DE1-SoC
```

### 2. Create Build Directory

```bash
mkdir build
cd build
```

### 3. Configure with CMake

```bash
cmake -G Ninja -DCMAKE_TOOLCHAIN_FILE=../arm-gcc-toolchain.cmake ..
```

### 4. Build

```bash
ninja
```

This will generate the `tflite_test` executable for ARM.

## Deployment

### Transfer Files to DE1-SoC

```bash
scp tflite_test model.tflite user@de1soc-ip:~/
```

### Run on DE1-SoC

```bash
ssh user@de1soc-ip
./tflite_test
```

## Expected Output

```
✅ Model loaded successfully
✅ Interpreter built successfully
✅ Tensors allocated successfully

📊 Model Information:
Input tensor: input_1
Input shape: [1, 224, 224, 3]
Input type: FLOAT32
Output tensor: Identity
Output shape: [1, 1000]
Output type: FLOAT32

🔄 Preparing input data...
✅ Input data prepared (size: 150528)

🚀 Running inference...
✅ Inference completed in ~10000 ms

🎯 Top 5 Predictions:
  Class 78: 10.1562%
  ...

✅ MobileNetV1 inference successful!
```

## Performance

- **Model**: MobileNetV1 (224x224x3 input)
- **Inference Time**: ~10 seconds per image on Cortex-A9
- **Platform**: ARM Cortex-A9 @ 925 MHz (no GPU acceleration)

## Dependencies

All dependencies are statically linked:
- TensorFlow Lite
- FlatBuffers
- cpuinfo
- pthreadpool
- ruy (matrix multiplication)
- fft2d
- farmhash

## Notes

- The current implementation uses dummy input data (neutral gray)
- For real inference, load and preprocess actual images
- ImageNet class labels not included (predictions show class IDs only)

## License

This project uses TensorFlow Lite, which is licensed under Apache 2.0.

## References

- [TensorFlow Lite](https://www.tensorflow.org/lite)
- [DE1-SoC Board](https://www.terasic.com.tw/cgi-bin/page/archive.pl?Language=English&No=836)
