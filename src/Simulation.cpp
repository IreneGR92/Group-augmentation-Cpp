
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

        population.reassignFloaters();
        population.disperse(generation);
        population.help();
        population.survival();

        if (generation % parameters->getSkip() == 0) {
            //Calculate stats

            statistics->calculateStatistics(population);

            //Print last generation
            if (generation == 10000 ||
                generation == 25000 ||
                generation == parameters->getNumGenerations() / 2 ||
                generation == parameters->getNumGenerations()) {

                statistics->printToFileLastGeneration(this, population);
            }
        }


        population.mortality();
        population.newBreeder();

        // Print main file (separately since we need values of deaths, newBreederFloater, newBreederHelper and inheritance to be calculated)
        if (generation % parameters->getSkip() == 0) {
            statistics->printToConsole(generation, population.getDeaths());
            statistics->printToFile(replica, generation, population.getDeaths(), population.getNewBreederFloater(), population.getNewBreederHelper(), population.getInheritance());
        }

        delete statistics;

        population.increaseAge();
        population.reproduce(generation);
    }
}

const int Simulation::getReplica() const {
    return replica;
}

int Simulation::getGeneration() const {
    return generation;
}










