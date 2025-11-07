cmake_minimum_required(VERSION 3.10)
project(TFLiteTest CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Executables
add_executable(tflite_test tflite.cpp)
add_executable(tflite_yolo yolo_inference.cpp)

# === Main TensorFlow Lite core ===
set(TFLITE_LIB ${CMAKE_SOURCE_DIR}/lib/libtensorflow-lite.a)

# === All dependency archives (order matters) ===
set(TFLITE_DEPS
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
        ${CMAKE_SOURCE_DIR}/lib/libpthreadpool.a
        ${CMAKE_SOURCE_DIR}/lib/_deps/cpuinfo-build/libcpuinfo.a
        ${CMAKE_SOURCE_DIR}/lib/_deps/cpuinfo-build/libcpuinfo_internals.a
        ${CMAKE_SOURCE_DIR}/lib/_deps/farmhash-build/libfarmhash.a
)


foreach (target tflite_test tflite_yolo)
    target_link_libraries(${target} PRIVATE ${TFLITE_LIB} ${TFLITE_DEPS} m pthread rt dl)
    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/include)
endforeach()
