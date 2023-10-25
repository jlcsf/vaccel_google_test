#!/bin/bash

rm ./code_coverage_google/ -rf
lcov --directory ./build/ --capture --output-file ./code_coverage.info 
lcov --remove ./code_coverage.info '/usr/include/c++/12/*' '/usr/include/x86_64-linux-gnu/c++/12/*' 'build/googletest-src/googletest/include/gtest/*' "*/build/*" -o ./code_coverage_filtered.info
genhtml code_coverage_filtered.info  --output-directory ./code_coverage_google/

mv ./code_coverage_google/* ../coverage_gtest/

rm code_coverage.info
rm code_coverage_filtered.info