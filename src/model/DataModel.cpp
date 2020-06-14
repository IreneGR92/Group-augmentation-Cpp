#include "DataModel.h"

int DataModel::getDeaths() const {
    return deaths;
}

void DataModel::setDeaths(int deaths) {
    DataModel::deaths = deaths;
}

int DataModel::getNewBreederFloater() const {
    return newBreederFloater;
}

void DataModel::setNewBreederFloater(int newBreederFloater) {
    DataModel::newBreederFloater = newBreederFloater;
}

int DataModel::getNewBreederHelper() const {
    return newBreederHelper;
}

void DataModel::setNewBreederHelper(int newBreederHelper) {
    DataModel::newBreederHelper = newBreederHelper;
}

int DataModel::getInheritance() const {
    return inheritance;
}

void DataModel::setInheritance(int inheritance) {
    DataModel::inheritance = inheritance;
}

const IndividualVector &DataModel::getFloaters() const {
    return floaters;
}

const std::vector<Group> &DataModel::getGroups() const {
    return groups;
}

DataModel::DataModel() {

}

void DataModel::prepareGeneration() {
    deaths = 0; // to keep track of how many individuals die each generation
    newBreederFloater = 0;
    newBreederHelper = 0;
    inheritance = 0;
}

void DataModel::increaseDeath() {
    this->deaths++;
}

void DataModel::addDeath(int newDeath) {
    this->deaths += newDeath;
}
