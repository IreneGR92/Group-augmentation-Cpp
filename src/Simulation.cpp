#include <iostream>
#include <cassert>
#include "Simulation.h"


Simulation::Simulation(const int replica)
        : replica(replica) {
    this->parameters = Parameters::instance();
    this->groups = initializeGroups();

}


void Simulation::run() {

    // Output file

    statistics.calculateStatistics(groups, floaters);
    statistics.printHeadersToConsole();
    statistics.printToConsole(generation, deaths);
    statistics.printHeadersToFile();
    statistics.printToFile(replica, generation, deaths, newBreederFloater, newBreederHelper, inheritance);

    for (generation = 1; generation <= Parameters::instance()->getNumGenerations(); generation++) {

        deaths = 0; // to keep track of how many individuals die each generation
        newBreederFloater = 0;
        newBreederHelper = 0;
        inheritance = 0;

        this->disperse();
        this->help();
        this->survival();
        this->mortality();

        //Become a breeder
        std::vector<Group, std::allocator<Group>>::iterator itBreeder;
        for (itBreeder = groups.begin(); itBreeder < groups.end(); ++itBreeder) {
            if (!itBreeder->isBreederAlive()) {
                itBreeder->newBreeder(floaters, newBreederFloater, newBreederHelper, inheritance);
            }
        }

        //Reassign floaters floaters to groups for later philopatry vs dispersal
        reassignFloaters();
        /*if (!floaters.empty()) {
            cout << "Not all floaters were reassigned!" << endl;
        }*/

        //Increase age
        std::vector<Group, std::allocator<Group>>::iterator itAge;
        for (itAge = groups.begin(); itAge < groups.end(); ++itAge) {
            itAge->increaseAge(); //add 1 rank or age to all individuals alive
        }

        //Print stats
        if (generation % parameters->getSkip() == 0) {
            //Calculate stats
            statistics.calculateStatistics(groups, floaters); // Statistics calculated before survival
            statistics.printToConsole(generation, deaths);
            statistics.printToFile(replica, generation, deaths, newBreederFloater, newBreederHelper, inheritance);


            //Print last generation
            if (generation == parameters->getNumGenerations() / 10 ||
                generation == parameters->getNumGenerations() / 4 ||
                generation == parameters->getNumGenerations() / 2 || generation == parameters->getNumGenerations()) {

                statistics.printToFileLastGeneration(groups, replica, generation);
            }
        }

        //Reproduction
        std::vector<Group, std::allocator<Group>>::iterator itReproduction;
        for (itReproduction = groups.begin(); itReproduction < groups.end(); ++itReproduction) {
            itReproduction->reproduce(generation);
        }
    }
}



/* INITIALISE POPULATION */
std::vector<Group> Simulation::initializeGroups() {

    std::vector<Group> createdGroups(this->parameters->getMaxColonies(),
                                     Group(generation));

    assert(createdGroups.size() == Parameters::instance()->getMaxColonies());

    return createdGroups;
}


void Simulation::mortalityFloaters() { //Calculate the survival of the floaters

    std::vector<Individual, std::allocator<Individual>>::iterator floater;
    floater = floaters.begin();
    int size = floaters.size();
    for (int count = 0; !floaters.empty() && size > count; count++) {

        //Mortality floaters
        if (parameters->uniform(*parameters->getGenerator()) > floater->getSurvival()) {
            *floater = floaters[floaters.size() - 1];
            floaters.pop_back();
            deaths++;
        } else {
            floater++; //go to next individual
        }
    }
}


/*REASSIGN FLOATERS*/

void Simulation::reassignFloaters() {
    if (!parameters->isNoRelatedness()) {
        std::uniform_int_distribution<int> UniformMaxCol(0, parameters->getMaxColonies() - 1);
        int selectGroup;
        std::vector<Individual>::iterator indIt;
        while (!floaters.empty()) {
            indIt = floaters.end() - 1;
            indIt->setHelp(0);
            selectGroup = UniformMaxCol(*parameters->getGenerator());
            //indIt->getFishType() = HELPER; //modify the class
            groups[selectGroup].helpers.push_back(
                    *indIt); //add the floater to the helper std::vector in a randomly selected group
            floaters.pop_back(); //remove the floater from its std::vector
        }
    } else {

        double sumcumHelp = 0;
        double currentposition = 0;
        double RandP = parameters->uniform(*parameters->getGenerator());
        int allNoHelp = 0;

        std::vector<double> position; //std::vector of cumHelp to choose with higher likelihood the ind with higher age

        std::vector<Group, std::allocator<Group>>::iterator groupIt;
        for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
            sumcumHelp += 1 + groupIt->getCumHelp(); //add all the cumHelp from the std::vector Groups
            if (groupIt->getCumHelp() !=
                parameters->getMaxColonies()) { allNoHelp++; } //to check if all groups display cumhelp 0

            //if (sumcumHelp != 0) { cout << "sumcumHelp =" << sumcumHelp << '\t' << "allNoHelp =" << allNoHelp << endl; } //track
        }

        if (allNoHelp != 0) {

            for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
                position.push_back(static_cast<double>(1 + (groupIt)->getCumHelp()) / static_cast<double>(sumcumHelp) +
                                   currentposition); //creates a std::vector with proportional segments to the cumHelp of each group
                currentposition = position[position.size() - 1];
            }

            /*std::vector<Individual, std::allocator<Individual>>::iterator floatIt;
            for (floatIt = floaters.begin(); floatIt < floaters.end(); ++floatIt) {
                floatIt->getFishType() = HELPER; //all floaters pass to be helpers
            }*/

            while (!floaters.empty()) {
                for (int i = 0; i < groups.size(); i++) {
                    if (!floaters.empty()) {
                        if (RandP < position[i]) //to access the same ind in the candidates std::vector
                        {
                            groups[i].helpers.push_back(floaters[floaters.size() -
                                                                 1]); //add the floater to the helper std::vector in a randomly selected group
                            floaters.pop_back();
                        }
                    }
                }
            }

        } else {
            std::uniform_int_distribution<int> UniformMaxCol(0, parameters->getMaxColonies() - 1);
            int selectGroup;
            std::vector<Individual>::iterator floatIt;
            while (!floaters.empty()) {
                floatIt = floaters.end() - 1;
                selectGroup = UniformMaxCol(*parameters->getGenerator());
                //floatIt->getFishType() = HELPER; //modify the class
                groups[selectGroup].helpers.push_back(
                        *floatIt); //add the floater to the helper std::vector in a randomly selected group
                floaters.pop_back(); //remove the floater from its std::vector
            }
        }
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

void Simulation::disperse() {
    //Dispersal
    std::vector<Group, std::allocator<Group>>::iterator group;
    for (group = groups.begin(); group < groups.end(); ++group) {
        group->disperse(floaters);
    }
}

void Simulation::help() {
    //Help & Survival
    std::vector<Group, std::allocator<Group>>::iterator group;
    for (group = groups.begin(); group < groups.end(); ++group) {
        //Calculate help
        group->calculateCumulativeHelp();
    }
}

void Simulation::survival() {
    //Survival
    std::vector<Group, std::allocator<Group>>::iterator group;
    for (group = groups.begin(); group < groups.end(); ++group) {
        group->survival();
    }
    this->survivalFloaters();
}

void Simulation::survivalFloaters() {
    //Calculate survival for floaters
    std::vector<Individual, std::allocator<Individual>>::iterator floatIt;
    for (floatIt = floaters.begin(); floatIt < floaters.end(); ++floatIt) {
        floatIt->calculateSurvival(0); // TODO:Change to 1?
    }
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

void Simulation::mortality() {
    //Mortality of helpers and breeders
    std::vector<Group, std::allocator<Group>>::iterator itSurvival;
    for (itSurvival = groups.begin(); itSurvival < groups.end(); ++itSurvival) {
        itSurvival->mortality(deaths);
    }
    this->mortalityFloaters();

}





