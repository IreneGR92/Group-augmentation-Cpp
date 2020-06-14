#ifndef GROUP_AUGMENTATION_SIMULATION_H
#define GROUP_AUGMENTATION_SIMULATION_H


#include "Parameters.h"
#include "model/Group.h"
#include "model/DataModel.h"

class Simulation {

    Parameters *parameters;

    IndividualVector floaters;

    std::vector<Group> groups;

    const int replica;

    DataModel model;

    int generation = 0;


    void survivalFloaters();

    void mortalityFloaters();

    void increaseAgeFloaters();

public:

    std::vector<Group> initializeGroups();

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

    const std::vector<Group> &getGroups() const;

    const int getReplica() const;

    int getGeneration() const;

};


#endif //GROUP_AUGMENTATION_SIMULATION_H
