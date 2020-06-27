#ifndef GROUP_AUGMENTATION_SIMULATION_H
#define GROUP_AUGMENTATION_SIMULATION_H


#include "Parameters.h"
#include "model/Population.h"

class Simulation {
    Parameters *parameters;

    Population population;

    const int replica;

    int generation = 0;

public:


    void run();

    Simulation(const int replica);

    const int getReplica() const;

    int getGeneration() const;

};


#endif //GROUP_AUGMENTATION_SIMULATION_H
