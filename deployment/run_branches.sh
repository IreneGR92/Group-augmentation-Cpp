#!/usr/bin/env bash

project_root=~/git/main/
git_root=~/git/

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

cd ${git_root}

echo "cleaning up old folders"

rm -rf -r auto_*

echo "deleting old results.."

rm -rf -r ~/git/results


echo "deleting results branch"

cd ${project_root}
git checkout master
git pull
git branch -D results
git push origin --delete results

echo "recreating results branch"

git branch results
git push --set-upstream origin results

echo "setup folders for results"
cd ${git_root}

git clone -b results --single-branch git@github.com:IreneGR92/Group-augmentation-RN.git results
cd ${git_root}/results/
git checkout results
mkdir results
cd results
mkdir main
cd main
touch .gitkeep
cd ..
mkdir last_generation
cd last_generation
touch .gitkeep

git add .

git commit -m "initial results folder structure"
git push

cd ${git_root}


for i in "${arr[@]}"
do
folder_name="auto_$i"

    echo deleting folder ${folder_name}
    rm -rf ${folder_name}
    pwd
    echo cloning "${folder_name}"
    pwd
    git clone -b ${i} --single-branch git@github.com:IreneGR92/Group-augmentation-RN.git ${folder_name}
    cd ${folder_name}
    pwd
    git checkout ${i}
    echo running "${folder_name}"
    cd deployment
    chmod +x build.sh
    pwd
    screen -d -S "${folder_name}" -m ./build.sh
    cd ../..
    pwd
done

screen -list


