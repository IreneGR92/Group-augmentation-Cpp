#!/usr/bin/env bash
git pull
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
cp group_augmentation*.txt ../../results/results/
cd ../../results/
echo "git stuff"
git add .
git commit -m "autocommit results"
git push

echo "done"