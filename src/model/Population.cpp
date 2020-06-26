#include "Population.h"

const Container<Group> &Population::getGroups() const {
    return groups;
}

const IndividualContainer &Population::getFloaters() const {
    return floaters;
}

int Population::getDeaths() const {
    return deaths;
}

void Population::increaseDeath() {
    this->deaths++;
}

void Population::reset() {
    this->deaths = 0;
    this->inheritance = 0;
    this->newBreederHelper = 0;
    this->newBreederFloater = 0;
}
//this initializes groups and fills it with "maxColonies times" groups
Population::Population() : groups(Parameters::instance()->getMaxColonies(), Group()) {
}

