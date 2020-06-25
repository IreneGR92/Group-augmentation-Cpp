#ifndef GROUP_AUGMENTATION_SIMULATION_H
#define GROUP_AUGMENTATION_SIMULATION_H


#include "Parameters.h"
#include "model/IndividualVector.h"
#include "model/DataModel.h"

class Simulation {
    Parameters *parameters;

    DataModel *dataModel;

    const int replica;

    int generation = 0;

    int deaths = 0;

    int newBreederFloater = 0, newBreederHelper = 0, inheritance = 0; //to know if the new breeder was a helper or a floater


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

    int getDeaths() const;

    int getNewbreederFloater() const;

    int getNewbreederHelper() const;

    int getInheritance() const;
};


#endif //GROUP_AUGMENTATION_SIMULATION_H
