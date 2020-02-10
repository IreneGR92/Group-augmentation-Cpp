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
#SBATCH --array=0-4

#### Your shell commands below this line ####

declare -a arr=(
		"default.yml"
		"default2.yml"
		"bla"
		"test"
		"1023wafdsf"
)


srun echo ${arr[$SLURM_ARRAY_TASK_ID]}
