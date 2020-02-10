#!/bin/bash
#
module load CMake

rm -rf build
mkdir build

cd build
cmake ..
make
cd ..

sbatch ubelix.sh