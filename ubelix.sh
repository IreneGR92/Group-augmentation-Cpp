#!/bin/bash
#
module load CMake

rm -rf build
mkdir build

cd build
cmake ..
make


#SBATCH --mail-user=irene.garcia@iee.unibe.ch
#SBATCH --mail-type=end,fail
#SBATCH --job-name=group-augmentation
#
#SBATCH --ntasks=6
#SBATCH --cpus-per-task=1
#SBATCH --time=10:00:00
#SBATCH --mem-per-cpu=2G
#SBATCH --test-only

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

declare -a arr=(

#   LOGISTIC SURVIVAL DEFAULT  #

#    "bias1.yml"
#    "bias1-RN.yml"
#    "bias2.yml"
#    "bias2-RN.yml"
#    "bias3.yml"
#    "bias3-RN.yml"
#    "bias4.yml"
#    "bias4-RN.yml"
#    "bias1-m01.yml"
#    "bias1-RN-m01.yml"
#    "bias2-m01.yml"
#    "bias2-RN-m01.yml"
#    "bias3-m01.yml"
#    "bias3-RN-m01.yml"
#    "bias4-m01.yml"
#    "bias4-RN-m01.yml"
#    "bias1-m03.yml"
#    "bias1-RN-m03.yml"
#    "bias2-m03.yml"
#    "bias2-RN-m03.yml"
#    "bias3-m03.yml"
#    "bias3-RN-m03.yml"
#    "bias4-m03.yml"
#    "bias4-RN-m03.yml"
#    "K2.yml"
#    "K2-RN.yml"
#    "Xh2.yml"
#    "Xh2-RN.yml"
#    "Xn1.yml"
#    "Xn1-RN.yml"
#    "Xn0-NRN.yml"
#    "Xn0-RN.yml"
#    "RN-help.yml"
#    "RN-dispersal.yml"



#   NO GROUP AUGMENTATION  #

#    "bias1-NoGA.yml"
#    "bias1-RN-NoGA.yml"
#    "bias2-NoGA.yml"
#    "bias2-RN-NoGA.yml"
#    "bias3-NoGA.yml"
#    "bias3-RN-NoGA.yml"
#    "bias4-NoGA.yml"
#    "bias4-RN-NoGA.yml"
#    "bias1-m01-NoGA.yml"
#    "bias1-RN-m01-NoGA.yml"
#    "bias2-m01-NoGA.yml"
#    "bias2-RN-m01-NoGA.yml"
#    "bias3-m01-NoGA.yml"
#    "bias3-RN-m01-NoGA.yml"
#    "bias4-m01-NoGA.yml"
#    "bias4-RN-m01-NoGA.yml"
#    "bias1-m03-NoGA.yml"
#    "bias2-m03-NoGA.yml"
#    "bias2-RN-m03-NoGA.yml"
#    "bias3-m03-NoGA.yml"
#    "bias3-RN-m03-NoGA.yml"
#    "bias4-m03-NoGA.yml"
#    "bias4-RN-m03-NoGA.yml"
#    "K2-NoGA.yml"
#    "K2-RN-NoGA.yml"
#    "Xh2-NoGA.yml"
#    "Xh2-RN-NoGA.yml"
#    "Xn1-NoGA.yml"
#    "Xn1-RN-NoGA.yml"


#   LOW SURVIVAL FLOATERS  #

    "LSF-n1-NRN.yml"
    "LSF-n2-NRN.yml"
    "LSF-n3-NRN.yml"
    "LSF-n1-RN.yml"
    "LSF-n2-RN.yml"
    "LSF-n3-RN.yml"


		)


for i in "${arr[@]}"
do

srun ./App ../parameters/${i}
done