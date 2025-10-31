cmake_minimum_required(VERSION 3.10)
project(TFLiteTest CXX)

# Use your existing cross-compilation toolchain
# (pass via command line: -DCMAKE_TOOLCHAIN_FILE=../armv7-de1-toolchain.cmake)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(TF_SRC_DIR "C:/Users/yazan/Documents/tflite-compile/tensorflow")

# Add your source file
add_executable(tflite_test tflite.cpp)
target_include_directories(
    tflite_test PUBLIC flatbuffers 
    ${TF_SRC_DIR}
    
)
# Link static libraries (use full paths)
# Order matters: libraries must come AFTER the libraries that depend on them
target_link_libraries(tflite_test
    ${CMAKE_SOURCE_DIR}/lib/libtensorflow-lite.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/flatbuffers-build/libflatbuffers.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/fft2d-build/libfft2d_fftsg.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/fft2d-build/libfft2d_fftsg2d.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/fft2d-build/libfft2d_alloc.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/fft2d-build/libfft2d_fft4f2d.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/fft2d-build/libfft2d_fftsg3d.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/fft2d-build/libfft2d_shrtdct.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_frontend.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_trmul.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_thread_pool.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_blocking_counter.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_block_map.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_context.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_context_get_ctx.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_ctx.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_cpuinfo.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_denormal.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_allocator.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_apply_multiplier.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_have_built_path_for_avx.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_have_built_path_for_avx2_fma.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_have_built_path_for_avx512.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_kernel_arm.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_kernel_avx.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_kernel_avx2_fma.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_kernel_avx512.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_pack_arm.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_pack_avx.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_pack_avx2_fma.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_pack_avx512.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_prepacked_cache.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_prepare_packed_matrices.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_system_aligned_alloc.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_tune.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/ruy-build/ruy/libruy_wait.a
    C:/Users/yazan/Documents/tflite-compile/tflite-build/pthreadpool/libpthreadpool.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/cpuinfo-build/libcpuinfo.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/cpuinfo-build/libcpuinfo_internals.a
    ${CMAKE_SOURCE_DIR}/lib/_deps/farmhash-build/libfarmhash.a
    pthread
    rt
    dl
)
