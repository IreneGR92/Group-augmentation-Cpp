#!/bin/bash

#Remove slurm output files after run finished
rm slurm*

# Remove previous results
rm -r Results

#Move files to folder
mkdir Results
mkdir Results/main
mkdir Results/last_generation
mv main* Results/main/
mv last* Results/last_generation/


