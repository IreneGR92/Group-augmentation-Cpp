
#include <iostream>

#include "Individual.h"
#include "FishType.h"
#include "../Parameters.h"

using namespace std;

Individual::Individual(Individual &individual, FishType fishType, Parameters &parameters,
                       std::default_random_engine &generator, int &generation) :
        Individual(individual.alpha, individual.alphaAge, individual.beta, individual.betaAge, individual.drift,
                   fishType,
                   generator, parameters, generation) {

    if (individual.fishType != BREEDER) {
        cout << "ERROR fishtype is not BREEDER";
    }
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

    this->fishType = fishType;

    this->inherit = true;
    this->age = 1;
    this->help = 0;
    this->survival = Parameters::NO_VALUE;
    this->dispersal = Parameters::NO_VALUE;

    if (generation != 0) {
        this->mutate(generation);
    }

}


/*DISPLAY LEVEL OF HELP*/
void Individual::calculateHelp() {
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

    if (parameters.isEvolutionHelpAfterDispersal()) {
        if (generation < parameters.getNumGenerations() / 4) {
            parameters.setMutationAlpha(0);
            parameters.setMutationAlphaAge(0);
        } else {
            parameters.setMutationAlpha(0.05);
            parameters.setMutationAlphaAge(0.05);
        }
    }

    if (parameters.uniform(generator) < parameters.getMutationAlpha()) {
        alpha += NormalA(generator);
    }
    if (parameters.isReactionNormHelp()) {
        if (parameters.uniform(generator) < parameters.getMutationAlphaAge()) {
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

void Individual::increaseAge(bool alive) {
    if (alive) {
        this->age++;
    } else {
        this->age = Parameters::NO_VALUE;
    }
}

void Individual::increaseAge() {
    this->increaseAge(true);
}


double Individual::getSurvival() const {
    return survival;
}

double Individual::getHelp() const {
    return help;
}

double Individual::getDispersal() const {
    return dispersal;
}

double Individual::getDrift() const {
    return drift;
}

double Individual::getBetaAge() const {
    return betaAge;
}

double Individual::getBeta() const {
    return beta;
}

double Individual::getAlphaAge() const {
    return alphaAge;
}

double Individual::getAlpha() const {
    return alpha;
}

FishType Individual::getFishType() const {
    return fishType;
}

int Individual::getAge() const {
    return age;
}

bool Individual::isInherit() const {
    return inherit;
}

void Individual::setHelp(double help) {
    Individual::help = help;
}

void Individual::setInherit(bool inherit) {
    Individual::inherit = inherit;
}

void Individual::setFishType(FishType fishType) {
    Individual::fishType = fishType;
}




