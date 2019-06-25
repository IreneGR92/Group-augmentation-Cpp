//
// Created by odin on 25/06/19.
//


#include <iostream>
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include "Parameters.h"

using namespace std;

Parameters::Parameters() {
    string url = "/home/odin/git/cpp/parameters/default.yml";
    YAML::Node config = YAML::LoadFile("/home/odin/git/cpp/parameters/NRN/init-NRN.yml");

    unsigned first = url.find("parameters/");
    unsigned last = url.find(".yml");
    string name = url.substr(first, last - first);
    replace(name.begin(), name.end(), '/', '_');

    this->name = name;
    this->REACTION_NORM_HELP = config["REACTION_NORM_HELP"].as<bool>();
    this->REACTION_NORM_DISPERSAL = config["REACTION_NORM_DISPERSAL"].as<bool>();
    this->NO_RELATEDNESS = config["NO_RELATEDNESS"].as<bool>();
    this->EVOLUTION_HELP_AFTER_DISPERSAL = config["EVOLUTION_HELP_AFTER_DISPERSAL"].as<bool>();
    this->OLD_SURVIVAL_FORMULA = config["OLD_SURVIVAL_FORMULA"].as<bool>();
    this->MAX_COLONIES = config["MAX_COLONIES"].as<int>();
    this->NUM_GENERATIONS = config["NUM_GENERATIONS"].as<int>();
    this->MAX_NUM_REPLICATES = config["MAX_NUM_REPLICATES"].as<int>();
    this->SKIP = config["SKIP"].as<int>();
    this->INIT_NUM_HELPERS = config["INIT_NUM_HELPERS"].as<int>();
    this->BIAS_FLOAT_BREEDER = config["BIAS_FLOAT_BREEDER"].as<double>();
    this->X0 = config["X0"].as<double>();
    this->Xsh = config["Xsh"].as<double>();
    this->Xsn = config["Xsn"].as<double>();
    this->K0 = config["K0"].as<double>();
    this->K1 = config["K1"].as<double>();
    this->INIT_ALPHA = config["INIT_ALPHA"].as<double>();
    this->INIT_ALPHA_AGE = config["INIT_ALPHA_AGE"].as<double>();
    this->INIT_ALPHA_AGE2 = config["INIT_ALPHA_AGE2"].as<double>();
    this->MUTATION_ALPHA = config["MUTATION_ALPHA"].as<double>();
    this->MUTATION_ALPHA_AGE = config["MUTATION_ALPHA_AGE"].as<double>();
    this->MUTATION_ALPHA_AGE2 = config["MUTATION_ALPHA_AGE2"].as<double>();
    this->STEP_ALPHA = config["STEP_ALPHA"].as<double>();
    this->INIT_BETA = config["INIT_BETA"].as<double>();
    this->INIT_BETA_AGE = config["INIT_BETA_AGE"].as<double>();
    this->MUTATION_BETA = config["MUTATION_BETA"].as<double>();
    this->MUTATION_BETA_AGE = config["MUTATION_BETA_AGE"].as<double>();
    this->STEP_BETA = config["STEP_BETA"].as<double>();
    this->MUTATION_DRIFT = config["MUTATION_DRIFT"].as<double>();
    this->STEP_DRIFT = config["STEP_DRIFT"].as<double>();


}

const string &Parameters::getName() const {
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

