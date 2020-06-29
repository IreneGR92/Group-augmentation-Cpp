#!/usr/bin/env bash


name=${PWD##*/}
echo $@

cd ../cmake-build-release/
./App "../parameters/$@"


sleep 10
echo "done"
