#!/usr/bin/env bash


declare -a arr=("supercomputer_X07_Xh02_Xn0"
                "supercomputer_X07_Xh04_Xn02"
                "supercomputer_X06_Xh02_Xn04"
                "supercomputer_X06"
                "supercomputer_X05")
cd ../../
for i in "${arr[@]}"
do
    echo deleting folder $i
    rm -rf $i
    pwd
    echo cloning "$i"
    pwd
    git clone -b $i --single-branch git@github.com:IreneGR92/Group-augmentation-RN.git $i
    cd $i
    pwd
    git checkout $i
    echo running "$i"
    cd deployment
    chmod +x build.sh
    pwd
    screen -d -S "$i" -m ./build.sh
    cd ../..
    pwd
done

screen -list


