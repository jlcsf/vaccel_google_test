#!/bin/bash

export CXX=g++
rm build -rf
mkdir -p build
cd build
cmake ../ -DBUILD_PLUGIN_NOOP=ON -DENABLE_TESTS=ON -DBUILD_EXAMPLES=ON
make
export VACCEL_BACKENDS=../plugins/noop/libvaccel-noop.so
export VACCEL_DEBUG_LEVEL=4
make test

cd ../
sh ./run_code_coverage.sh