#ifndef GROUP_AUGMENTATION_SIMULATION_H
#define GROUP_AUGMENTATION_SIMULATION_H


#include "Parameters.h"
#include "model/Group.h"
#include "Statistics.h"

class Simulation {

    const int replica;

    Statistics statistics;

    std::default_random_engine generator;

    int generation = 0;

    std::vector<Individual> floaters;

    std::vector<Group> groups;

    int deaths = 0;

    int newbreederFloater = 0, newbreederHelper = 0, inheritance = 0; //to know if the new breeder was a helper or a floater

    Parameters parameters;


public:

    Simulation(Parameters &parameters, std::default_random_engine &generator, const int replica);

    void run();

    std::vector<Group> initializeGroups();

    void survivalFloaters(std::vector<Individual> &floaters, int &deaths);

    void reassignFloaters(std::vector<Individual> &floaters, std::vector<Group> &groups);

    const std::vector<Group> &getGroups() const;

    int getGeneration() const;

    const int getReplica() const;

    const std::default_random_engine &getGenerator() const;

    const std::vector<Individual> &getFloaters() const;

    int getDeaths() const;

    int getNewbreederFloater() const;

    int getNewbreederHelper() const;

    int getInheritance() const;

    const Parameters &getParameters() const;


};


#endif //GROUP_AUGMENTATION_SIMULATION_H
