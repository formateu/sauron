#!/bin/bash

#Author Mateusz Forc

#build
mkdir build
cd build
cmake ..
make -j8

#unit test
echo "Running unit tests..."
./test/unit_tests/sauron_unit_test

#integration test
echo "Running integration tests..."
./test/integration_tests/sauron_integration_test

cd ../test/acceptance
./run.sh
