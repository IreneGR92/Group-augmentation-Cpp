
#ifndef GROUP_AUGMENTATION_INDIVIDUAL_H
#define GROUP_AUGMENTATION_INDIVIDUAL_H


#include "Classes.h"
#include "../Parameters.h"

struct Individual {

    Individual(Individual &individual, Classes fishType, Parameters &parameters, std::default_random_engine &generator,
               int &generation);

    Individual(double drift_, Classes fishType_, Parameters &parameters, std::default_random_engine &generator,
               int &generation);


    Parameters parameters;

    std::default_random_engine generator;

    double alpha, alphaAge, beta, betaAge, drift,        // genetic values
            dispersal, help, survival;                                    // phenotypic values
    Classes fishType;                                                // possible classes: breeder, helper, floater
    int age;
    bool inherit;                                                    //did the new breeder inherit the territory or did it disperse?

    //Functions inside Individual
    double calcDispersal();

    void calcHelp();

    double calcSurvival(int groupSize);

    void mutate(int generation);

};


#endif //GROUP_AUGMENTATION_INDIVIDUAL_H
