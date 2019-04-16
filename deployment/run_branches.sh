#!/usr/bin/env bash

project_root="~/git/main/"
git_root="~/git/"

declare -a arr=("sc-init"
		"sc-X07-Xh04-Xn02"
		)

cd ${git_root}

echo "cleaning up old folders"

rm -rf -r auto_*

echo "deleting old results.."

rm -rf -r ~/git/results


echo "deleting results branch"

cd ${project_root}
git checkout master
git branch -D results
git push origin --delete results

echo "recreating results branch"

git branch results
git push --set-upstream origin results

echo "setup folders for results"
cd ${git_root}

git clone -b results --single-branch git@github.com:IreneGR92/Group-augmentation-RN.git results
cd cd ${git_root}/results/
git checkout results
mkdir results
cd results
mkdir main
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
    git clone -b ${folder_name} --single-branch git@github.com:IreneGR92/Group-augmentation-RN.git ${folder_name}
    cd ${folder_name}
    pwd
    git checkout ${folder_name}
    echo running "${folder_name}"
    cd deployment
    chmod +x build.sh
    pwd
    screen -d -S "${folder_name}" -m ./build.sh
    cd ../..
    pwd
done

screen -list


