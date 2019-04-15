#!/usr/bin/env bash


declare -a arr=("sc-init"
		"sc-X07-Xh04-Xn02"
		"sc-X07-Xh02-Xn02"
		"sc-X07-Xh02-Xn0"
		"sc-X05-Xh04-Xn04"
		"sc-X05-Xh02-Xn04"
		"sc-K2"
		"sc-K2-Xn02"
		"sc-K2-Xh02-Xn02"
		"sc-K2-Xh02-Xn0"
		"sc-K2-X05"
		"sc-K2-X05-Xh02"
		"sc-bias10"
		"sc-bias10-Xn02"
		"sc-bias10-Xh02-Xn02"
		"sc-bias10-Xh02-Xn0"
		"sc-bias10-X05"
		"sc-bias10-X05-Xh02"
		"sc-bias10-X05-Xh02-Xn02"
		"sc-bias10-K2"
		"sc-bias10-K2-Xn02"
		"sc-bias10-K2-Xh02-Xn02"
		"sc-bias10-K2-Xh02-Xn0"
		"sc-bias10-K2-X05"
		"sc-bias10-K2-X05-Xh02"
		"sc-bias10-K2-X05-Xh02-Xn02")

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


