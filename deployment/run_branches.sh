#!/usr/bin/env bash

declare -a arr=(

#		"init-NRN.yml"
#		"X07-Xh04-Xn02-NRN.yml"
#		"X07-Xh02-Xn02-NRN.yml"
#		"X07-Xh02-Xn0-NRN.yml"
#		"X05-Xh04-Xn04-NRN.yml"
#		"X05-Xh02-Xn04-NRN.yml"
#		"K2-NRN.yml"
#		"K2-Xn02-NRN.yml"
#		"K2-Xh02-Xn02-NRN.yml"
#		"K2-Xh02-Xn0-NRN.yml"
#		"K2-X05-NRN.yml"
#		"K2-X05-Xh02-NRN.yml"
#		"bias10-NRN.yml"
#		"bias10-Xn02-NRN.yml"
#		"bias10-Xh02-Xn02-NRN.yml"
#		"bias10-Xh02-Xn0-NRN.yml"
#		"bias10-X05-NRN.yml"
#		"bias10-X05-Xh02-NRN.yml"
#		"bias10-X05-Xh02-Xn02-NRN.yml"
#		"bias10-K2-NRN.yml"
#		"bias10-K2-Xn02-NRN.yml"
#		"bias10-K2-Xh02-Xn02-NRN.yml"
#		"bias10-K2-Xh02-Xn0-NRN.yml"
#		"bias10-K2-X05-NRN.yml"
#		"bias10-K2-X05-Xh02-NRN.yml"
#		"bias10-K2-X05-Xh02-Xn02-NRN.yml"
#
#		"init-RN.yml"
#		"X07-Xh04-Xn02-RN.yml"
#		"X07-Xh02-Xn02-RN.yml"
#		"X07-Xh02-Xn0-RN.yml"
#		"X05-Xh04-Xn04-RN.yml"
#		"X05-Xh02-Xn04-RN.yml"
#		"K2-RN.yml"
#		"K2-Xn02-RN.yml"
#		"K2-Xh02-Xn02-RN.yml"
#		"K2-Xh02-Xn0-RN.yml"
#		"K2-X05-RN.yml"
#		"K2-X05-Xh02-RN.yml"
#		"bias10-RN.yml"
#		"bias10-Xn02-RN.yml"
#		"bias10-Xh02-Xn02-RN.yml"
#		"bias10-Xh02-Xn0-RN.yml"
#		"bias10-X05-RN.yml"
#		"bias10-X05-Xh02-RN.yml"
#		"bias10-X05-Xh02-Xn02-RN.yml"
#		"bias10-K2-RN.yml"
#		"bias10-K2-Xn02-RN.yml"
#		"bias10-K2-Xh02-Xn02-RN.yml"
#		"bias10-K2-Xh02-Xn0-RN.yml"
#		"bias10-K2-X05-RN.yml"
#		"bias10-K2-X05-Xh02-RN.yml"
#		"bias10-K2-X05-Xh02-Xn02-RN.yml"

    "bias1.yml"
    "bias1-RN.yml"
    "bias2.yml"
    "bias2-RN.yml"
    "bias3.yml"
    "bias3-RN.yml"
    "bias5.yml"
    "bias5-RN.yml"
    "bias7.yml"
    "bias7-RN.yml"
    "bias10.yml"
    "bias10-RN.yml"
    "bias10-X06-NRN.yml"
    "bias10-X06-RN.yml"
    "RN-Dispersal.yml"
    "RN-Help.yml"
    "X06-NRN.yml"
    "X06-RN.yml"
    "X06-Xh06-Xn06-NRN.yml"
    "X06-Xh06-Xn06-RN.yml"

		)



for i in "${arr[@]}"
do
echo $i
    screen  -d -S ${i} -m ./build.sh ${i}

done

screen -list

