
#ifndef GROUP_AUGMENTATION_GROUP_H
#define GROUP_AUGMENTATION_GROUP_H


#include "Individual.h"
#include "../Parameters.h"
#include "container/Container.h"

class Group {

private:
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
    Container<Individual> helpers; // create a vector of helpers inside each group

    explicit Group(int &generation);

    void calculateGroupSize();

    std::vector<Individual> disperse();


    void calculateCumulativeHelp();

    void survivalGroup();

    void mortalityGroup(int &deaths);

    void newBreeder(std::vector<Individual> &floaters, int &newBreederFloater, int &newBreederHelper, int &inheritance);

    void increaseAge();

    void reproduce(int generation);

    const Container<Individual> &getHelpers() const;

    const Individual &getBreeder() const;

    int getGroupSize() const;

    bool isBreederAlive() const;

    double getCumHelp() const;

    std::vector<double> get(Attribute attribute) const;

    std::vector<double> get(Attribute attribute, bool includeBreeder) const;

    std::vector<Individual> reassignNoRelatedness();
};


#endif //GROUP_AUGMENTATION_GROUP_H
