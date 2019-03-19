#!/usr/bin/env bash


declare -a arr=("sc_dispersal1_RN-F2"
		"sc_dispersal0_RN-F2"
		"sc_dispersal0_NRN-F2"
		¨sc_dispersal1_NRN-F2¨
		¨sc_helpRN_dispersal1-F2¨
		¨sc_helpRN_dispersal065-F2¨
		¨sc_helpRN_dispersal035-F2¨
		¨sc_helpRN_dispersal0-F2¨
		¨sc_helpNRN_dispersal0-F2¨
		¨sc_helpNRN_dispersal035-F2¨
		¨helpNRN_dispersal065-F2¨
		¨sc_helpNRN_dispersal1-F2¨
		¨sc_helpRN_dispersal1-F1¨
		¨sc_helpRN_dispersal065-F1¨
		¨sc_helpRN_dispersal035-F1¨
		¨sc_helpRN_dispersal0-F1¨
		¨sc_helpNRN_dispersal0-F1¨
		¨sc_helpNRN_dispersal035-F1¨
		¨sc_helpNRN_dispersal1-F1¨
		¨sc_helpNRN_dispersal065-F1¨
		¨sc_dispersion1RN-F1¨
		¨sc_dispersion0RN-F1¨
		¨sc_dispersion0NRN-F1¨
		¨sc_dispersion1NRN-F1¨)

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


