#!/bin/bash
#
#SBATCH --job-name=test_omp
#SBATCH --output=res_omp.txt
#
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --time=10:00
#SBATCH --mem-per-cpu=100

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
./hello.omp


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