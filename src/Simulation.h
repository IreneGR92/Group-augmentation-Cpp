#ifndef GROUP_AUGMENTATION_SIMULATION_H
#define GROUP_AUGMENTATION_SIMULATION_H


#include "Parameters.h"
#include "model/Population.h"

class Simulation {
    Parameters *parameters;

    Population population;

    const int replica;

    int generation = 0;

    void survivalFloaters();

    void mortalityFloaters();

    void increaseAgeFloaters();

public:


    void run();

    void reassignFloaters();

    void disperse();

    void help();

    void survival();

    void mortality();

    void newBreeder();

    void increaseAge();

    void reproduce();


    Simulation(const int replica);

    //TODO: stats?

    const std::vector<Individual> &getFloaters() const;

    const Container<Group>&getGroups() const;

    const int getReplica() const;

    int getGeneration() const;

    int getDeaths() const;

    int getNewbreederFloater() const;

    int getNewbreederHelper() const;

    int getInheritance() const;
};


#endif //GROUP_AUGMENTATION_SIMULATION_H
