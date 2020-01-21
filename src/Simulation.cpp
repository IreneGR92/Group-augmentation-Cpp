#include <iostream>
#include "Simulation.h"


void Simulation::run() {

    // Output file

    statistics.calculateStatistics(groups, floaters);
    statistics.printHeadersToConsole();
    statistics.printToConsole(generation, deaths);
    statistics.printHeadersToFile();
    statistics.printToFile(replica, generation, deaths, newbreederFloater, newbreederHelper, inheritance);

    for (generation = 1; generation <= parameters.getNumGenerations(); generation++) {

        deaths = 0; // to keep track of how many individuals die each generation
        newbreederFloater = 0;
        newbreederHelper = 0;
        inheritance = 0;

        //Dispersal
        std::vector<Group, std::allocator<Group>>::iterator itDispersal;
        for (itDispersal = groups.begin(); itDispersal < groups.end(); ++itDispersal) {
            itDispersal->disperse(floaters);
        }

        //Help & Survival
        std::vector<Group, std::allocator<Group>>::iterator itHelpSurvival;
        for (itHelpSurvival = groups.begin(); itHelpSurvival < groups.end(); ++itHelpSurvival) {
            //Calculate help
            itHelpSurvival->calcCumHelp();

            //Calculate survival for the helpers
            itHelpSurvival->calcGroupSize();
            std::vector<Individual, std::allocator<Individual>>::iterator helperStatsIt; //helpers
            for (helperStatsIt = itHelpSurvival->helpers.begin(); helperStatsIt < itHelpSurvival->helpers.end(); ++helperStatsIt) {
                 helperStatsIt->calcSurvival(itHelpSurvival->groupSize);
            }

            //Calculate the survival of the breeder
            if (parameters.isLowSurvivalBreeder()) {
                itHelpSurvival->breeder.calcSurvival(0); //survival for breeder does not include group size benefits
            } // TODO:Change to 1?
            else {
                itHelpSurvival->breeder.calcSurvival(itHelpSurvival->groupSize);
            }
        }

        //Calculate survival for floaters
        std::vector<Individual, std::allocator<Individual>>::iterator floatIt;
        for (floatIt = floaters.begin(); floatIt < floaters.end(); ++floatIt) {
            floatIt->calcSurvival(0); // TODO:Change to 1?
        }

        //Calculate stats
        if (generation % parameters.getSkip() == 0) {
            statistics.calculateStatistics(groups, floaters); // Statistics calculated before survival
        }

        //Mortality of helpers and breeders
        std::vector<Group, std::allocator<Group>>::iterator itSurvival;
        for (itSurvival = groups.begin(); itSurvival < groups.end(); ++itSurvival) {
            itSurvival->survival(deaths);
        }

        //Mortality of floaters
        survivalFloaters(floaters, deaths);

        //Become a breeder
        std::vector<Group, std::allocator<Group>>::iterator itBreeder;
        for (itBreeder = groups.begin(); itBreeder < groups.end(); ++itBreeder) {
            if (!itBreeder->breederAlive) {
                itBreeder->newBreeder(floaters, newbreederFloater, newbreederHelper, inheritance);
            }
        }

        //Reassign floaters floaters to groups for later philopatry vs dispersal
        reassignFloaters(floaters, groups);
        /*if (!floaters.empty()) {
            cout << "Not all floaters were reassigned!" << endl;
        }*/

        //Increase age
        std::vector<Group, std::allocator<Group>>::iterator itAge;
        for (itAge = groups.begin(); itAge < groups.end(); ++itAge) {
            itAge->increaseAge(); //add 1 rank or age to all individuals alive
        }

        //Print stats
        if (generation % parameters.getSkip() == 0) {
            statistics.printToConsole(generation, deaths);
            statistics.printToFile(replica, generation, deaths, newbreederFloater, newbreederHelper, inheritance);


            //Print last generation
            if (generation == parameters.getNumGenerations() / 10 || generation == parameters.getNumGenerations() / 4 ||
                generation == parameters.getNumGenerations() / 2 || generation == parameters.getNumGenerations()) {

                statistics.printToFileLastGeneration(groups, replica, generation);
            }
        }

        //Reproduction
        std::vector<Group, std::allocator<Group>>::iterator itReproduction;
        for (itReproduction = groups.begin(); itReproduction < groups.end(); ++itReproduction) {
            itReproduction->reproduce();
        }
    }
}

/* INITIALISE POPULATION */
std::vector<Group> Simulation::initializeGroups() {

    std::vector<Group> groups(this->parameters.getMaxColonies(), Group(this->parameters, generator, generation));

    return groups;
}


void Simulation::survivalFloaters(std::vector<Individual> &floaters,
                             int &deaths){ //Calculate the survival of the floaters

    std::vector<Individual, std::allocator<Individual>>::iterator survFIt;
    survFIt = floaters.begin();
    int sizevec = floaters.size();
    int counting = 0;
    while (!floaters.empty() && sizevec > counting) {

        //Mortality floaters
        if (parameters.uniform(generator) > survFIt->getSurvival()) {
            *survFIt = floaters[floaters.size() - 1];
            floaters.pop_back();
            ++counting;
            deaths++;
        } else
            ++survFIt, ++counting; //go to next individual
    }
}


/*REASSIGN FLOATERS*/

void Simulation::reassignFloaters(std::vector<Individual> &floaters, std::vector<Group> &groups) {
    if (!parameters.isNoRelatedness()) {
        std::uniform_int_distribution<int> UniformMaxCol(0, parameters.getMaxColonies() - 1);
        int selectGroup;
        std::vector<Individual>::iterator indIt;
        while (!floaters.empty()) {
            indIt = floaters.end() - 1;
            indIt->setHelp(0);
            selectGroup = UniformMaxCol(generator);
            //indIt->getFishType() = HELPER; //modify the class
            groups[selectGroup].helpers.push_back(
                    *indIt); //add the floater to the helper std::vector in a randomly selected group
            floaters.pop_back(); //remove the floater from its std::vector
        }
    } else {

        double sumcumHelp = 0;
        double currentposition = 0;
        double RandP = parameters.uniform(generator);
        int allNoHelp = 0;

        std::vector<double> position; //std::vector of cumHelp to choose with higher likelihood the ind with higher age

        std::vector<Group, std::allocator<Group>>::iterator groupIt;
        for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
            sumcumHelp += 1 + groupIt->cumHelp; //add all the cumHelp from the std::vector Groups
            if (groupIt->cumHelp !=
                parameters.getMaxColonies()) { allNoHelp++; } //to check if all groups display cumhelp 0

            //if (sumcumHelp != 0) { cout << "sumcumHelp =" << sumcumHelp << '\t' << "allNoHelp =" << allNoHelp << endl; } //track
        }

        if (allNoHelp != 0) {

            for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
                position.push_back(static_cast<double>(1 + (groupIt)->cumHelp) / static_cast<double>(sumcumHelp) +
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
            std::uniform_int_distribution<int> UniformMaxCol(0, parameters.getMaxColonies() - 1);
            int selectGroup;
            std::vector<Individual>::iterator floatIt;
            while (!floaters.empty()) {
                floatIt = floaters.end() - 1;
                selectGroup = UniformMaxCol(generator);
                //floatIt->getFishType() = HELPER; //modify the class
                groups[selectGroup].helpers.push_back(
                        *floatIt); //add the floater to the helper std::vector in a randomly selected group
                floaters.pop_back(); //remove the floater from its std::vector
            }
        }
    }
}

Simulation::Simulation(Parameters &parameters, std::default_random_engine &generator, const int replica)
        : replica(replica), statistics(parameters) {
    this->generator = generator;
    this->parameters = parameters;
    this->groups = initializeGroups();
}


const Parameters &Simulation::getParameters() const {
    return parameters;
}

const std::default_random_engine &Simulation::getGenerator() const {
    return generator;
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
    return newbreederFloater;
}

int Simulation::getNewbreederHelper() const {
    return newbreederHelper;
}

int Simulation::getInheritance() const {
    return inheritance;
}

