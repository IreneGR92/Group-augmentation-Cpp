#!/usr/bin/env bash


declare -a arr=("supercomputer_conf_NRN_m02"
                "supercomputer_conf_dispersal003"
                "supercomputer_config_NRN_K1-2"
                )


for i in "${arr[@]}"
do
   echo "$i"
   echo ../../$i/deployment/build.sh
   screen -d -S "$i" -m ../../$i/deployment/build.sh
done

screen -list


