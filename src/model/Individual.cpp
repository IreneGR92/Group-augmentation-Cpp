
#include <iostream>
#include <cassert>

#include "Individual.h"
#include "FishType.h"
#include "../Parameters.h"

using namespace std;

//Constructor for reproduction of a Breeder
Individual::Individual(Individual &individual, FishType fishType, int &generation) {

    assert(individual.fishType == BREEDER);

    this->alpha = individual.alpha;
    this->alphaAge = individual.alphaAge;
    this->beta = individual.beta;
    this->betaAge = individual.betaAge;
    this->drift = individual.getDrift();

    this->dispersal = Parameters::NO_VALUE;
    this->help = Parameters::NO_VALUE;

    this->initializeIndividual(fishType);

    this->mutate(generation);
}

//Constructor for initial creation
Individual::Individual(FishType fishType, int &generation) {

    auto param = Parameters::instance();

    this->alpha = param->getInitAlpha();
    this->alphaAge = param->getInitAlphaAge();
    this->beta = param->getInitBeta();
    this->betaAge = param->getInitBetaAge();
    this->drift = param->driftUniform(*param->getGenerator());
    this->initializeIndividual(fishType);
}

void Individual::initializeIndividual(FishType fishType) {
    this->parameters = Parameters::instance();
    this->dispersal = Parameters::NO_VALUE;
    this->help = 0;
    this->survival = Parameters::NO_VALUE;
    this->fishType = fishType;
    this->inherit = true;
    this->age = 1;
}

/* BECOME FLOATER (STAY VS DISPERSE) */

double Individual::calcDispersal() {
    if (!parameters->isReactionNormDispersal()) {
        dispersal = beta; // Range from 0 to 1 to compare to a Uniform distribution
    } else {
        dispersal = 1 / (1 + exp(betaAge * age - beta));
    }
    return dispersal;
}

/*DISPLAY LEVEL OF HELP*/

void Individual::calcHelp() {
    if (fishType == HELPER) {
        if (!parameters->isReactionNormHelp()) {
            help = alpha;

        } else {
            help = alpha + alphaAge * age;
        }

        if (help < 0) { help = 0; }

    } else {
        help = Parameters::NO_VALUE;
        cout << "Error: floaters get a help value" << endl;
    }
}


/*SURVIVAL*/

double Individual::calculateSurvival(int groupSize) {

    if (parameters->isLogisticSurvival()) {
        if (parameters->isNoGroupAugmentation()) {
            survival = (1 - parameters->getM()) / (1 + exp(-parameters->getX0() -
                                                           parameters->getXsn() * parameters->getFixedGroupSize() +
                                                           parameters->getXsh() * help));

        } else {
            if (parameters->isLowSurvivalFloater() && fishType == FLOATER) {
                survival = (1 - parameters->getM() * parameters->getN()) / (1 + exp(-parameters->getX0() -
                                                                                    parameters->getXsn() * groupSize +
                                                                                    parameters->getXsh() *
                                                                                    help));
            } else {
                survival = (1 - parameters->getM()) / (1 + exp(-parameters->getX0() - parameters->getXsn() * groupSize +
                                                               parameters->getXsh() * help));
            }
        }
    } else {
        if (parameters->isNoGroupAugmentation()) {
            survival = parameters->getX0() + parameters->getXsn() / (1 + exp(-(parameters->getFixedGroupSize()))) -
                       parameters->getXsh() / (1 + exp(-help));

        } else {
            if (parameters->isLowSurvivalFloater() && fishType == FLOATER) {
                survival = parameters->getX0(); //TODO:change?
            } else {
                survival = parameters->getX0() + parameters->getXsn() / (1 + exp(-(groupSize))) -
                           parameters->getXsh() / (1 + exp(-help));
            }
        }
        if (survival > 0.95) {
            survival = 0.95;
            //cout << "process camuflaged to selection" << endl;
        }
    }

    return survival;
}


void Individual::mutate(int generation) // mutate genome of offspring
{
    auto rng = *parameters->getGenerator();
    normal_distribution<double> NormalA(0,
                                        parameters->getStepAlpha()); //TODO: could be simplified if I decide to have all the steps size with the same magnitude
    normal_distribution<double> NormalB(0, parameters->getStepBeta());
    normal_distribution<double> NormalD(0, parameters->getStepDrift());
    double mutationAlpha;
    double mutationAlphaAge;

    if (parameters->isEvolutionHelpAfterDispersal() && generation < parameters->getNumGenerations() / 4) {
        mutationAlpha = 0;
        mutationAlphaAge = 0;
    } else {
        mutationAlpha = parameters->getMutationAlpha();
        mutationAlphaAge = parameters->getMutationAlphaAge();
    }

    if (parameters->uniform(rng) < mutationAlpha) {
        alpha += NormalA(rng);
    }
    if (parameters->isReactionNormHelp()) {
        if (parameters->uniform(rng) < mutationAlphaAge) {
            alphaAge += NormalA(rng);
        }
    }

    if (parameters->uniform(rng) < parameters->getMutationBeta()) {
        beta += NormalB(rng);
        if (!parameters->isReactionNormDispersal()) {
            if (beta < 0) { beta = 0; }
            else if (beta > 1) { beta = 1; }
        }
    }
    if (parameters->isReactionNormDispersal()) {
        if (parameters->uniform(rng) < parameters->getMutationBetaAge()) {
            betaAge += NormalB(rng);
        }
    }

    if (parameters->uniform(rng) < parameters->getMutationDrift()) {
        drift += NormalD(rng);
    }
}


/* INCREASE AGE */
//for breeders
void Individual::increaseAge(bool alive) {
    if (alive) {
        this->age++;
    } else {
        this->age = Parameters::NO_VALUE;
    }
}

//for helpers and floaters
void Individual::increaseAge() {
    this->increaseAge(true);
}


/* GETTERS AND SETTERS */

double Individual::getAlpha() const {
    return alpha;
}

double Individual::getAlphaAge() const {
    return alphaAge;
}

double Individual::getBeta() const {
    return beta;
}

double Individual::getBetaAge() const {
    return betaAge;
}

double Individual::getDrift() const {
    return drift;
}

double Individual::getDispersal() const {
    return dispersal;
}

double Individual::getHelp() const {
    return help;
}

void Individual::setHelp(double help) {
    Individual::help = help;
}

double Individual::getSurvival() const {
    return survival;
}

FishType Individual::getFishType() const {
    return fishType;
}

void Individual::setFishType(FishType fishType) {
    Individual::fishType = fishType;
    if (fishType == BREEDER) {
        this->dispersal = Parameters::NO_VALUE;
        this->help = Parameters::NO_VALUE;
    }


}

int Individual::getAge() const {
    return age;
}

bool Individual::isInherit() const {
    return inherit;
}

void Individual::setInherit(bool inherit) {
    Individual::inherit = inherit;
}





