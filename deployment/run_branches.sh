#!/usr/bin/env bash


declare -a arr=("supercomputer_init"
                "supercomputer_X0-0.6_Xsh-0.5_Xsn-0.5"
                "supercomputer_X0-0.5_Xsh-0.5_Xsn-0.5"
                "supercomputer_X0-0.7_Xsh-0.5_Xsn-0.3"
                "supercomputer_X0-0.5_Xsh-0.3_Xsn-0.5"
                "supercomputer_X0-0.7_Xsh-0.1_Xsn-0"
                "supercomputer_bias10"
                "supercomputer_K1-2"
                "supercomputer_K1-2_X0-0.5"
                "supercomputer_K1-0.5"
                "supercomputer_dispersal1"
                "supercomputer_dispersal0")
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


