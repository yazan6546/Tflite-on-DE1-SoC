set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(TOOLCHAIN_PREFIX arm-linux-gnueabihf-)
find_program(BINUTILS_PATH ${TOOLCHAIN_PREFIX}gcc NO_CACHE)

if (NOT BINUTILS_PATH)
    message(FATAL_ERROR "ARM GCC toolchain not found")
endif ()

get_filename_component(ARM_TOOLCHAIN_DIR ${BINUTILS_PATH} DIRECTORY)
# Without that flag CMake is not able to pass test compilation check
if (${CMAKE_VERSION} VERSION_EQUAL "3.6.0" OR ${CMAKE_VERSION} VERSION_GREATER "3.6")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
else ()
    set(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nosys.specs")
endif ()

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_AR ${TOOLCHAIN_PREFIX}gcc-ar)
set(CMAKE_RANLIB ${TOOLCHAIN_PREFIX}gcc-ranlib)

execute_process(COMMAND ${CMAKE_C_COMPILER} -print-sysroot
    OUTPUT_VARIABLE ARM_GCC_SYSROOT OUTPUT_STRIP_TRAILING_WHITESPACE)


# set coommon compilation flags for DE1-SoC
# Cortex-A9, ARMv7-A, NEON + VFPv3, hard float ABI, optimization, PIC
set(COMMON_FLAGS "-march=armv7-a -mtune=cortex-a9 -mfpu=neon -mfloat-abi=hard -O3 -fPIC")

# 4. Set flags for C and C++ compilers
set(CMAKE_C_FLAGS   "${COMMON_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++14" CACHE STRING "" FORCE)

set(CMAKE_FIND_ROOT_PATH ${BINUTILS_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


# 10. Messages
message(STATUS "DE1-SoC ARMv7 toolchain configured")
message(STATUS "Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "CFLAGS: ${CMAKE_C_FLAGS}")
message(STATUS "Sysroot: ${CMAKE_SYSROOT}")
