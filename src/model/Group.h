
#ifndef GROUP_AUGMENTATION_GROUP_H
#define GROUP_AUGMENTATION_GROUP_H


#include "Individual.h"
#include "../Parameters.h"
#include "container/Container.h"
#include "container/IndividualContainer.h"

class Group {

private:
    Parameters *parameters;
    double cumHelp;
    bool breederAlive;      // for the breeder: 1 alive, 0 dead
    bool helpersPresent;    //helpers present in group before reassign?
    int groupSize;
    double fecundity;
    int realFecundity;

    Individual breeder;
    Container<Individual> helpers; // create a vector of helpers inside each group

public:

    explicit Group();

    void calculateGroupSize();

    IndividualContainer disperse();


    void calculateCumulativeHelp();

    void survivalGroup();

    void mortalityGroup(int &deaths);

    void newBreeder(IndividualContainer &floaters, int &newBreederFloater, int &newBreederHelper, int &inheritance);

    void increaseAge();

    void reproduce(int generation);

    const Container<Individual> &getHelpers() const;

    const Individual &getBreeder() const;

    int getGroupSize() const;

    bool isBreederAlive() const;

    double getCumHelp() const;

    bool hasHelpers() const;

    std::vector<double> get(Attribute attribute) const;

    std::vector<double> get(Attribute attribute, bool includeBreeder) const;

    IndividualContainer reassignNoRelatedness();

};


#endif //GROUP_AUGMENTATION_GROUP_H
