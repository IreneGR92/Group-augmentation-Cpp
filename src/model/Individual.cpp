
#include <iostream>

#include "Individual.h"
#include "Classes.h"
#include "../Parameters.h"

using namespace std;

Individual::Individual(Individual &individual, Classes fishType, Parameters &parameters,
                       std::default_random_engine &generator,  int &generation) {
    if (individual.fishType != BREEDER) {
        cout << "ERROR fishtype is not BREEDER";
    }
    this->parameters = parameters;
    this->generator = generator;

    alpha = individual.alpha;
    alphaAge = individual.alphaAge;
    beta = individual.beta;
    betaAge = individual.betaAge;
    drift = individual.drift;


        if (generation != 0) {
            mutate(generation);
    }
    this->fishType = fishType;
    age = 1;
    inherit = true;

    survival = Parameters::NO_VALUE;
    help = 0;

    dispersal = Parameters::NO_VALUE;
}


Individual::Individual(double drift_, Classes fishType_, Parameters &parameters,
                       std::default_random_engine &generator,  int &generation) {
    this->parameters = parameters;
    this->generator = generator;


    alpha = parameters.getInitAlpha();
    alphaAge = parameters.getInitAlphaAge();
    beta = parameters.getInitBeta();
    betaAge = parameters.getInitBetaAge();
    drift = drift_;


        if (generation != 0) {
            mutate(generation);
    }
    fishType = fishType_;
    age = 1;
    inherit = true;

    survival = Parameters::NO_VALUE;
    help = 0;

    dispersal = Parameters::NO_VALUE;
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