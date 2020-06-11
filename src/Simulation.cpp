#include <iostream>
#include <cassert>
#include "Simulation.h"
#include "stats/Statistics.h"


Simulation::Simulation(const int replica)
        : replica(replica) {
    this->parameters = Parameters::instance();
    this->groups = initializeGroups();

}


void Simulation::run() {

    // Output file
    Statistics statistics;

    statistics.calculateStatistics(groups, floaters);
    statistics.printHeadersToConsole();
    statistics.printToConsole(generation, deaths);
    statistics.printToFile(replica, generation, deaths, newBreederFloater, newBreederHelper, inheritance);

    for (generation = 1; generation <= Parameters::instance()->getNumGenerations(); generation++) {

        deaths = 0; // to keep track of how many individuals die each generation
        newBreederFloater = 0;
        newBreederHelper = 0;
        inheritance = 0;

        reassignFloaters();
        this->disperse();
        this->help();
        this->survival();

        if (generation % parameters->getSkip() == 0) {
            //Calculate stats
            statistics.calculateStatistics(groups, floaters);

            //Print last generation
            if (generation == parameters->getNumGenerations() / 10 ||
                generation == parameters->getNumGenerations() / 4 ||
                generation == parameters->getNumGenerations() / 2 || generation == parameters->getNumGenerations()) {

                statistics.printToFileLastGeneration(this);
            }
        }

        this->mortality();
        this->newBreeder();

        // Print main file (separately since we need values of deaths, newBreederFloater, newBreederHelper and inheritance to be calculated)
        if (generation % parameters->getSkip() == 0) {
            statistics.printToConsole(generation, deaths);
            statistics.printToFile(replica, generation, deaths, newBreederFloater, newBreederHelper, inheritance);
        }

        this->increaseAge();
        this->reproduce();
    }
}


/* INITIALISE POPULATION */
std::vector<Group> Simulation::initializeGroups() {

    std::vector<Group> createdGroups(this->parameters->getMaxColonies(), Group(generation));

    assert(createdGroups.size() == Parameters::instance()->getMaxColonies());

    return createdGroups;
}


void Simulation::reassignFloaters() {
    std::uniform_int_distribution<int> UniformMaxCol(0, parameters->getMaxColonies() - 1);
    int selectGroup;
    std::vector<Individual>::iterator floaterIt;
    while (!floaters.empty()) {
        floaterIt = floaters.end() - 1;
        floaterIt->setHelp(0);
        selectGroup = UniformMaxCol(*parameters->getGenerator());
        //floaterIt->getFishType() = HELPER; //modify the class
        groups[selectGroup].helpers.push_back(
                *floaterIt); //add the floater to the helper vector in a randomly selected group
        floaters.pop_back(); //remove the floater from its vector
    }
}


void Simulation::disperse() {
    //Dispersal
    std::vector<Group, std::allocator<Group>>::iterator group;
    for (group = groups.begin(); group < groups.end(); ++group) {
        std::vector<Individual> noRelatedHelpers = group->disperse(floaters); //creates floaters and passes the new offspring for reassignment in the noRelatedness configuration

        if (!noRelatedHelpers.empty()) {
            std::uniform_int_distribution<int> UniformMaxCol(0, parameters->getMaxColonies() - 1);
            int selectGroup;
            std::vector<Individual>::iterator NoRelatedHelperIt;
            while (!noRelatedHelpers.empty()) {
                NoRelatedHelperIt = noRelatedHelpers.end() - 1;
                selectGroup = UniformMaxCol(*parameters->getGenerator());
                groups[selectGroup].helpers.push_back(
                        *NoRelatedHelperIt); //add the no related helper to the helper vector in a randomly selected group
                noRelatedHelpers.pop_back(); //remove the no related helper from its vector
            }
        }
    }
}

void Simulation::help() {
    //Help & Survival
    std::vector<Group, std::allocator<Group>>::iterator groupIt;
    for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
        //Calculate help
        groupIt->calculateCumulativeHelp();
    }
}

void Simulation::survival() {
    //Survival
    std::vector<Group, std::allocator<Group>>::iterator groupIt;
    for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
        groupIt->survivalGroup();
    }
    this->survivalFloaters();
}

void Simulation::survivalFloaters() {
    //Calculate survival for floaters
    std::vector<Individual, std::allocator<Individual>>::iterator floatersIt;
    for (floatersIt = floaters.begin(); floatersIt < floaters.end(); ++floatersIt) {
        floatersIt->calculateSurvival(0); // TODO:Change to 1?
    }
}

void Simulation::mortality() {
    //Mortality of helpers and breeders
    std::vector<Group, std::allocator<Group>>::iterator groupIt;
    for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
        groupIt->mortalityGroup(deaths);
    }
    this->mortalityFloaters();

}

void Simulation::mortalityFloaters() { //Calculate the survival of the floaters

    std::vector<Individual, std::allocator<Individual>>::iterator floaterIt;
    floaterIt = floaters.begin();
    int size = floaters.size();
    for (int count = 0; !floaters.empty() && size > count; count++) {

        //Mortality floaters
        if (parameters->uniform(*parameters->getGenerator()) > floaterIt->getSurvival()) {
            *floaterIt = floaters[floaters.size() - 1];
            floaters.pop_back();
            deaths++;
        } else {
            floaterIt++; //go to next individual
        }
    }
}

void Simulation::newBreeder() {
    std::vector<Group, std::allocator<Group>>::iterator groupIt;
    for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
        if (!groupIt->isBreederAlive()) {
            groupIt->newBreeder(floaters, newBreederFloater, newBreederHelper, inheritance);
        }
    }
}

void Simulation::increaseAge() {
    std::vector<Group, std::allocator<Group>>::iterator groupIt;
    for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
        groupIt->increaseAge();
    }
    this->increaseAgeFloaters();
}

void Simulation::increaseAgeFloaters() {
    std::vector<Individual, std::allocator<Individual>>::iterator floatersIt;
    for (floatersIt = floaters.begin(); floatersIt < floaters.end(); ++floatersIt) {
        floatersIt->increaseAge();
    }
}

void Simulation::reproduce() {
    std::vector<Group, std::allocator<Group>>::iterator group;
    for (group = groups.begin(); group < groups.end(); ++group) {
        group->reproduce(generation);
    }
}


const std::vector<Individual> &Simulation::getFloaters() const {
    return floaters;
}

const std::vector<Group> &Simulation::getGroups() const {
    return groups;
}


const int Simulation::getReplica() const {
    return replica;
}

int Simulation::getGeneration() const {
    return generation;
}

int Simulation::getDeaths() const {
    return deaths;
}

int Simulation::getNewbreederFloater() const {
    return newBreederFloater;
}

int Simulation::getNewbreederHelper() const {
    return newBreederHelper;
}

int Simulation::getInheritance() const {
    return inheritance;
}






