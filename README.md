#Group augmentation

Simulation code, written in C++, to support the paper:
"Group benefits are a prerequisite for the evolution of cooperative breeding". 


##Getting Started
###Pre conditions

- The software was built and tested on Ubuntu Linux -> this guide only covers Ubuntu Linux if you are using macOS or Windows you may need to adapt some steps.
- Cmake and make installed
- yaml-cpp version 0.6.2 binary is either installed or source code is accessible on the system -->https://github.com/jbeder/yaml-cpp/tree/yaml-cpp-0.6.2

###Compile the binary

1. Configure the correct path to the yaml-cpp binary in CmakeLists.txt line 20
2. Make folder build and navigate into it `mkdir build` -> `cd build`
3. Generate make files with  `cmake .. -DCMAKE_BUILD_TYPE=Release`
4. Compile binary with `make`
5. The binary target App should be successfully built now.

### Run

Now you can normally run it by adding the relative path to the parameters yaml file as first run parameter:

`./App ../parameters/example.yml`

The output of the application is then stored in file `main_parameters_example.txt` and `last_generation_parameters_example.txt`

You can modify the input parameters of the model by modifying the yml file. First lines allow you to choose between the different 
models (with/without age-dependent plasticity and with/without relatedness building up from model dynamics). 