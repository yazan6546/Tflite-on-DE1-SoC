#!/bin/bash
BUILD_TYPE=$1
REMOTE_DIR="/home/knat/tflite"

if [ -z "$BUILD_TYPE" ]; then
  BUILD_TYPE="Release"
fi

if [ $BUILD_TYPE != "Release" ] && [ $BUILD_TYPE != "Debug" ]; then
  echo "Invalid build type. Use 'Release' or 'Debug'."
  exit 1
fi

if [ "$BUILD_TYPE"=="Debug" ]; then
  scp model/* knat@192.168.1.123:${REMOTE_DIR}/Debug/model
fi

scp cmake-build-${BUILD_TYPE}/tflite_test knat@192.168.1.123:${REMOTE_DIR}/${BUILD_TYPE}/
ssh knat@192.168.1.123 "cd ${REMOTE_DIR} && chmod +x $BUILD_TYPE/tflite_test && ./$BUILD_TYPE/tflite_test"