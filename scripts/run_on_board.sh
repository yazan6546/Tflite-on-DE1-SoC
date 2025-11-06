#!/bin/bash
BUILD_TYPE=$1

if [ -z "$BUILD_TYPE" ]; then
  BUILD_TYPE="Release"
fi
REMOTE_DIR="/home/knat/tflite"

scp cmake-build-${BUILD_TYPE}/tflite_test knat@192.168.1.123:${REMOTE_DIR}/${BUILD_TYPE}/
ssh knat@192.168.1.123 "cd ${REMOTE_DIR} && chmod +x ./tflite_test && ./$BUILD_TYPE/tflite_test"