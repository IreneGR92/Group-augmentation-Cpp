/***********************************************
 GROUP AUGMENTATION MODEL
 - Passive group augmentation: individuals help in order to increase group size which in turn increases survival
 - Active group augmentation or delayed reciprocity: if inherit the breeding position, individuals benefit of the help given by the recruits in the group.


 Stochastic individual based model.
 Limited breeding spots, unlimited population size; one breeder per group.
 Population overlap, turn over when breeder dies. 
 Older/more dominant individual higher probability of becoming new breeder.
 Evolution of level of help and dispersal. Inclusion of reaction norm to age.
 Dispersal produces dispersers/floaters that can reproduce in another group or join as helpers.
 Survival dependent on group size, level of help and predation/environment.
 Fecundity dependent on cumulative level of help within group.
 Relatedness as an emergent property.

***********************************************/

/*HEADER FILES*/

#include <iostream>
#include "Parameters.h"
#include "stats/Statistics.h"


/* MAIN PROGRAM */
int main(int count, char **argv) {
    //goes to HEAP since we need it only once globally
    Parameters *parameters;
    if (count > 1) {
        std::cout << "reading file " << argv[1] << "\n";
        parameters = new Parameters(argv[1]);
    } else {
        parameters = new Parameters;
    }

    parameters->print();

    Statistics statistics;
    statistics.printHeadersToFile();


    for (int replica = 0; replica < parameters->getMaxNumReplicates(); replica++) {

        auto *simulation = new Simulation(replica);
        simulation->run();
        delete simulation;
    }


//    vector<double> testVector;
//
//    testVector.push_back(4);
//    testVector.push_back(5);
//
//
//    StatisticalFormulas test;
//    test.addValues(testVector);
//    cout << test.calculateMean();


    return 0;
}

