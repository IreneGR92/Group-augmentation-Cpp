
#ifndef GROUP_AUGMENTATION_GROUP_H
#define GROUP_AUGMENTATION_GROUP_H


#include "Individual.h"
#include "../Parameters.h"

struct Group {

    Group(Parameters &parameters, std::default_random_engine &generator, int &generation);

    std::default_random_engine generator;
    Parameters parameters;
    double cumHelp;
    bool breederAlive;                                     // for the breeder: 1 alive, 0 dead
    bool helpersPresent;                                   //helpers present in group before reassign?
    int groupSize;
    double fecundity;
    int realFecundity;

    int generation;

    Individual breeder;
    std::vector<Individual> helpers; // create a vector of helpers inside each group

//Functions inside Group
    void disperse(std::vector<Individual> &floaters);

    void CumHelp();

    void survival(int &deaths);

    void newBreeder(std::vector<Individual> &floaters, int &newBreederFloater, int &newBreederHelper, int &inheritance);

    void increaseAge();

    void GroupSize();

    void reproduce();
};


#endif //GROUP_AUGMENTATION_GROUP_H
