#!/bin/bash
#
module load CMake/3.10.2-GCCcore-6.4.0

rm -rf build
mkdir build

cd build
cmake ..
make
cd ..

sbatch ubelix.sh