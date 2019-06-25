#!/usr/bin/env bash


name=${PWD##*/}
echo $@

cd ../bin/
./App "../parameters/$@"


sleep 10
echo "done"
