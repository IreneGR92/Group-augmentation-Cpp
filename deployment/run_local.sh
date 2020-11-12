#!/bin/bash

# You must specify a valid email address!
#SBATCH --mail-user=ig17c521@campus.unibe.ch

# Mail on NONE, BEGIN, END, FAIL, REQUEUE, ALL
#SBATCH --mail-type=end,fail

# Job name
#SBATCH --job-name="GroupAugmentation"

# Runtime and memory
#SBATCH --time=300:00:00
#SBATCH --mem-per-cpu=2G
#SBATCH --cpus-per-task=1

# Partition
#SBATCH --partition=long

# For parallel jobs

##SBATCH --nodes=5
##SBATCH --ntasks=5
##SBATCH --ntasks-per-node=1

##SBATCH --output=/path/to/outfile
##SBATCH --error=/path/to/errfile

# For array jobs
# Indicate how many input files you have
#SBATCH --array=0-5 //TODO: Number of runs, remember to change!

#### Your shell commands below this line ####

declare -a arr=(

  #   new RUNS FOR GRAPH  #
  "bias15-RN-m015.yml"
  "bias15-RN-m01.yml"
  "bias15-RN-m025.yml"
  "bias15-RN-m02.yml"
  "bias15-RN-m03.yml"
  "bias1-RN-m015.yml"
  "bias1-RN-m025.yml"
  "bias25-RN-m015.yml"
  "bias25-RN-m01.yml"
  "bias25-RN-m025.yml"
  "bias25-RN-m02.yml"
  "bias25-RN-m03.yml"
  "bias2-RN-m015.yml"
  "bias2-RN-m025.yml"
  "bias2-RN-m02-dispersal1.yml"
  "bias2-RN-m03-dispersal1.yml"
  "bias35-RN-m015.yml"
  "bias35-RN-m01.yml"
  "bias35-RN-m025.yml"
  "bias35-RN-m02.yml"
  "bias35-RN-m03.yml"
  "bias3-RN-m015.yml"
  "bias3-RN-m025.yml"
  "bias4-RN-m015.yml"
  "bias4-RN-m025.yml"

  #   LOGISTIC SURVIVAL DEFAULT  #

  #    "default/bias1.yml"
  #    "default/bias1-RN.yml"
  #    "default/bias2.yml"
  #    "default/bias2-RN.yml"
  #    "default/bias3.yml"
  #    "default/bias3-RN.yml"
  #    "default/bias4.yml"
  #    "default/bias4-RN.yml"
  #    "default/bias1-m01.yml"
  #    "default/bias1-RN-m01.yml"
  #    "default/bias2-m01.yml"
  #    "default/bias2-RN-m01.yml"
  #    "default/bias3-m01.yml"
  #    "default/bias3-RN-m01.yml"
  #    "default/bias4-m01.yml"
  #    "default/bias4-RN-m01.yml"
  #    "default/bias1-m03.yml"
  #    "default/bias1-RN-m03.yml"
  #    "default/bias2-m03.yml"
  #    "default/bias2-RN-m03.yml"
  #    "default/bias3-m03.yml"
  #    "default/bias3-RN-m03.yml"
  #    "default/bias4-m03.yml"
  #    "default/bias4-RN-m03.yml"
  #    "default/K2.yml"
  #    "default/K2-RN.yml"
  #    "default/K05.yml"
  #    "default/K05-RN.yml"
  #    "default/K02.yml"
  #    "default/K02-RN.yml"
  #    "default/Xh2.yml"
  #    "default/Xh2-RN.yml"
  #    "default/Xn1.yml"
  #    "default/Xn1-RN.yml"
  #    "default/Xn0-NRN.yml"
  #    "default/Xn0-RN.yml"
  #    "default/RN-help.yml"
  #    "default/RN-dispersal.yml"
  #    "default/n2-NRN.yml"
  #    "default/n2-RN.yml"

  #   NO RELATEDNESS    #

  #    "noRelatedness/bias1.yml"
  #    "noRelatedness/bias1-RN.yml"
  #    "noRelatedness/bias2.yml"
  #    "noRelatedness/bias2-RN.yml"
  #    "noRelatedness/bias3.yml"
  #    "noRelatedness/bias3-RN.yml"
  #    "noRelatedness/bias4.yml"
  #    "noRelatedness/bias4-RN.yml"
  #    "noRelatedness/bias1-m01.yml"
  #    "noRelatedness/bias1-RN-m01.yml"
  #    "noRelatedness/bias2-m01.yml"
  #    "noRelatedness/bias2-RN-m01.yml"
  #    "noRelatedness/bias3-m01.yml"
  #    "noRelatedness/bias3-RN-m01.yml"
  #    "noRelatedness/bias4-m01.yml"
  #    "noRelatedness/bias4-RN-m01.yml"
  #    "noRelatedness/bias1-m03.yml"
  #    "noRelatedness/bias1-RN-m03.yml"
  #    "noRelatedness/bias2-m03.yml"
  #    "noRelatedness/bias2-RN-m03.yml"
  #    "noRelatedness/bias3-m03.yml"
  #    "noRelatedness/bias3-RN-m03.yml"
  #    "noRelatedness/bias4-m03.yml"
  #    "noRelatedness/bias4-RN-m03.yml"
  #    "noRelatedness/K2.yml"
  #    "noRelatedness/K2-RN.yml"
  #    "noRelatedness/K05.yml"
  #    "noRelatedness/K05-RN.yml"
  #    "noRelatedness/K02.yml"
  #    "noRelatedness/K02-RN.yml"
  #    "noRelatedness/Xh2.yml"
  #    "noRelatedness/Xh2-RN.yml"
  #    "noRelatedness/Xn1.yml"
  #    "noRelatedness/Xn1-RN.yml"
  #    "noRelatedness/Xn0-NRN.yml"
  #    "noRelatedness/Xn0-RN.yml"
  #    "noRelatedness/RN-help.yml"
  #    "noRelatedness/RN-dispersal.yml"
  #    "noRelatedness/n2-NRN.yml"
  #    "noRelatedness/n2-RN.yml"

  #   NO GROUP AUGMENTATION  #

  #    "noGA/bias1-NoGA.yml"
  #    "noGA/bias1-RN-NoGA.yml"
  #    "noGA/bias2-NoGA.yml"
  #    "noGA/bias2-RN-NoGA.yml"
  #    "noGA/bias3-NoGA.yml"
  #    "noGA/bias3-RN-NoGA.yml"
  #    "noGA/bias4-NoGA.yml"
  #    "noGA/bias4-RN-NoGA.yml"
  #    "noGA/bias1-m01-NoGA.yml"
  #    "noGA/bias1-RN-m01-NoGA.yml"
  #    "noGA/bias2-m01-NoGA.yml"
  #    "noGA/bias2-RN-m01-NoGA.yml"
  #    "noGA/bias3-m01-NoGA.yml"
  #    "noGA/bias3-RN-m01-NoGA.yml"
  #    "noGA/bias4-m01-NoGA.yml"
  #    "noGA/bias4-RN-m01-NoGA.yml"
  #    "noGA/bias1-m03-NoGA.yml"
  #    "noGA/bias2-m03-NoGA.yml"
  #    "noGA/bias2-RN-m03-NoGA.yml"
  #    "noGA/bias3-m03-NoGA.yml"
  #    "noGA/bias3-RN-m03-NoGA.yml"
  #    "noGA/bias4-m03-NoGA.yml"
  #    "noGA/bias4-RN-m03-NoGA.yml"
  #    "noGA/K2-NoGA.yml"
  #    "noGA/K2-RN-NoGA.yml"
  #    "noGA/Xh2-NoGA.yml"
  #    "noGA/Xh2-RN-NoGA.yml"
  #    "noGA/Xn1-NoGA.yml"
  #    "noGA/Xn1-RN-NoGA.yml"

)

for i in "${arr[@]}"; do
  echo $i
  screen -d -S ${i} -m ./build.sh ${i}

done

screen -list
