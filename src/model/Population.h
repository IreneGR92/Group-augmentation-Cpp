#ifndef GROUP_AUGMENTATION_DATAMODEL_H
#define GROUP_AUGMENTATION_DATAMODEL_H


#include "container/Container.h"
#include "container/IndividualContainer.h"
#include "Individual.h"
#include "Group.h"

class Population {
public:
    const Container<Group> &getGroups() const;

    const IndividualContainer &getFloaters() const;

    Population();

private:
    Parameters *parameters = Parameters::instance();

    Container<Group> groups;

    IndividualContainer floaters;

    int deaths = 0;

    int newBreederFloater = 0, newBreederHelper = 0, inheritance = 0; //to know if the new breeder was a helper or a floater

public:

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

    void increaseDeath();

    void mortalityFloaters();

    void reset();

public:
};


#endif //GROUP_AUGMENTATION_DATAMODEL_H

