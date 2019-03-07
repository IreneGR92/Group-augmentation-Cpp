#!/usr/bin/env bash


declare -a arr=("supercomputer_dispersal1_NRN"
		"supercomputer_X05_Xh02_Xn04_NRN"
		"supercomputer_X05_Xh03_Xn05"
"supercomputer_low_relatedness_NRN"
"supercomputer_low_relatedness2"
"supercomputer_X06_Xh02_Xn04_NRN"
"supercomputer_bias10_NRN"
"supercomputer_K05_NRN"
"supercomputer_X05_K2_NRN"
"supercomputer_dispersal005_NRN"
"supercomputer_K2_NRN"
"supercomputer_X07_Xh02_Xn0_NRN"
"supercomputer_X_Xh_Xn_NRN"
"supercomputer_init"
"supercomputer_X5_bias10_NRN"
"supercomputer_X05_K05_NRN"
"supercomputer_supercomputer_X07_Xh04_Xn02_NRN")

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


