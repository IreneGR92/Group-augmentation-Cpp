#include "Population.h"

const Container<Group> &Population::getGroups() const {
    return groups;
}

const Container<Individual> &Population::getFloaters() const {
    return floaters;
}
