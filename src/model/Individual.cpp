
#include <iostream>

#include "Individual.h"
#include "FishType.h"
#include "../Parameters.h"

using namespace std;

Individual::Individual(Individual &individual, FishType fishType, Parameters &parameters,
                       std::default_random_engine &generator, int &generation) :
        Individual(individual.alpha, individual.alphaAge, individual.beta, individual.betaAge, individual.drift,
                   fishType,
                   generator, parameters, generation) {  // TODO: is the order correct?

    if (individual.fishType != BREEDER) {
        cout << "ERROR fishtype is not BREEDER" << endl;
    }
    this->mutate(generation);
}

Individual::Individual(double drift, FishType fishType, Parameters &parameters,
                       std::default_random_engine &generator, int &generation) :
        Individual(parameters.getInitAlpha(), parameters.getInitAlphaAge(), parameters.getInitBeta(),
                   parameters.getInitBetaAge(), drift, fishType, generator, parameters, generation) {
}

//PRIVATE
Individual::Individual(double alpha, double alphaAge, double beta, double betaAge, double drift, FishType fishType,
                       std::default_random_engine &generator, Parameters &parameters, int &generation) {
    this->parameters = parameters;
    this->generator = generator;

    this->alpha = alpha;
    this->alphaAge = alphaAge;
    this->beta = beta;
    this->betaAge = betaAge;
    this->drift = drift;

    this->dispersal = Parameters::NO_VALUE;
    this->help = 0;
    this->survival = Parameters::NO_VALUE;
    this->fishType = fishType;
    this->inherit = true;
    this->age = 1;
}


/* BECOME FLOATER (STAY VS DISPERSE) */

double Individual::calcDispersal() {
    if (parameters.isNoRelatedness() && age == 1) {
        dispersal = 1;

    } else {
        if (!parameters.isReactionNormDispersal()) {

            dispersal = beta; // Range from 0 to 1 to compare to a Uniform distribution
        } else {
            dispersal = 1 / (1 + exp(betaAge * age - beta));
        }
    }

    return dispersal;
}

/*DISPLAY LEVEL OF HELP*/

void Individual::calcHelp() {
    if (fishType == HELPER) {
        if (!parameters.isReactionNormHelp()) {
            help = alpha;

        } else {
            help = alpha + alphaAge * age;
        }

        if (help < 0) { help = 0; }

    } else {
        help = NAN;
        cout << "Error: floaters get a help value" << endl;
    }
}


/*SURVIVAL*/

double Individual::calcSurvival(int groupSize) {

    if (parameters.isLogisticSurvival()) {
        if (parameters.isNoGroupAugmentation()) {
            survival = (1 - parameters.getM()) / (1 + exp(-parameters.getX0() -
                                                          parameters.getXsn() * parameters.getFixedGroupSize() +
                                                          parameters.getXsh() * help));

        } else {
            if (parameters.isLowSurvivalFloater() && fishType == FLOATER) {
                survival = (1 - parameters.getM() * parameters.getN()) / (1 + exp(-parameters.getX0() -
                                                                                  parameters.getXsn() * groupSize +
                                                                                  parameters.getXsh() *
                                                                                  help)); //TODO: change?
            } else {
                survival = (1 - parameters.getM()) / (1 + exp(-parameters.getX0() - parameters.getXsn() * groupSize +
                                                              parameters.getXsh() * help));
            }
        }
    } else {
        if (parameters.isNoGroupAugmentation()) {
            survival = parameters.getX0() + parameters.getXsn() / (1 + exp(-(parameters.getFixedGroupSize()))) -
                       parameters.getXsh() / (1 + exp(-help));

        } else {
            if (parameters.isLowSurvivalFloater() && fishType == FLOATER) {
                survival = parameters.getX0(); //TODO:change?
            } else {
                survival = parameters.getX0() + parameters.getXsn() / (1 + exp(-(groupSize))) -
                           parameters.getXsh() / (1 + exp(-help));
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
    normal_distribution<double> NormalA(0,
                                        parameters.getStepAlpha()); //TODO: could be simplified if I decide to have all the steps size with the same magnitude
    normal_distribution<double> NormalB(0, parameters.getStepBeta());
    normal_distribution<double> NormalD(0, parameters.getStepDrift());
    double mutationAlpha;
    double mutationAlphaAge;

    if (parameters.isEvolutionHelpAfterDispersal() && generation < parameters.getNumGenerations() / 4) {
        mutationAlpha = 0;
        mutationAlphaAge = 0;
    } else {
        mutationAlpha = parameters.getMutationAlpha();
        mutationAlphaAge = parameters.getMutationAlphaAge();
    }

    if (parameters.uniform(generator) < mutationAlpha) {
        alpha += NormalA(generator);
    }
    if (parameters.isReactionNormHelp()) {
        if (parameters.uniform(generator) < mutationAlphaAge) {
            alphaAge += NormalA(generator);
        }
    }

    if (parameters.uniform(generator) < parameters.getMutationBeta()) {
        beta += NormalB(generator);
        if (!parameters.isReactionNormDispersal()) {
            if (beta < 0) { beta = 0; }
            else if (beta > 1) { beta = 1; }
        }
    }
    if (parameters.isReactionNormDispersal()) {
        if (parameters.uniform(generator) < parameters.getMutationBetaAge()) {
            betaAge += NormalB(generator);
        }
    }

    if (parameters.uniform(generator) < parameters.getMutationDrift()) {
        drift += NormalD(generator);
    }
}


/* INCREASE AGE */
//TODO: is this for breeders?
void Individual::increaseAge(bool alive) {
    if (alive) {
        this->age++;
    } else {
        this->age = Parameters::NO_VALUE;
    }
}

//TODO: individuals that are not breeders do not have a alive value, for breeders this is a variable of group
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





