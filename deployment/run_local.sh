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


#"original_m02-Xn3.yml"
#"original_m02-Xn4.yml"
#"original_m02-Xn0.yml"
#"original_m02-Xn1.yml"
#"fullRandom_m02-Xn4.yml"
#"fullRandom_m02-Xn3.yml"
#"fullRandom_m02-Xn1.yml"
#"fullRandom_m02-Xn0.yml"

#"noRelatedness_m02.yml"
#"noRelatedness_m02-Age2.yml"
#"noRelatedness_m02-Age3.yml"
#"noRelatedness_m02-RN.yml"
#"noRelatedness_m02-RN-Age2.yml"
#"noRelatedness_m02-RN-Age3.yml"
#"noRelatedness_m03.yml"
#"noRelatedness_m03-Age2.yml"
#"noRelatedness_m03-Age3.yml"
#"noRelatedness_m03-RN.yml"
#"noRelatedness_m03-RN-Age2.yml"
#"noRelatedness_m03-RN-Age3.yml"
#"noRelatedness_m023.yml"
#"noRelatedness_m023-Age2.yml"
#"noRelatedness_m023-Age3.yml"
#"noRelatedness_m023-n12.yml"
#"noRelatedness_m023-n12-RN.yml"
#"noRelatedness_m023-RN.yml"
#"noRelatedness_m023-RN-Age2.yml"
#"noRelatedness_m023-RN-Age3.yml"
#"noRelatedness_m026.yml"
#"noRelatedness_m026-Age2.yml"
#"noRelatedness_m026-Age3.yml"
#"noRelatedness_m026-RN.yml"
#"noRelatedness_m026-RN-Age2.yml"
#"noRelatedness_m026-RN-Age3.yml"

#   FIGURE 2  #
#"default_m01-Xn0.yml"
#"default_m01-Xn1.yml"
#"default_m01-Xn3.yml"
#"default_m01-Xn4.yml"
#"default_m02-Xn0.yml"
#"default_m02-Xn1.yml"
#"default_m02-Xn3.yml"
#"default_m02-Xn4.yml"
#"default_m03-Xn0.yml"
#"default_m03-Xn1.yml"
#"default_m03-Xn3.yml"
#"default_m03-Xn4.yml"
#"noRelatedness_m01-Xn0.yml"
#"noRelatedness_m01-Xn1.yml"
#"noRelatedness_m01-Xn3.yml"
#"noRelatedness_m01-Xn4.yml"
#"noRelatedness_m02-Xn0.yml"
#"noRelatedness_m02-Xn1.yml"
#"noRelatedness_m02-Xn3.yml"
#"noRelatedness_m02-Xn4.yml"
#"noRelatedness_m03-Xn0.yml"
#"noRelatedness_m03-Xn1.yml"
#"noRelatedness_m03-Xn3.yml"
#"noRelatedness_m03-Xn4.yml"

#"default_m01-Xn0-RN.yml"
#"default_m01-Xn1-RN.yml"
#"default_m01-Xn3-RN.yml"
#"default_m01-Xn4-RN.yml"
#"default_m02-Xn0-RN.yml"
#"default_m02-Xn1-RN.yml"
#"default_m02-Xn3-RN.yml"
#"default_m02-Xn4-RN.yml"
#"default_m03-Xn0-RN.yml"
#"default_m03-Xn1-RN.yml"
#"default_m03-Xn3-RN.yml"
#"default_m03-Xn4-RN.yml"
#"noRelatedness_m01-Xn0-RN.yml"
#"noRelatedness_m01-Xn1-RN.yml"
#"noRelatedness_m01-Xn3-RN.yml"
#"noRelatedness_m01-Xn4-RN.yml"
#"noRelatedness_m02-Xn0-RN.yml"
#"noRelatedness_m02-Xn1-RN.yml"
#"noRelatedness_m02-Xn3-RN.yml"
#"noRelatedness_m02-Xn4-RN.yml"
#"noRelatedness_m03-Xn0-RN.yml"
#"noRelatedness_m03-Xn1-RN.yml"
#"noRelatedness_m03-Xn3-RN.yml"
#"noRelatedness_m03-Xn4-RN.yml"


#   FIGURE 3  #
#"default_m02-bias1.yml"
#"default_m03-bias2.yml"
#"noRelatedness_m02-bias1.yml"
#"noRelatedness_m03-bias2.yml"


#   FIGURE 4  #
#"default_m012-n12-RN.yml"
#"default_m012-n14-RN.yml"
#"default_m012-n16-RN.yml"
#"default_m012-n18-RN.yml"
#"default_m012-n1-RN.yml"
#"default_m012-n2-RN.yml"
#"default_m014-n12-RN.yml"
#"default_m014-n14-RN.yml"
#"default_m014-n16-RN.yml"
#"default_m014-n18-RN.yml"
#"default_m014-n1-RN.yml"
#"default_m014-n2-RN.yml"
#"default_m016-n12-RN.yml"
#"default_m016-n14-RN.yml"
#"default_m016-n16-RN.yml"
#"default_m016-n18-RN.yml"
#"default_m016-n1-RN.yml"
#"default_m016-n2-RN.yml"
#"default_m018-n12-RN.yml"
#"default_m018-n14-RN.yml"
#"default_m018-n16-RN.yml"
#"default_m018-n18-RN.yml"
#"default_m018-n1-RN.yml"
#"default_m018-n2-RN.yml"
#"default_m01-n12-RN.yml"
#"default_m01-n14-RN.yml"
#"default_m01-n16-RN.yml"
#"default_m01-n18-RN.yml"
#"default_m01-n1-RN.yml"
#"default_m01-n2-RN.yml"
#"default_m02-n12-RN.yml"
#"default_m02-n14-RN.yml"
#"default_m02-n16-RN.yml"
#"default_m02-n18-RN.yml"
#"default_m02-n1-RN.yml"
#"default_m02-n2-RN.yml"
#"noRelatedness_m012-n12-RN.yml"
#"noRelatedness_m012-n14-RN.yml"
#"noRelatedness_m012-n16-RN.yml"
#"noRelatedness_m012-n18-RN.yml"
#"noRelatedness_m012-n1-RN.yml"
#"noRelatedness_m012-n2-RN.yml"
#"noRelatedness_m014-n12-RN.yml"
#"noRelatedness_m014-n14-RN.yml"
#"noRelatedness_m014-n16-RN.yml"
#"noRelatedness_m014-n18-RN.yml"
#"noRelatedness_m014-n1-RN.yml"
#"noRelatedness_m014-n2-RN.yml"
#"noRelatedness_m016-n12-RN.yml"
#"noRelatedness_m016-n14-RN.yml"
#"noRelatedness_m016-n16-RN.yml"
#"noRelatedness_m016-n18-RN.yml"
#"noRelatedness_m016-n1-RN.yml"
#"noRelatedness_m016-n2-RN.yml"
#"noRelatedness_m018-n12-RN.yml"
#"noRelatedness_m018-n14-RN.yml"
#"noRelatedness_m018-n16-RN.yml"
#"noRelatedness_m018-n18-RN.yml"
#"noRelatedness_m018-n1-RN.yml"
#"noRelatedness_m018-n2-RN.yml"
#"noRelatedness_m01-n12-RN.yml"
#"noRelatedness_m01-n14-RN.yml"
#"noRelatedness_m01-n16-RN.yml"
#"noRelatedness_m01-n18-RN.yml"
#"noRelatedness_m01-n1-RN.yml"
#"noRelatedness_m01-n2-RN.yml"
#"noRelatedness_m02-n12-RN.yml"
#"noRelatedness_m02-n14-RN.yml"
#"noRelatedness_m02-n16-RN.yml"
#"noRelatedness_m02-n18-RN.yml"
#"noRelatedness_m02-n1-RN.yml"
#"noRelatedness_m02-n2-RN.yml"


)

for i in "${arr[@]}"; do
  echo $i
  screen -d -S ${i} -m ./build.sh ${i}

done

screen -list
