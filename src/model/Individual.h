
#ifndef GROUP_AUGMENTATION_INDIVIDUAL_H
#define GROUP_AUGMENTATION_INDIVIDUAL_H


#include "Classes.h"

struct Individual {
    Individual(double alpha_, double alphaAge_, double beta_, double betaAge_, double drift_, Classes fishType_);

    double alpha, alphaAge, beta, betaAge, drift,        // genetic values
            dispersal, help, survival;                                    // phenotypic values
    Classes fishType;                                                // possible classes: breeder, helper, floater
    int age;
    bool inherit;                                                    //did the new breeder inherit the territory or did it disperse?

    //Functions inside Individual
    double calcDispersal();

    void calcHelp();

    double calcSurvival(int groupSize);

    void Mutate();
};


#endif //GROUP_AUGMENTATION_INDIVIDUAL_H
