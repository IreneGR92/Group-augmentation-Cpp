#include "Population.h"
#include <vector>

const std::vector<Group> &Population::getGroups() const {
    return groups;
}

const IndividualVector &Population::getFloaters() const {
    return floaters;
}

int Population::getDeaths() const {
    return deaths;
}


void Population::reset() {
    this->deaths = 0;
    this->inheritance = 0;
    this->newBreederHelper = 0;
    this->newBreederFloater = 0;
}

Population::Population() {
    for (int i = 0; i < Parameters::instance()->getMaxColonies(); i++) {
        Group group;
        this->groups.emplace_back(group);
    }
}


void Population::reassignFloaters() {
    std::uniform_int_distribution<int> UniformMaxCol(0, parameters->getMaxColonies() - 1);
    int selectGroup;
    while (!floaters.empty()) {
        Individual floater = floaters.at(floaters.size()-1);
        floater.setHelp(0);
        selectGroup = UniformMaxCol(*parameters->getGenerator());
        groups[selectGroup].helpers.emplace_back(floater); //add the floater to the helper vector in a randomly selected group
        floaters.pop_back(); //remove the floater from its vector
    }
}


void Population::disperse(int generation) {

    int groupID = 0;

    std::vector<Individual>  noRelatedHelpers;
    IndividualVector allNoRelatedHelpers;
    std::vector<int> noRelatednessGroupsID;


    for(Group &group: groups){
        //Dispersal
        this->floaters.merge(group.disperse());

        // In the non relatedness implementation, helpers just born are reassigned to random groups. Groups receive as many helpers as helpers left the group for reassignment.
        if (parameters->isNoRelatedness() && generation > 0) {
            noRelatedHelpers = group.reassignNoRelatedness();
            for (int i = 0; i < noRelatedHelpers.size(); i++) {
                noRelatednessGroupsID.push_back(groupID);
            }
            allNoRelatedHelpers.merge(noRelatedHelpers);
            groupID++;
        }
    }

    if (parameters->isNoRelatedness() && !allNoRelatedHelpers.empty()) {

        int selectGroupID, selectGroupIndex;
        while (!allNoRelatedHelpers.empty()) {
            std::uniform_int_distribution<int> UniformGroupID(0, noRelatednessGroupsID.size() - 1);
            selectGroupIndex = UniformGroupID(
                    *parameters->getGenerator()); // selects a random index the noRelatednessGroupsID vector
            selectGroupID = noRelatednessGroupsID[selectGroupIndex]; // translates the index to the ID of a group from the noRelatednessGroupsID vector
            noRelatednessGroupsID.erase(noRelatednessGroupsID.begin()+selectGroupIndex); //remove the group ID from the vector to not draw it again
            groups[selectGroupID].helpers.emplace_back(allNoRelatedHelpers[allNoRelatedHelpers.size()-1]); //add the no related helper to the helper vector in a randomly selected group
            allNoRelatedHelpers.pop_back(); //remove the no related helper from its vector
        }
    }

//    if (parameters->isNoRelatedness() && !allNoRelatedHelpers.empty()) {
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
}


void Population::help() {
    for (Group &group:groups) {
        //Calculate help & cumulative help for group
        group.calculateCumulativeHelp();
    }
}

void Population::survival() {
    for (Group &group:groups) {
        group.survivalGroup();
    }
    this->survivalFloaters();
}


void Population::survivalFloaters() {
    for (Individual &floater:floaters) {
        floater.calculateSurvival(0); // TODO:Change to 1?
    }
}

void Population::mortality() {
    for (Group &group:groups) {
        group.mortalityGroup(deaths);
    }
    this->mortalityFloaters();

}

void Population::mortalityFloaters() {

    std::vector<Individual, std::allocator<Individual>>::iterator floaterIt;
    floaterIt = floaters.begin();
    int size = floaters.size();
    for (int count = 0; !floaters.empty() && size > count; count++) {

        //Mortality floaters
        if (parameters->uniform(*parameters->getGenerator()) > floaterIt->getSurvival()) {
            *floaterIt = floaters[floaters.size() - 1];
            floaters.pop_back();
            deaths++;
        } else {
            floaterIt++;
        }
    }
}

void Population::newBreeder() {
    for (Group &group:groups) {
        if (!group.isBreederAlive()) {
            group.newBreeder(floaters, newBreederFloater, newBreederHelper, inheritance);
        }
    }
}

void Population::increaseAge() {
    for (Group &group:groups) {
        group.increaseAge();
    }
    this->increaseAgeFloaters();
}

void Population::increaseAgeFloaters() {
    for (Individual &floater:floaters) {
        floater.increaseAge();
    }
}

void Population::reproduce(int generation) {
    for (Group &group:groups) {
        group.reproduce(generation);
    }
}



