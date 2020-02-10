#!/bin/bash

# You must specify a valid email address!
#SBATCH --mail-user=ig17c521@campus.unibe.ch

# Mail on NONE, BEGIN, END, FAIL, REQUEUE, ALL
#SBATCH --mail-type=end,fail

# Job name
#SBATCH --job-name="GroupAugmentation"

# Runtime and memory
#SBATCH --time=00:00:10
#SBATCH --mem-per-cpu=2G
#SBATCH --cpus-per-task=1

# Partition
#SBATCH --partition=debug

# For parallel jobs

##SBATCH --nodes=5
##SBATCH --ntasks=5
##SBATCH --ntasks-per-node=1


##SBATCH --output=/path/to/outfile
##SBATCH --error=/path/to/errfile

# For array jobs
# Array job containing 100 tasks, run max 10 tasks at the same time
#SBATCH --array=0-68

#### Your shell commands below this line ####

declare -a arr=(

#   LOGISTIC SURVIVAL DEFAULT  #

    "Logistic_formula/bias1.yml"
    "Logistic_formula/bias1-RN.yml"
    "Logistic_formula/bias2.yml"
    "Logistic_formula/bias2-RN.yml"
    "Logistic_formula/bias3.yml"
    "Logistic_formula/bias3-RN.yml"
    "Logistic_formula/bias4.yml"
    "Logistic_formula/bias4-RN.yml"
    "Logistic_formula/bias1-m01.yml"
    "Logistic_formula/bias1-RN-m01.yml"
    "Logistic_formula/bias2-m01.yml"
    "Logistic_formula/bias2-RN-m01.yml"
    "Logistic_formula/bias3-m01.yml"
    "Logistic_formula/bias3-RN-m01.yml"
    "Logistic_formula/bias4-m01.yml"
    "Logistic_formula/bias4-RN-m01.yml"
    "Logistic_formula/bias1-m03.yml"
    "Logistic_formula/bias1-RN-m03.yml"
    "Logistic_formula/bias2-m03.yml"
    "Logistic_formula/bias2-RN-m03.yml"
    "Logistic_formula/bias3-m03.yml"
    "Logistic_formula/bias3-RN-m03.yml"
    "Logistic_formula/bias4-m03.yml"
    "Logistic_formula/bias4-RN-m03.yml"
    "Logistic_formula/K2.yml"
    "Logistic_formula/K2-RN.yml"
    "Logistic_formula/Xh2.yml"
    "Logistic_formula/Xh2-RN.yml"
    "Logistic_formula/Xn1.yml"
    "Logistic_formula/Xn1-RN.yml"
    "Logistic_formula/Xn0-NRN.yml"
    "Logistic_formula/Xn0-RN.yml"
    "Logistic_formula/RN-help.yml"
    "Logistic_formula/RN-dispersal.yml"



#   NO GROUP AUGMENTATION  #

    "noGA/bias1-NoGA.yml"
    "noGA/bias1-RN-NoGA.yml"
    "noGA/bias2-NoGA.yml"
    "noGA/bias2-RN-NoGA.yml"
    "noGA/bias3-NoGA.yml"
    "noGA/bias3-RN-NoGA.yml"
    "noGA/bias4-NoGA.yml"
    "noGA/bias4-RN-NoGA.yml"
    "noGA/bias1-m01-NoGA.yml"
    "noGA/bias1-RN-m01-NoGA.yml"
    "noGA/bias2-m01-NoGA.yml"
    "noGA/bias2-RN-m01-NoGA.yml"
    "noGA/bias3-m01-NoGA.yml"
    "noGA/bias3-RN-m01-NoGA.yml"
    "noGA/bias4-m01-NoGA.yml"
    "noGA/bias4-RN-m01-NoGA.yml"
    "noGA/bias1-m03-NoGA.yml"
    "noGA/bias2-m03-NoGA.yml"
    "noGA/bias2-RN-m03-NoGA.yml"
    "noGA/bias3-m03-NoGA.yml"
    "noGA/bias3-RN-m03-NoGA.yml"
    "noGA/bias4-m03-NoGA.yml"
    "noGA/bias4-RN-m03-NoGA.yml"
    "noGA/K2-NoGA.yml"
    "noGA/K2-RN-NoGA.yml"
    "noGA/Xh2-NoGA.yml"
    "noGA/Xh2-RN-NoGA.yml"
    "noGA/Xn1-NoGA.yml"
    "noGA/Xn1-RN-NoGA.yml"


#   LOW SURVIVAL FLOATERS  #

    "LSF/LSF-n1-NRN.yml"
    "LSF/LSF-n2-NRN.yml"
    "LSF/LSF-n3-NRN.yml"
    "LSF/LSF-n1-RN.yml"
    "LSF/LSF-n2-RN.yml"
    "LSF/LSF-n3-RN.yml"

		)

srun ./build/App ".parameters/${arr[$SLURM_ARRAY_TASK_ID]}"
