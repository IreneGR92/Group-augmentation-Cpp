
#include <algorithm>
#include <cassert>
#include "Group.h"
#include "FishType.h"

using namespace std;


Group::Group() : breeder(BREEDER) {
    this->parameters = Parameters::instance();

    breederAlive = true;
    cumHelp = Parameters::NO_VALUE;
    fecundity = Parameters::NO_VALUE;
    realFecundity = Parameters::NO_VALUE;

    for (int i = 0; i < parameters->getInitNumHelpers(); ++i) {
        auto individual = Individual(HELPER);
        helpers.emplace_back(individual);
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

vector<Individual> Group::disperse() {

    vector<Individual> newFloaters;

    for (int i = 0; i < helpers.size();) {
        Individual helper = helpers[i];

        helper.calcDispersal();

        if (parameters->uniform(*parameters->getGenerator()) < helper.getDispersal()) {
            helper.setInherit(false); //the location of the individual is not the natal territory
            helper.setFishType(FLOATER);
            newFloaters.push_back(helper); //add the individual to the vector floaters in the last position
            helpers.removeIndividual(i);

        } else {
            helper.setFishType(HELPER); //individuals that stay or disperse to this group become helpers
            i++;

        }
        return newFloaters;
    }
}

std::vector<Individual> Group::reassignNoRelatedness() {

    std::vector<Individual> noRelatedHelpers;

    for (int i = 0; i < helpers.size();) {
        Individual helper = helpers[i];
        if (helper.getAge() == 1) { // all new offspring is assigned to new groups so no related to breeder

            helper.setInherit(false); //the location of the individual is not the natal territory
            noRelatedHelpers.push_back(helper); //add the individual to the vector in the last position
            helpers.removeIndividual(i); // removes the individual from the helpers vector

        } else {
            i++;
        }
    }
    return noRelatedHelpers;
}


/*  CALCULATE CUMULATIVE LEVEL OF HELP */

void Group::calculateCumulativeHelp() //Calculate accumulative help of all individuals inside of each group.
{
    cumHelp = 0;

    //Level of help for helpers
    for (Individual &helper:helpers) {
        helper.calcHelp();
        cumHelp += helper.getHelp();
    }
}

/*  MORTALITY */

void Group::survivalGroup() {
    this->calculateGroupSize();
    //Calculate survival for the helpers
    for (Individual &helper:helpers) {
        assert(helper.getFishType() == HELPER);
        helper.calculateSurvival(groupSize);
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

    vector<Individual, std::allocator<Individual>>::iterator helperIt;
    helperIt = helpers.begin();
    int sizeVectorHelpers = helpers.size();
    int counting = 0;
    while (!helpers.empty() && sizeVectorHelpers > counting) {

        //Mortality helpers
        if (parameters->uniform(*parameters->getGenerator()) > helperIt->getSurvival()) {
            *helperIt = helpers[helpers.size() - 1];
            helpers.pop_back();
            counting++;
            deaths++;
        } else
            helperIt++, counting++; //go to next individual
    }

    //Mortality breeder
    if (parameters->uniform(*parameters->getGenerator()) > breeder.getSurvival()) {
        breederAlive = false;
        deaths++;
    }
    this->calculateGroupSize(); //update group size after mortality
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

    vector<Individual *> candidates;
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
                candidates.push_back(&floaters[floaterSampledID]);
                temp = *itTempCandidates;
            }
        }
    } else if (!floaters.empty() && floaters.size() <
                                    proportionFloaters) { //TODO:When less floaters available than the sample size, takes all of them. Change to a proportion?
        vector<Individual, std::allocator<Individual>>::iterator floaterIt;
        for (floaterIt = floaters.begin(); floaterIt < floaters.end(); ++floaterIt) {
            candidates.push_back(&(*floaterIt));
        }
    }

    //    Join the helpers in the group to the sample of floaters
    vector<Individual, std::allocator<Individual>>::iterator helperIt;
    for (helperIt = helpers.begin(); helperIt < helpers.end(); ++helperIt) {
        candidates.push_back(&(*helperIt));
    }

    //    Choose breeder with higher likelihood for the highest age
    vector<Individual *, std::allocator<Individual *>>::iterator candidateIt;
    for (candidateIt = candidates.begin(); candidateIt < candidates.end(); ++candidateIt) {
        sumAge += (*candidateIt)->getAge(); //add all the age from the vector candidates
    }

    for (candidateIt = candidates.begin(); candidateIt < candidates.end(); ++candidateIt) {
        position.push_back(static_cast<double>((*candidateIt)->getAge()) / static_cast<double>(sumAge) +
                           currentPosition); //creates a vector with proportional segments to the age of each individual
        currentPosition = position[position.size() - 1];
    }

//    if (floaters.empty() && candidates.size() != helpers.size()) {
//        std::cout << "Error assigning empty floaters to Breeder" << endl;
//    }

    candidateIt = candidates.begin();
    int counting = 0;
    while (counting < candidates.size()) {
        if (RandP < position[candidateIt - candidates.begin()]) //to access the same ind in the candidates vector
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
            counting = candidates.size();//end loop
        } else
            ++candidateIt, ++counting;
    }
}


/* INCREASE AGE OF ALL GROUP INDIVIDUALS*/
void Group::increaseAge() {
    for(Individual &helper:helpers) {
        helper.increaseAge();
    }
    if (breederAlive) {
        breeder.increaseAge(breederAlive);
    }
}


/* REPRODUCTION */

void Group::reproduce(int generation) { // populate offspring generation
    //Calculate fecundity
    fecundity = parameters->getK0() + parameters->getK1() * cumHelp / (1 + cumHelp * parameters->getK1());

    poisson_distribution<int> PoissonFecundity(fecundity);
    realFecundity = PoissonFecundity(*parameters->getGenerator()); //integer number

    //Reproduction
    if (breederAlive) {
        for (int i = 0; i < realFecundity; i++) //number of offspring dependent on real fecundity
        {
            Individual offspring = Individual(breeder, HELPER, generation);

            helpers.emplace_back(
                    offspring); //create a new individual as helper in the group. Call construct to assign the mother genetic values to the offspring, construct calls Mutate function.
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

std::vector<double> Group::get(Attribute attribute, bool includeBreeder) const {
    std::vector<double> result;
    if (includeBreeder && isBreederAlive()) {
        result.push_back(breeder.get(attribute));
    }
    for (const Individual &helper: helpers) {
        result.push_back(helper.get(attribute));
    }
    return result;
}

std::vector<double> Group::get(Attribute attribute) const {
    return this->get(attribute, true);
}


bool Group::hasHelpers() const {
    return !helpers.empty();
}

void Group::addHelper(const Individual &helper) {
    this->helpers.emplace_back(helper);
}




