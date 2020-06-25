#ifndef GROUP_AUGMENTATION_DATAMODEL_H
#define GROUP_AUGMENTATION_DATAMODEL_H


#include <vector>
#include "Group.h"

class DataModel {
private:
        std::vector<Group> *groups;

    IndividualVector *floaters;

public:


    DataModel();

    virtual ~DataModel();

    std::vector<Group> *getGroups() const;

    const IndividualVector *getFloaters() const;

};


#endif //GROUP_AUGMENTATION_DATAMODEL_H
