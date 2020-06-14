
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <iostream>
#include "Group.h"
#include "FishType.h"
#include "../Parameters.h"

using namespace std;


Group::Group(int &generation) : breeder(BREEDER, generation) {
    this->parameters = Parameters::instance();
    this->generation = generation;


    breederAlive = true;
    helpersPresent = false;
    cumHelp = Parameters::NO_VALUE;
    fecundity = Parameters::NO_VALUE;
    realFecundity = Parameters::NO_VALUE;

    for (int i = 0; i < parameters->getInitNumHelpers(); ++i) {

        helpers.emplace_back(
                Individual(HELPER, generation));
    }

    calculateGroupSize();
}

/* TOTAL NUMBER OF INDIVIDUALS PER GROUP*/

void Group::calculateGroupSize() {


    if (breederAlive) {
        groupSize = helpers.size() + 1;
    } else {
        groupSize = helpers.size();
    }
}

/*  DISPERSAL (STAY VS DISPERSE) */

vector<Individual> Group::disperse(vector<Individual> &floaters) {
    vector<Individual, std::allocator<Individual >>::iterator helper;
    helper = helpers.begin();
    int sizevec = helpers.size();
    int counting = 0;

    vector<Individual> noRelatedHelpers;

    while (!helpers.empty() && sizevec > counting) {
        helper->calcDispersal();

        if (parameters->uniform(*parameters->getGenerator()) < helper->getDispersal()) {
            helper->setInherit(false); //the location of the individual is not the natal territory
            floaters.push_back(*helper); //add the individual to the vector floaters in the last position
            floaters[floaters.size() - 1].setFishType(FLOATER);
            *helper = helpers[helpers.size() - 1]; // this and next line removes the individual from the helpers vector
            helpers.pop_back();
            ++counting;
        } else {
            if (parameters->isNoRelatedness() &&
                helper->getAge() == 1) { // all new offspring is assigned to new groups so no related to breeder
                helper->setInherit(false); //the location of the individual is not the natal territory
                helper->setFishType(HELPER);
                noRelatedHelpers.push_back(*helper); //add the individual to the vector in the last position
                *helper = helpers[helpers.size() -
                                  1]; // this and next line removes the individual from the helpers vector
                helpers.pop_back();
                ++counting;
            } else {
                helper->setFishType(HELPER); //individuals that stay or disperse to this group become helpers
                ++helper, ++counting;
            }
        }
    }
    return noRelatedHelpers;
}

/*  CALCULATE CUMULATIVE LEVEL OF HELP */

void Group::calculateCumulativeHelp() //Calculate accumulative help of all individuals inside of each group.
{
    cumHelp = 0;

    helpersPresent = false;
    if (!helpers.empty()) {
        helpersPresent = true;
    }

    //Level of help for helpers
    vector<Individual, std::allocator<Individual>>::iterator helperIt;
    for (helperIt = helpers.begin(); helperIt < helpers.end(); ++helperIt) {
        helperIt->calcHelp();
        cumHelp += helperIt->getHelp();
    }
    //Level of help for breeders
    //breeder.calcHelp();
    //cumHelp += breeder.help;
}

/*  MORTALITY */

void Group::survivalGroup() {
    //Calculate survival for the helpers
    this->calculateGroupSize();
    std::vector<Individual, std::allocator<Individual>>::iterator helperIt; //helpers
    for (helperIt = this->helpers.begin(); helperIt < this->helpers.end(); helperIt++) {
        assert(helperIt->getFishType() == HELPER);
        helperIt->calculateSurvival(groupSize);
    }

    //Calculate the survival of the breeder
    if (parameters->isLowSurvivalBreeder()) {
        this->breeder.calculateSurvival(0); //survival for breeder does not include group size benefits
    } // TODO:Change to 1?
    else {
        this->breeder.calculateSurvival(groupSize);
    }

}

void Group::mortalityGroup(int &deaths) {

    calculateGroupSize(); //update group size after dispersal

    vector<Individual, std::allocator<Individual>>::iterator helperIt;
    helperIt = helpers.begin();
    int sizevec = helpers.size();
    int counting = 0;
    while (!helpers.empty() && sizevec > counting) {

        //Mortality helpers
        if (parameters->uniform(*parameters->getGenerator()) > helperIt->getSurvival()) {
            *helperIt = helpers[helpers.size() - 1];
            helpers.pop_back();
            ++counting;
            deaths++;
        } else
            ++helperIt, ++counting; //go to next individual
    }

    //Mortality breeder
    if (parameters->uniform(*parameters->getGenerator()) > breeder.getSurvival()) {
        breederAlive = false;
        deaths++;
    }
}


/* BECOME BREEDER */

void Group::newBreeder(vector<Individual> &floaters, int &newBreederFloater, int &newBreederHelper, int &inheritance) {
    //    Select a random sample from the floaters
    int i = 0;
    int sumAge = 0;
    double currentPosition = 0; //age of the previous ind taken from Candidates
    int floaterSampledID;
    double RandP = parameters->uniform(*parameters->getGenerator());
    int proportionFloaters;
    proportionFloaters = round(floaters.size() * parameters->getBiasFloatBreeder() / parameters->getMaxColonies());

    vector<Individual *> Candidates;
    vector<double> position; //vector of age to choose with higher likelihood the ind with higher age
    vector<int> TemporaryCandidates; // to prevent taking the same ind several times in the sample

    if (!floaters.empty() && floaters.size() > proportionFloaters) {
        while (i < proportionFloaters) {
            uniform_int_distribution<int> UniformFloat(0, floaters.size() - 1); //random floater ID taken in the sample
            floaterSampledID = UniformFloat(*parameters->getGenerator());
            TemporaryCandidates.push_back(floaterSampledID); //add references of the floaters sampled to a vector
            sort(TemporaryCandidates.begin(), TemporaryCandidates.end()); //sort vector
            i++;
        }

        int temp = 0;
        vector<int, std::allocator<int>>::iterator itTempCandidates;
        for (itTempCandidates = TemporaryCandidates.begin();
             itTempCandidates < TemporaryCandidates.end(); ++itTempCandidates) {
            if (*itTempCandidates != temp) //to make sure the same ind is not taken more than ones
            {
                Candidates.push_back(&floaters[floaterSampledID]);
                temp = *itTempCandidates;
            }
        }
    } else if (!floaters.empty() && floaters.size() <
                                    proportionFloaters) { //TODO:When less floaters available than the sample size, takes all of them. Change to a proportion?
        vector<Individual, std::allocator<Individual>>::iterator floaterIt;
        for (floaterIt = floaters.begin(); floaterIt < floaters.end(); ++floaterIt) {
            Candidates.push_back(&(*floaterIt));
        }
    }

    //    Join the helpers in the group to the sample of floaters
    vector<Individual, std::allocator<Individual>>::iterator helperIt;
    for (helperIt = helpers.begin(); helperIt < helpers.end(); ++helperIt) {
        Candidates.push_back(&(*helperIt));
    }

    //    Choose breeder with higher likelihood for the highest age
    vector<Individual *, std::allocator<Individual *>>::iterator candidateIt;
    for (candidateIt = Candidates.begin(); candidateIt < Candidates.end(); ++candidateIt) {
        sumAge += (*candidateIt)->getAge(); //add all the age from the vector Candidates
    }

    for (candidateIt = Candidates.begin(); candidateIt < Candidates.end(); ++candidateIt) {
        position.push_back(static_cast<double>((*candidateIt)->getAge()) / static_cast<double>(sumAge) +
                           currentPosition); //creates a vector with proportional segments to the age of each individual
        currentPosition = position[position.size() - 1];
    }

//    if (floaters.empty() && Candidates.size() != helpers.size()) {
//        std::cout << "Error assigning empty floaters to Breeder" << endl;
//    }

    candidateIt = Candidates.begin();
    int counting = 0;
    while (counting < Candidates.size()) {
        if (RandP < position[candidateIt - Candidates.begin()]) //to access the same ind in the candidates vector
        {
            breeder = **candidateIt; //substitute the previous dead breeder
            breederAlive = true;

            if ((*candidateIt)->getFishType() == FLOATER) //delete the ind from the vector floaters
            {
                **candidateIt = floaters[floaters.size() - 1];
                floaters.pop_back();
                newBreederFloater++;
//                if ((*candidate3It)->inherit == 1) {
//                    std::cout << "error in inheritance" << endl;
//                }
            } else {
                **candidateIt = helpers[helpers.size() - 1]; //delete the ind from the vector helpers
                helpers.pop_back();
                newBreederHelper++;
                if ((*candidateIt)->isInherit() == 1) {
                    inheritance++;                    //calculates how many individuals that become breeders are natal to the territory
                }
            }

            breeder.setFishType(BREEDER); //modify the class
            counting = Candidates.size();//end loop
        } else
            ++candidateIt, ++counting;
    }
}


/* INCREASE AGE OF ALL GROUP INDIVIDUALS*/
void Group::increaseAge() {
    vector<Individual, std::allocator<Individual>>::iterator helperIt;
    for (helperIt = helpers.begin(); helperIt < helpers.end(); ++helperIt) {
        helperIt->increaseAge();
    }
    if (breederAlive) {
        breeder.increaseAge(breederAlive);
    }
}


/* REPRODUCTION */

void Group::reproduce(int generation) // populate offspring generation
{
    //Calculate fecundity
    fecundity = parameters->getK0() + parameters->getK1() * cumHelp / (1 + cumHelp * parameters->getK1());

    poisson_distribution<int> PoissonFecundity(fecundity);
    realFecundity = PoissonFecundity(*parameters->getGenerator()); //integer number

    //Reproduction
    if (breederAlive) {
        for (int i = 0; i < realFecundity; i++) //number of offspring dependent on real fecundity
        {
            helpers.emplace_back(breeder, HELPER,
                                 generation); //create a new individual as helper in the group. Call construct to assign the mother genetic values to the offspring, construct calls Mutate function.
        }
    }
}

const Individual &Group::getBreeder() const {
    return breeder;
}

const vector<Individual> &Group::getHelpers() const {
    return helpers;
}

int Group::getGroupSize() const {
    return groupSize;
}

bool Group::isBreederAlive() const {
    return breederAlive;
}

double Group::getCumHelp() const {
    return cumHelp;
}

bool Group::isHelpersPresent() const {
    return helpersPresent;
}




