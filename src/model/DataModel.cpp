#include "DataModel.h"

std::vector<Group> *DataModel::getGroups() const {
    return groups;
}

const IndividualVector *DataModel::getFloaters() const {
    return floaters;
}

DataModel::DataModel() {
    floaters = new IndividualVector;
    groups = new std::vector<Group>;

}

DataModel::~DataModel() {
    delete groups;
    delete floaters;
}
