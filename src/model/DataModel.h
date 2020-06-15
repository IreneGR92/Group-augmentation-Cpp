#ifndef GROUP_AUGMENTATION_DATAMODEL_H
#define GROUP_AUGMENTATION_DATAMODEL_H


#include "Group.h"

class DataModel {
    // to keep track of how many individuals die each generation
    int deaths = 0;

    int newBreederFloater = 0;
    int newBreederHelper = 0;
    int inheritance = 0; //to know if the new breeder was a helper or a floater


    const IndividualVector floaters;

    const std::vector<Group> groups;

public:
    void prepareGeneration();

    int getDeaths() const;

    void setDeaths(int deaths);

    int getNewBreederFloater() const;

    void setNewBreederFloater(int newBreederFloater);

    int getNewBreederHelper() const;

    void setNewBreederHelper(int newBreederHelper);

    int getInheritance() const;

    void setInheritance(int inheritance);

    const IndividualVector &getFloaters() const;

    const std::vector<Group> &getGroups() const;

    void increaseDeath();

    void increaseNewBreederFloater();

    void increaseNewBreederHelper();

    void increaseInheritance();

    void addDeath(int deaths);

    DataModel();
};


#endif //GROUP_AUGMENTATION_DATAMODEL_H
