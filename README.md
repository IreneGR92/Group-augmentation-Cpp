# Group augmentation

Simulation code, written in C++, to support the paper:
"Group benefits are a prerequisite for the evolution of cooperative breeding".

## Getting Started

### Pre conditions

- The software was built and tested on Ubuntu Linux -> this guide only covers Ubuntu Linux x86_64 if you are using macOS
  or Windows you may need to adapt some steps.
- Cmake, make and gcc installed
- If you are not on Linux with x86_64 you'll need to compile yaml-cpp yourself and reference it correctly in CMake.txt
  GitHub --> https://github.com/jbeder/yaml-cpp/tree/yaml-cpp-0.6.2

### Compile the binary

1. Make folder build and navigate into it `mkdir build` -> `cd build`
2. Generate make files with  `cmake .. -DCMAKE_BUILD_TYPE=Release`
3. Compile binary with `make`
4. The binary target App should be successfully built now.

### Run

Now you can normally run it by adding the relative path to the parameters yaml file as first run parameter:

`./App ../parameters/example.yml`

The output of the application is then stored in file `main_parameters_example.txt`
and `last_generation_parameters_example.txt`

You can modify the input parameters of the model by modifying the yml file. First lines allow you to choose between the
different models (with/without age-dependent plasticity and with/without relatedness building up from model dynamics). 