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

void Population::help() {
    //Help & Survival
    std::vector<Group, std::allocator<Group>>::iterator groupIt;
    for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
        //Calculate help
        groupIt->calculateCumulativeHelp();
    }
}

void Population::survival() {
    //Survival
    std::vector<Group, std::allocator<Group>>::iterator groupIt;
    for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
        groupIt->survivalGroup();
    }
    this->survivalFloaters();
}


void Population::survivalFloaters() {
    //Calculate survival for floaters
    std::vector<Individual, std::allocator<Individual>>::iterator floatersIt;
    for (floatersIt = floaters.begin(); floatersIt < floaters.end(); ++floatersIt) {
        floatersIt->calculateSurvival(0); // TODO:Change to 1?
    }
}

void Population::mortality() {
    //Mortality of helpers and breeders
    std::vector<Group, std::allocator<Group>>::iterator groupIt;
    for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
        groupIt->mortalityGroup(deaths);
    }
    this->mortalityFloaters();

}

void Population::mortalityFloaters() { //Calculate the survival of the floaters

    std::vector<Individual, std::allocator<Individual>>::iterator floaterIt;
    floaterIt = floaters.begin();
    int size = floaters.size();
    for (int count = 0; !floaters.isEmpty() && size > count; count++) {

        //Mortality floaters
        if (parameters->uniform(*parameters->getGenerator()) > floaterIt->getSurvival()) {
            *floaterIt = floaters.accessElement(floaters.size() - 1);
            floaters.removeLast();
            deaths++;
        } else {
            floaterIt++; //go to next individual
        }
    }
}

void Population::newBreeder() {
    std::vector<Group, std::allocator<Group>>::iterator groupIt;
    for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
        if (!groupIt->isBreederAlive()) {
            groupIt->newBreeder(floaters, newBreederFloater, newBreederHelper, inheritance);
        }
    }
}

void Population::increaseAge() {
    std::vector<Group, std::allocator<Group>>::iterator groupIt;
    for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
        groupIt->increaseAge();
    }
    this->increaseAgeFloaters();
}

void Population::increaseAgeFloaters() {
    std::vector<Individual, std::allocator<Individual>>::iterator floatersIt;
    for (floatersIt = floaters.begin(); floatersIt < floaters.end(); ++floatersIt) {
        floatersIt->increaseAge();
    }
}

void Population::reproduce(int generation) {
    std::vector<Group, std::allocator<Group>>::iterator group;
    for (group = groups.begin(); group < groups.end(); ++group) {
        group->reproduce(generation);
    }
}

void Population::disperse(int generation) {

    int groupID = 0;

    IndividualContainer noRelatedHelpers;
    IndividualContainer allNoRelatedHelpers;
    std::vector<int> noRelatednessGroupsID;


    std::vector<Group, std::allocator<Group>>::iterator group;
    for (group = groups.begin(); group < groups.end(); ++group) {
        //Dispersal
        this->floaters.merge(group->disperse());

        // In the non relatedness implementation, helpers just born are reassigned to random groups. Groups receive as many helpers as helpers left the group for reassignment.
        if (parameters->isNoRelatedness() && generation > 0) {
            noRelatedHelpers = group->reassignNoRelatedness();
            for (int i = 0; i < noRelatedHelpers.size(); i++) {
                noRelatednessGroupsID.push_back(groupID);
            }
            allNoRelatedHelpers.merge(noRelatedHelpers);
            groupID++;
        }
    }

//        if (parameters->isNoRelatedness() && !allNoRelatedHelpers.empty()) {
//        std::uniform_int_distribution<int> UniformMaxCol(0, parameters->getMaxColonies() - 1);
//        int selectGroup;
//        std::vector<Individual>::iterator NoRelatedHelperIt;
//        while (!allNoRelatedHelpers.empty()) {
//            NoRelatedHelperIt = allNoRelatedHelpers.end() - 1;
//            selectGroup = UniformMaxCol(*parameters->getGenerator());
//            groups[selectGroup].helpers.push_back(
//                    *NoRelatedHelperIt); //add the no related helper to the helper vector in a randomly selected group
//            allNoRelatedHelpers.pop_back(); //remove the no related helper from its vector
//        }
//    }

    if (parameters->isNoRelatedness() && !allNoRelatedHelpers.isEmpty()) {

        int selectGroupID, selectGroupIndex, size;
        std::vector<Individual>::iterator NoRelatedHelperIt;
        while (!allNoRelatedHelpers.isEmpty()) {
            std::uniform_int_distribution<int> UniformGroupID(0, noRelatednessGroupsID.size() - 1);
            NoRelatedHelperIt = allNoRelatedHelpers.end() - 1;
            selectGroupIndex = UniformGroupID(
                    *parameters->getGenerator()); // selects a random index the noRelatednessGroupsID vector
            selectGroupID = noRelatednessGroupsID[selectGroupIndex]; // translates the index to the ID of a group from the noRelatednessGroupsID vector
            noRelatednessGroupsID.erase(noRelatednessGroupsID.begin() +
                                        selectGroupIndex); //remove the group ID from the vector to not draw it again
            groups.accessElement(selectGroupID).getHelpers().add(
                    *NoRelatedHelperIt); //add the no related helper to the helper vector in a randomly selected group
            allNoRelatedHelpers.removeLast(); //remove the no related helper from its vector
        }
    }
}


void Population::reassignFloaters() {
//FIXME implement this
//    for (Individual individual:floaters) {
//        Group randomGroup = groups.getRandomElement();
//
//        randomGroup.getHelpers().add(individual);
//    }

    std::uniform_int_distribution<int> UniformMaxCol(0, parameters->getMaxColonies() - 1);
    int selectGroup;
    std::vector<Individual>::iterator floaterIt;
    while (!floaters.isEmpty()) {
        floaterIt = floaters.end() - 1;
        floaterIt->setHelp(0);
        selectGroup = UniformMaxCol(*parameters->getGenerator());
        groups.accessElement(selectGroup).getHelpers().add(
                *floaterIt); //add the floater to the helper vector in a randomly selected group
        floaters.removeLast(); //remove the floater from its vector
    }
}


