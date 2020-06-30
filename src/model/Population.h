#ifndef GROUP_AUGMENTATION_DATAMODEL_H
#define GROUP_AUGMENTATION_DATAMODEL_H

#include "container/IndividualVector.h"
#include "Individual.h"
#include "Group.h"

class Population {


private:
    Parameters *parameters = Parameters::instance();

    std::vector<Group> groups;

    IndividualVector floaters;

    int deaths = 0;

    int newBreederFloater = 0, newBreederHelper = 0, inheritance = 0;
public:
    int getNewBreederFloater() const;

    int getNewBreederHelper() const;

    int getInheritance() const;
    //to know if the new breeder was a helper or a floater

public:
    const std::vector<Group> &getGroups() const;

    const IndividualVector &getFloaters() const;

    Population();

    void reassignFloaters();

    void disperse(int generation);

    void help();

    void survival();

    void mortality();

    void newBreeder();

    void increaseAge();

    void increaseAgeFloaters();

    void reproduce(int generation);

    void survivalFloaters();

    int getDeaths() const;

    void mortalityFloaters();

    void reset();



public:
};


#endif //GROUP_AUGMENTATION_DATAMODEL_H

