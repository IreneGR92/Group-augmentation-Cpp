
#include "Simulation.h"
#include <iostream>
#include <cassert>
#include "Simulation.h"
#include "stats/Statistics.h"
#include <vector>


Simulation::Simulation(const int replica)
        : replica(replica),
          population(),
          parameters(Parameters::instance()) {
}


void Simulation::run() {

    // Output file
    auto *statistics = new Statistics();

    statistics->calculateStatistics(population);
    statistics->printHeadersToConsole();
    statistics->printToConsole(generation, population.getDeaths());
//    statistics->printToFile(replica, generation, population.getDeaths(), newBreederFloater, newBreederHelper, inheritance);

    delete statistics;

    for (generation = 1; generation <= Parameters::instance()->getNumGenerations(); generation++) {
        statistics = new Statistics();
        population.reset();

        reassignFloaters();
        this->disperse();
        this->help();
        this->survival();

        if (generation % parameters->getSkip() == 0) {
            //Calculate stats

            statistics->calculateStatistics(population);

            //Print last generation
            if (generation == parameters->getNumGenerations() / 10 ||
                generation == parameters->getNumGenerations() / 4 ||
                generation == parameters->getNumGenerations() / 2 || generation == parameters->getNumGenerations()) {

                statistics->printToFileLastGeneration(this);
            }
        }


        this->mortality();
        this->newBreeder();

        // Print main file (separately since we need values of deaths, newBreederFloater, newBreederHelper and inheritance to be calculated)
        if (generation % parameters->getSkip() == 0) {
            statistics->printToConsole(generation, population.getDeaths());
//            statistics->printToFile(replica, generation, deaths, newBreederFloater, newBreederHelper, inheritance);
        }

        delete statistics;

        this->increaseAge();
        this->reproduce();
    }
}


void Simulation::reassignFloaters() {
    std::uniform_int_distribution<int> UniformMaxCol(0, parameters->getMaxColonies() - 1);
    int selectGroup;
    std::vector<Individual>::iterator floaterIt;
    while (!population.getFloaters().isEmpty()) {
        floaterIt = population.getFloaters().end() - 1;
        floaterIt->setHelp(0);
        selectGroup = UniformMaxCol(*parameters->getGenerator());
        groups[selectGroup].helpers.push_back(
                *floaterIt); //add the floater to the helper vector in a randomly selected group
        floaters.pop_back(); //remove the floater from its vector
    }
}


void Simulation::disperse() {

    int groupID = 0;

    std::vector<Individual> noRelatedHelpers;
    IndividualContainer allNoRelatedHelpers;
    std::vector<int> noRelatednessGroupsID;


    std::vector<Group, std::allocator<Group>>::iterator group;
    for (group = groups.begin(); group < groups.end(); ++group) {
        //Dispersal
        this->floaters.merge(group->disperse());

        // In the non relatedness implementation, helpers just born are reassigned to random groups. Groups receive as many helpers as helpers left the group for reassignment.
        if (parameters->isNoRelatedness() && generation > 0) {
            noRelatedHelpers = group->reassignNoRelatedness();
            for (int i = 0; i < noRelatedHelpers.size(); i++) {
                noRelatednessGroupsID.push_back(groupID);
            }
            allNoRelatedHelpers.merge(noRelatedHelpers);
            groupID++;
        }
    }

//        if (parameters->isNoRelatedness() && !allNoRelatedHelpers.empty()) {
//        std::uniform_int_distribution<int> UniformMaxCol(0, parameters->getMaxColonies() - 1);
//        int selectGroup;
//        std::vector<Individual>::iterator NoRelatedHelperIt;
//        while (!allNoRelatedHelpers.empty()) {
//            NoRelatedHelperIt = allNoRelatedHelpers.end() - 1;
//            selectGroup = UniformMaxCol(*parameters->getGenerator());
//            groups[selectGroup].helpers.push_back(
//                    *NoRelatedHelperIt); //add the no related helper to the helper vector in a randomly selected group
//            allNoRelatedHelpers.pop_back(); //remove the no related helper from its vector
//        }
//    }

    if (parameters->isNoRelatedness() && !allNoRelatedHelpers.empty()) {

        int selectGroupID, selectGroupIndex, size;
        std::vector<Individual>::iterator NoRelatedHelperIt;
        while (!allNoRelatedHelpers.empty()) {
            std::uniform_int_distribution<int> UniformGroupID(0, noRelatednessGroupsID.size() - 1);
            NoRelatedHelperIt = allNoRelatedHelpers.end() - 1;
            selectGroupIndex = UniformGroupID(
                    *parameters->getGenerator()); // selects a random index the noRelatednessGroupsID vector
            selectGroupID = noRelatednessGroupsID[selectGroupIndex]; // translates the index to the ID of a group from the noRelatednessGroupsID vector
            noRelatednessGroupsID.erase(noRelatednessGroupsID.begin() +
                                        selectGroupIndex); //remove the group ID from the vector to not draw it again
            groups[selectGroupID].helpers.push_back(
                    *NoRelatedHelperIt); //add the no related helper to the helper vector in a randomly selected group
            allNoRelatedHelpers.pop_back(); //remove the no related helper from its vector
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

const Container<Group> &Simulation::getGroups() const {
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







