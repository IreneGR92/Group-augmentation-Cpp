//
// Created by odin on 25/06/19.
//

#include "Parameters.h"

const std::string &Parameters::getName() const {
    return name;
}

bool Parameters::isReactionNormHelp() const {
    return REACTION_NORM_HELP;
}

bool Parameters::isNoRelatedness() const {
    return NO_RELATEDNESS;
}

bool Parameters::isEvolutionHelpAfterDispersal() const {
    return EVOLUTION_HELP_AFTER_DISPERSAL;
}

bool Parameters::isOldSurvivalFormula() const {
    return OLD_SURVIVAL_FORMULA;
}

int Parameters::getMaxColonies() const {
    return MAX_COLONIES;
}

int Parameters::getNumGenerations() const {
    return NUM_GENERATIONS;
}

int Parameters::getMaxNumReplicates() const {
    return MAX_NUM_REPLICATES;
}

int Parameters::getSkip() const {
    return SKIP;
}

int Parameters::getInitNumHelpers() const {
    return INIT_NUM_HELPERS;
}

double Parameters::getBiasFloatBreeder() const {
    return BIAS_FLOAT_BREEDER;
}

double Parameters::getX0() const {
    return X0;
}

double Parameters::getXsh() const {
    return Xsh;
}

double Parameters::getXsn() const {
    return Xsn;
}

double Parameters::getK0() const {
    return K0;
}

double Parameters::getK1() const {
    return K1;
}

double Parameters::getInitAlpha() const {
    return INIT_ALPHA;
}

double Parameters::getInitAlphaAge() const {
    return INIT_ALPHA_AGE;
}

double Parameters::getInitAlphaAge2() const {
    return INIT_ALPHA_AGE2;
}

double Parameters::getMutationAlpha() const {
    return MUTATION_ALPHA;
}

double Parameters::getMutationAlphaAge() const {
    return MUTATION_ALPHA_AGE;
}

double Parameters::getMutationAlphaAge2() const {
    return MUTATION_ALPHA_AGE2;
}

double Parameters::getStepAlpha() const {
    return STEP_ALPHA;
}

double Parameters::getInitBeta() const {
    return INIT_BETA;
}

double Parameters::getInitBetaAge() const {
    return INIT_BETA_AGE;
}

double Parameters::getMutationBeta() const {
    return MUTATION_BETA;
}

double Parameters::getMutationBetaAge() const {
    return MUTATION_BETA_AGE;
}

double Parameters::getStepBeta() const {
    return STEP_BETA;
}

double Parameters::getMutationDrift() const {
    return MUTATION_DRIFT;
}

double Parameters::getStepDrift() const {
    return STEP_DRIFT;
}

void Parameters::setMutationAlpha(double mutationAlpha) {
    MUTATION_ALPHA = mutationAlpha;
}

void Parameters::setMutationAlphaAge(double mutationAlphaAge) {
    MUTATION_ALPHA_AGE = mutationAlphaAge;
}

void Parameters::setMutationAlphaAge2(double mutationAlphaAge2) {
    MUTATION_ALPHA_AGE2 = mutationAlphaAge2;
}

bool Parameters::isReactionNormDispersal() const {
    return REACTION_NORM_DISPERSAL;
}
