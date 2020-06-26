#ifndef GROUP_AUGMENTATION_DATAMODEL_H
#define GROUP_AUGMENTATION_DATAMODEL_H


#include "container/Container.h"
#include "Individual.h"
#include "Group.h"

class Population {
public:
    const Container<Group> &getGroups() const;

    const Container<Individual> &getFloaters() const;

    Population();

private:
    Container<Group> groups;

    Container<Individual> floaters;

    int deaths = 0;

    int newBreederFloater = 0, newBreederHelper = 0, inheritance = 0; //to know if the new breeder was a helper or a floater

public:
    int getDeaths() const;

    void increaseDeath();

    void reset();

public:
};


#endif //GROUP_AUGMENTATION_DATAMODEL_H

