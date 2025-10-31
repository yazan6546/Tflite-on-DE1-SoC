# Install script for directory: /workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE FILE FILES
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/AdolcForward"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/AlignedVector3"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/ArpackSupport"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/AutoDiff"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/BVH"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/EulerAngles"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/FFT"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/IterativeSolvers"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/KroneckerProduct"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/LevenbergMarquardt"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/MatrixFunctions"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/MoreVectorization"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/MPRealSupport"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/NNLS"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/NonLinearOptimization"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/NumericalDiff"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/OpenGLSupport"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/Polynomials"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/Skyline"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/SparseExtra"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/SpecialFunctions"
    "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/Splines"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE DIRECTORY FILES "/workspace/tensorflow/tensorflow/lite/build/eigen/unsupported/Eigen/src" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/workspace/tensorflow/tensorflow/lite/build/_deps/eigen-build/unsupported/Eigen/CXX11/cmake_install.cmake")

endif()

