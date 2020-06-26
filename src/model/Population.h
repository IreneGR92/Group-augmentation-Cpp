#ifndef GROUP_AUGMENTATION_DATAMODEL_H
#define GROUP_AUGMENTATION_DATAMODEL_H


#include "container/Container.h"
#include "Individual.h"
#include "Group.h"

class Population {
public:
    const Container<Group> &getGroups() const;

    const Container<Individual> &getFloaters() const;

private:
    Container<Group> groups;

    Container<Individual> floaters;
};


#endif //GROUP_AUGMENTATION_DATAMODEL_H

