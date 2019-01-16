#!/usr/bin/env bash


declare -a arr=("supercomputer_init"

                "supercomputer_m015"

                "supercomputer_bias10"
                "supercomputer_K1-2"
                "supercomputer_Xsh1"
                "supercomputer_Xsn1"
                "supercomputer_Xsn0"
                "supercomputer_dispersal1"
                "supercomputer_NRN"

                "supercomputer_NRN_m015"

                "supercomputer_NRN_bias10"
                "supercomputer_NRN_K1-2"
                "supercomputer_NRN_Xsh1"
                "supercomputer_NRN_Xsn1"
                "supercomputer_NRN_Xsn0"
                "supercomputer_NRN_dispersal1")
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


