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
#SBATCH --array=0-2 //TODO: Number of runs, remember to change!

#### Your shell commands below this line ####

declare -a arr=(

#   MUTATION MODIFIED  #
"default_bias1-RN.yml"
"default_bias2-RN-m03.yml"
"noRelatedness_bias1-RN.yml"

		)

srun ./build/App "parameters/${arr[$SLURM_ARRAY_TASK_ID]}"
