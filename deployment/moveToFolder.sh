#!/usr/bin/env bash

mkdir -p ~/Documents/new_results

mv ~/git/Group-augmentation-Cpp/cmake-build-release/*.txt ~/Documents/new_results/

mkdir -p ~/Documents/new_results/main
mkdir -p ~/Documents/new_results/last_generation

mv ~/Documents/new_results/main_parameters* ~/Documents/new_results/main/
mv ~/Documents/new_results/last_generation_parameters_* ~/Documents/new_results/last_generation/
