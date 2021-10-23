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

"noRelatedness_bias1-RN.yml"
"noRelatedness_bias2-RN-m03.yml"

"noRelatedness_bias1-RN-m015.yml"
"noRelatedness_bias1-RN-m025.yml"
"noRelatedness_bias2-RN-m015.yml"
"noRelatedness_bias2-RN-m025.yml"
"noRelatedness_bias3-RN-m015.yml"
"noRelatedness_bias3-RN-m025.yml"
"noRelatedness_bias4-RN-m015.yml"
"noRelatedness_bias4-RN-m025.yml"
"noRelatedness_bias15-RN-m01.yml"
"noRelatedness_bias15-RN-m02.yml"
"noRelatedness_bias15-RN-m03.yml"
"noRelatedness_bias15-RN-m015.yml"
"noRelatedness_bias15-RN-m025.yml"
"noRelatedness_bias25-RN-m01.yml"
"noRelatedness_bias25-RN-m02.yml"
"noRelatedness_bias25-RN-m03.yml"
"noRelatedness_bias25-RN-m015.yml"
"noRelatedness_bias25-RN-m025.yml"
"noRelatedness_bias35-RN-m01.yml"
"noRelatedness_bias35-RN-m02.yml"
"noRelatedness_bias35-RN-m03.yml"
"noRelatedness_bias35-RN-m015.yml"
"noRelatedness_bias35-RN-m025.yml"
"noRelatedness_m01-n1-RN.yml"
"noRelatedness_m01-n2-RN.yml"
"noRelatedness_m01-n12-RN.yml"
"noRelatedness_m01-n14-RN.yml"
"noRelatedness_m01-n16-RN.yml"
"noRelatedness_m01-n18-RN.yml"
"noRelatedness_m02-n1-RN.yml"
"noRelatedness_m02-n2-RN.yml"
"noRelatedness_m02-n12-RN.yml"
"noRelatedness_m02-n14-RN.yml"
"noRelatedness_m02-n16-RN.yml"
"noRelatedness_m02-n18-RN.yml"
"noRelatedness_m012-n1-RN.yml"
"noRelatedness_m012-n2-RN.yml"
"noRelatedness_m012-n12-RN.yml"
"noRelatedness_m012-n14-RN.yml"
"noRelatedness_m012-n16-RN.yml"
"noRelatedness_m012-n18-RN.yml"
"noRelatedness_m014-n1-RN.yml"
"noRelatedness_m014-n2-RN.yml"
"noRelatedness_m014-n12-RN.yml"
"noRelatedness_m014-n14-RN.yml"
"noRelatedness_m014-n16-RN.yml"
"noRelatedness_m014-n18-RN.yml"
"noRelatedness_m016-n1-RN.yml"
"noRelatedness_m016-n2-RN.yml"
"noRelatedness_m016-n12-RN.yml"
"noRelatedness_m016-n14-RN.yml"
"noRelatedness_m016-n16-RN.yml"
"noRelatedness_m016-n18-RN.yml"
"noRelatedness_m018-n1-RN.yml"
"noRelatedness_m018-n2-RN.yml"
"noRelatedness_m018-n12-RN.yml"
"noRelatedness_m018-n14-RN.yml"
"noRelatedness_m018-n16-RN.yml"
"noRelatedness_m018-n18-RN.yml"


)

for i in "${arr[@]}"; do
  echo $i
  screen -d -S ${i} -m ./build.sh ${i}

done

screen -list
