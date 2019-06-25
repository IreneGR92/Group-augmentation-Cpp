#!/usr/bin/env bash

project_root=~/git/main/
git_root=~/git/

declare -a arr=("init-No_relatedness.yml"
		"07-Xh04-Xn02-No_relatedness.yml"
		"X07-Xh02-Xn02-No_relatedness.yml"
		"X07-Xh02-Xn0-No_relatedness.yml"
		"X05-Xh04-Xn04-No_relatedness.yml"
		"X05-Xh02-Xn04-No_relatedness.yml"
		"K2-No_relatedness.yml"
		"K2-Xn02-No_relatedness.yml"
		"K2-Xh02-Xn02-No_relatedness.yml"
		"K2-Xh02-Xn0-No_relatedness.yml"
		"K2-X05-No_relatedness.yml"
		"K2-X05-Xh02-No_relatedness.yml"
		"bias10-No_relatedness.yml"
		"bias10-Xn02-No_relatedness.yml"
		"bias10-Xh02-Xn02-No_relatedness.yml"
		"bias10-Xh02-Xn0-No_relatedness.yml"
		"bias10-X05-No_relatedness.yml"
		"bias10-X05-Xh02-No_relatedness.yml"
		"bias10-X05-Xh02-Xn02-No_relatedness.yml"
		"bias10-K2-No_relatedness.yml"
		"bias10-K2-Xn02-No_relatedness.yml"
		"bias10-K2-Xh02-Xn02-No_relatedness.yml"
		"bias10-K2-Xh02-Xn0-No_relatedness.yml"
		"bias10-K2-X05-No_relatedness.yml"
		"bias10-K2-X05-Xh02-No_relatedness.yml"
		"bias10-K2-X05-Xh02-Xn02-No_relatedness.yml"

		"init-NRN.yml"
		"X07-Xh04-Xn02-NRN.yml"
		"X07-Xh02-Xn02-NRN.yml"
		"X07-Xh02-Xn0-NRN.yml"
		"X05-Xh04-Xn04-NRN.yml"
		"X05-Xh02-Xn04-NRN.yml"
		"K2-NRN.yml"
		"K2-Xn02-NRN.yml"
		"K2-Xh02-Xn02-NRN.yml"
		"K2-Xh02-Xn0-NRN.yml"
		"K2-X05-NRN.yml"
		"K2-X05-Xh02-NRN.yml"
		"bias10-NRN.yml"
		"bias10-Xn02-NRN.yml"
		"bias10-Xh02-Xn02-NRN.yml"
		"bias10-Xh02-Xn0-NRN.yml"
		"bias10-X05-NRN.yml"
		"bias10-X05-Xh02-NRN.yml"
		"bias10-X05-Xh02-Xn02-NRN.yml"
		"bias10-K2-NRN.yml"
		"bias10-K2-Xn02-NRN.yml"
		"bias10-K2-Xh02-Xn02-NRN.yml"
		"bias10-K2-Xh02-Xn0-NRN.yml"
		"bias10-K2-X05-NRN.yml"
		"bias10-K2-X05-Xh02-NRN.yml"
		"bias10-K2-X05-Xh02-Xn02-NRN.yml"

		"init-RN.yml"
		"X07-Xh04-Xn02-RN.yml"
		"X07-Xh02-Xn02-RN.yml"
		"X07-Xh02-Xn0-RN.yml"
		"X05-Xh04-Xn04-RN.yml"
		"X05-Xh02-Xn04-RN.yml"
		"K2-RN.yml"
		"K2-Xn02-RN.yml"
		"K2-Xh02-Xn02-RN.yml"
		"K2-Xh02-Xn0-RN.yml"
		"K2-X05-RN.yml"
		"K2-X05-Xh02-RN.yml"
		"bias10-RN.yml"
		"bias10-Xn02-RN.yml"
		"bias10-Xh02-Xn02-RN.yml"
		"bias10-Xh02-Xn0-RN.yml"
		"bias10-X05-RN.yml"
		"bias10-X05-Xh02-RN.yml"
		"bias10-X05-Xh02-Xn02-RN.yml"
		"bias10-K2-RN.yml"
		"bias10-K2-Xn02-RN.yml"
		"bias10-K2-Xh02-Xn02-RN.yml"
		"bias10-K2-Xh02-Xn0-RN.yml"
		"bias10-K2-X05-RN.yml"
		"bias10-K2-X05-Xh02-RN.yml"
		"bias10-K2-X05-Xh02-Xn02-RN.yml")


cd ${git_root}

echo "cleaning up old folders"

rm -rf -r auto_*

#echo "deleting old results.."
#
#rm -rf -r ~/git/results
#
#
#echo "deleting results branch"
#
#cd ${project_root}
#git checkout master
#git pull
#git branch -D results
#git push origin --delete results
#
#echo "recreating results branch"
#
#git branch results
#git push --set-upstream origin results
#
#echo "setup folders for results"
#cd ${git_root}
#
#git clone -b results --single-branch git@github.com:IreneGR92/Group-augmentation-RN.git results
#cd ${git_root}/results/
#git checkout results
#mkdir results
#cd results
#mkdir main
#cd main
#touch .gitkeep
#cd ..
#mkdir last_generation
#cd last_generation
#touch .gitkeep
#
#git add .
#
#git commit -m "initial results folder structure"
#git push

cd ${git_root}


for i in "${arr[@]}"
do
folder_name="auto_$i"

srun ./App
done

screen -list


