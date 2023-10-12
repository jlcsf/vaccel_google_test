#!/bin/bash
export CXX=g++
rm build -rf
mkdir -p build
cd build
cmake ../ -DBUILD_PLUGIN_NOOP=ON -DENABLE_TESTS=ON -DBUILD_EXAMPLES=ON
make
make test