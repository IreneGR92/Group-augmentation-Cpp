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
#include "Simulation.h"

using namespace std;


/* MAIN PROGRAM */
int main(int count, char **argv) {

    cout << "reading file " << argv[1] << "\n";

    unsigned seed = 0;

    Parameters parameters(argv[1]);


    default_random_engine generator(seed);


    parameters.print();

    for (int replica = 0; replica < parameters.getMaxNumReplicates(); replica++) {

        Simulation simulation(parameters, generator, replica);


        simulation.run();

    }

    return 0;
}

