#!/usr/bin/env bash
git pull
module load CMake
echo "cleaning up build folder"
rm -rf ../build
mkdir ../build
echo "building software"
cd ../build
cmake ..
make
echo "running software"
./app


echo "copy results"
mv group_augmentation_last_generation*.txt ../../results/results/last_generation
mv group_augmentation*.txt ../../results/results/main
cd ../../results/
echo "git stuff"
git add .
git commit -m "autocommit results"
git push

echo "done"
