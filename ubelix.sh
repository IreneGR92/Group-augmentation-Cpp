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

    "bias1.yml"
    "bias1-RN.yml"
    "bias2.yml"
    "bias2-RN.yml"
    "bias3.yml"
    "bias3-RN.yml"
    "bias4.yml"
    "bias4-RN.yml"
    "bias1-m01.yml"
    "bias1-RN-m01.yml"
    "bias2-m01.yml"
    "bias2-RN-m01.yml"
    "bias3-m01.yml"
    "bias3-RN-m01.yml"
    "bias4-m01.yml"
    "bias4-RN-m01.yml"
    "bias1-m03.yml"
    "bias1-RN-m03.yml"
    "bias2-m03.yml"
    "bias2-RN-m03.yml"
    "bias3-m03.yml"
    "bias3-RN-m03.yml"
    "bias4-m03.yml"
    "bias4-RN-m03.yml"
    "K2.yml"
    "K2-RN.yml"
    "Xh2.yml"
    "Xh2-RN.yml"
    "Xn1.yml"
    "Xn1-RN.yml"
    "Xn0-NRN.yml"
    "Xn0-RN.yml"
    "RN-help.yml"
    "RN-dispersal.yml"



#   NO GROUP AUGMENTATION  #

    "bias1-NoGA.yml"
    "bias1-RN-NoGA.yml"
    "bias2-NoGA.yml"
    "bias2-RN-NoGA.yml"
    "bias3-NoGA.yml"
    "bias3-RN-NoGA.yml"
    "bias4-NoGA.yml"
    "bias4-RN-NoGA.yml"
    "bias1-m01-NoGA.yml"
    "bias1-RN-m01-NoGA.yml"
    "bias2-m01-NoGA.yml"
    "bias2-RN-m01-NoGA.yml"
    "bias3-m01-NoGA.yml"
    "bias3-RN-m01-NoGA.yml"
    "bias4-m01-NoGA.yml"
    "bias4-RN-m01-NoGA.yml"
    "bias1-m03-NoGA.yml"
    "bias2-m03-NoGA.yml"
    "bias2-RN-m03-NoGA.yml"
    "bias3-m03-NoGA.yml"
    "bias3-RN-m03-NoGA.yml"
    "bias4-m03-NoGA.yml"
    "bias4-RN-m03-NoGA.yml"
    "K2-NoGA.yml"
    "K2-RN-NoGA.yml"
    "Xh2-NoGA.yml"
    "Xh2-RN-NoGA.yml"
    "Xn1-NoGA.yml"
    "Xn1-RN-NoGA.yml"


#   LOW SURVIVAL FLOATERS  #

    "LSF-n1-NRN.yml"
    "LSF-n2-NRN.yml"
    "LSF-n3-NRN.yml"
    "LSF-n1-RN.yml"
    "LSF-n2-RN.yml"
    "LSF-n3-RN.yml"

		)


srun ./build/App ".parameters/${arr[$SLURM_ARRAY_TASK_ID]}"
