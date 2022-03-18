
#include <iostream>
#include <cassert>

#include "Individual.h"
#include "FishType.h"

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
Individual::Individual(FishType fishType) {

    auto param = Parameters::instance();

    this->alpha = param->getInitAlpha();
    this->alphaAge = param->getInitAlphaAge();
    this->beta = param->getInitBeta();
    this->betaAge = param->getInitBetaAge();
    this->drift = param->driftUniform(*param->getGenerator());
    this->initializeIndividual(fishType);
}

void Individual::initializeIndividual(FishType type) {
    this->parameters = Parameters::instance();
    this->dispersal = Parameters::NO_VALUE;
    this->help = 0;
    this->survival = Parameters::NO_VALUE;
    this->fishType = type;
    this->inherit = true;
    this->age = 1;
}

/* BECOME FLOATER (STAY VS DISPERSE) */

void Individual::calcDispersal() {
    if (!parameters->isReactionNormDispersal()) {
        this->dispersal = beta;
    } else {
        this->dispersal = 1 / (1 + exp(betaAge * age - beta));
    }
}

void Individual::setGroupIndex(int groupIndex) {
    Individual::groupIndex = groupIndex;
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
        std::cout << "Error: floaters get a help value" << std::endl;
    }
}

/*SURVIVAL*/

void Individual::calculateSurvival(const int &groupSize) {

    if (parameters->isNoGroupAugmentation()) {
        if (fishType == FLOATER) {
            this->survival = (1 - parameters->getM() * parameters->getN()) /
                             (1 + exp(-parameters->getX0() - parameters->getXsn() * parameters->getFixedGroupSize() +
                                      parameters->getXsh() * this->help));
        } else {
            this->survival = (1 - parameters->getM()) /
                             (1 + exp(-parameters->getX0() - parameters->getXsn() * parameters->getFixedGroupSize() +
                                      parameters->getXsh() * this->help));
        }

    } else {
        if (fishType == FLOATER) {
            this->survival = (1 - parameters->getM() * parameters->getN()) /
                             (1 + exp(-parameters->getX0() - parameters->getXsn() * groupSize +
                                      parameters->getXsh() * this->help));
        } else {
            this->survival = (1 - parameters->getM()) /
                             (1 + exp(-parameters->getX0() - parameters->getXsn() * groupSize +
                                      parameters->getXsh() * this->help));
        }
    }
}


void Individual::mutate(int generation) // mutate genome of offspring
{
    auto rng = *parameters->getGenerator();
    std::normal_distribution<double> NormalA(0,
                                             parameters->getStepAlpha()); //TODO: could be simplified if I decide to have all the steps size with the same magnitude
    std::normal_distribution<double> NormalB(0, parameters->getStepBeta());
    std::normal_distribution<double> NormalD(0, parameters->getStepDrift());
    double mutationAlpha;
    double mutationAlphaAge;

    if (parameters->isEvolutionHelpAfterDispersal() && generation < 25000) {
        mutationAlpha = 0;
        mutationAlphaAge = 0;
    } else if (parameters->isNoRelatedness() && parameters->isNoRelatednessRandomGroup()){
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

void Individual::setHelp(double help_) {
    Individual::help = help_;
}

double Individual::getSurvival() const {
    return survival;
}

FishType Individual::getFishType() const {
    return fishType;
}

void Individual::setFishType(FishType type) {
    Individual::fishType = type;
    if (type == BREEDER) {
        this->dispersal = Parameters::NO_VALUE;
        this->help = 0;
    }
    if (type == FLOATER) {
        this->help = 0;
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

double Individual::get(Attribute type) const {
    switch (type) {
        case ALPHA:
            return this->alpha;
        case ALPHA_AGE:
            return this->alphaAge;
        case BETA:
            return this->beta;
        case BETA_AGE:
            return this->betaAge;
        case HELP:
            return this->help;
        case DISPERSAL:
            return this->dispersal;
        case SURVIVAL:
            return this->survival;
        case DRIFT:
            return this->drift;
        case AGE:
            return this->age;
    }

    assert(false);
}

int Individual::getGroupIndex() const {
    return groupIndex;
}

bool Individual::isViableBreeder() {
    if (age > parameters->getMinAgeBecomeBreeder()-1) {
        return true;
    } else {
        return false;
    }
}





