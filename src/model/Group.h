
#ifndef GROUP_AUGMENTATION_GROUP_H
#define GROUP_AUGMENTATION_GROUP_H


#include "Individual.h"
#include "../Parameters.h"
#include "IndividualVector.h"

class Group {


    Parameters *parameters;
    double cumHelp;
    bool breederAlive;      // for the breeder: 1 alive, 0 dead
    bool helpersPresent;    //helpers present in group before reassign?
    int groupSize;
    double fecundity;
    int realFecundity;

    int generation;


public:
    Individual breeder;
    IndividualVector helpers; // create a vector of helpers inside each group

    explicit Group(int &generation);

    void calculateGroupSize();

    std::vector<Individual> disperse(std::vector<Individual> &floaters);


    void calculateCumulativeHelp();

    void survivalGroup();

    void mortalityGroup(int &deaths);

    void newBreeder(std::vector<Individual> &floaters, int &newBreederFloater, int &newBreederHelper, int &inheritance);

    void increaseAge();

    void reproduce(int generation);

    const std::vector<Individual> &getHelpers() const;

    const Individual &getBreeder() const;

    int getGroupSize() const;

    bool isBreederAlive() const;

    double getCumHelp() const;

    bool isHelpersPresent() const;

    std::vector<double> get(Attribute attribute) const;

    std::vector<double> get(Attribute attribute, bool includeBreeder) const;
};


#endif //GROUP_AUGMENTATION_GROUP_H
