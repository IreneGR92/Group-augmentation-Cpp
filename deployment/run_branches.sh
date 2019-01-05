#!/usr/bin/env bash


declare -a arr=("supercomputer_conf_NRN_m02"
                "supercomputer_conf_dispersal003"
                "supercomputer_config_NRN_K1-2"
                )
cd ../../
for i in "${arr[@]}"
do
    echo deleting folder $i
    rm -rf $i
    echo cloning "$i"
    git clone git@github.com:IreneGR92/Group-augmentation-RN.git $i
    cd $i
    git checkout $i
    echo running "$i"
    cd deployment
    screen -d -S "$i" -m build.sh
    cd ../..
done

screen -list


