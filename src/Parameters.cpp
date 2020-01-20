#include <iostream>
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include "Parameters.h"

using namespace std;

Parameters::Parameters() {

}

Parameters::Parameters(string url) {


    YAML::Node config = YAML::LoadFile(url);

    this->mainWriter = "main_" + this->getName(url) + ".txt";
    this->lastGenerationWriter = "last_generation_" + this->getName(url) + ".txt";

    this->name = this->getName(url);
    this->REACTION_NORM_HELP = config["REACTION_NORM_HELP"].as<bool>();
    this->REACTION_NORM_DISPERSAL = config["REACTION_NORM_DISPERSAL"].as<bool>();
    this->EVOLUTION_HELP_AFTER_DISPERSAL = config["EVOLUTION_HELP_AFTER_DISPERSAL"].as<bool>();
    this->LOW_SURVIVAL_BREEDER = config["LOW_SURVIVAL_BREEDER"].as<bool>();
    this->LOW_SURVIVAL_FLOATER = config["LOW_SURVIVAL_FLOATER"].as<bool>();
    this->NO_GROUP_AUGMENTATION = config["NO_GROUP_AUGMENTATION"].as<bool>();
    this->NO_RELATEDNESS = config["NO_RELATEDNESS"].as<bool>();
    this->LOGISTIC_SURVIVAL = config["LOGISTIC_SURVIVAL"].as<bool>();
    this->MAX_COLONIES = config["MAX_COLONIES"].as<int>();
    this->NUM_GENERATIONS = config["NUM_GENERATIONS"].as<int>();
    this->MAX_NUM_REPLICATES = config["MAX_NUM_REPLICATES"].as<int>();
    this->SKIP = config["SKIP"].as<int>();
    this->INIT_NUM_HELPERS = config["INIT_NUM_HELPERS"].as<int>();
    this->BIAS_FLOAT_BREEDER = config["BIAS_FLOAT_BREEDER"].as<double>();
    this->FIXED_GROUP_SIZE = config["FIXED_GROUP_SIZE"].as<double>();
    this->m = config["m"].as<double>();
    this->n = config["n"].as<double>();
    this->X0 = config["X0"].as<double>();
    this->Xsh = config["Xsh"].as<double>();
    this->Xsn = config["Xsn"].as<double>();
    this->K0 = config["K0"].as<double>();
    this->K1 = config["K1"].as<double>();
    this->INIT_ALPHA = config["INIT_ALPHA"].as<double>();
    this->INIT_ALPHA_AGE = config["INIT_ALPHA_AGE"].as<double>();
    this->MUTATION_ALPHA = config["MUTATION_ALPHA"].as<double>();
    this->MUTATION_ALPHA_AGE = config["MUTATION_ALPHA_AGE"].as<double>();
    this->STEP_ALPHA = config["STEP_ALPHA"].as<double>();
    this->INIT_BETA = config["INIT_BETA"].as<double>();
    this->INIT_BETA_AGE = config["INIT_BETA_AGE"].as<double>();
    this->MUTATION_BETA = config["MUTATION_BETA"].as<double>();
    this->MUTATION_BETA_AGE = config["MUTATION_BETA_AGE"].as<double>();
    this->STEP_BETA = config["STEP_BETA"].as<double>();
    this->MUTATION_DRIFT = config["MUTATION_DRIFT"].as<double>();
    this->STEP_DRIFT = config["STEP_DRIFT"].as<double>();


    this->driftUniform = uniform_real_distribution<double>(0, 100);
    this->uniform = uniform_real_distribution<double>(0, 1);


}

void Parameters::print() {
//    this->print(std::ofstream(mainWriter));
//    this->print(std::ofstream(lastGenerationWriter));
}

void Parameters::print(std::ofstream &outputStream) {
    outputStream << "PARAMETER VALUES" << endl

                 << "Reaction_norm_help?: " << "\t" << this->isReactionNormHelp() << endl
                 << "Reaction_norm_dispersal?: " << "\t" << this->isReactionNormDispersal() << endl
                 << "Evolution_help_after_dispersal?: " << "\t" << this->isEvolutionHelpAfterDispersal() << endl
                 << "Low_survival_breeder?: " << "\t" << this->isLowSurvivalBreeder() << endl
                 << "Low_survival_floater?: " << "\t" << this->isLowSurvivalFloater() << endl
                 << "No_group_augmentation?: " << "\t" << this->isNoGroupAugmentation() << endl
                 << "No_effect_relatedness?: " << "\t" << this->isNoRelatedness() << endl
                 << "Logistic_survival?: " << "\t" << this->isLogisticSurvival() << endl
                 << "Initial_population: " << "\t" << this->getMaxColonies() * (this->getInitNumHelpers() + 1) << endl
                 << "Number_of_colonies: " << "\t" << this->getMaxColonies() << endl
                 << "Number_generations: " << "\t" << this->getNumGenerations() << endl
                 << "Number_replicates: " << "\t" << this->getMaxNumReplicates() << endl
                 << "Bias_float_breeder: " << "\t" << this->getBiasFloatBreeder() << endl
                 << "m(predation): " << "\t" << this->getM() << endl
                 << "n(effect_size_mortality_dispersal): " << "\t" << this->getN() << endl
                 << "X0(intercept): " << "\t" << this->getX0() << endl
                 << "Xh(Cost_help_survival): " << "\t" << this->getXsh() << endl
                 << "Xn(Benefit_group_size_survival): " << "\t" << this->getXsn() << endl
                 << "K0(Base_fecundity): " << "\t" << this->getK0() << endl
                 << "K1(Benefit_help_fecundity): " << "\t" << this->getK1() << endl
                 << "initAlpha: " << "\t" << this->getInitAlpha() << endl
                 << "initAlphaAge: " << "\t" << this->getInitAlphaAge() << endl
                 << "initBeta: " << "\t" << this->getInitBeta() << endl
                 << "initBetaAge: " << "\t" << this->getInitBetaAge() << endl
                 << "mutAlpha: " << "\t" << this->getMutationAlpha() << endl
                 << "mutAlphaAge: " << "\t" << this->getMutationAlphaAge() << endl
                 << "mutBeta: " << "\t" << this->getMutationBeta() << endl
                 << "mutBetaAge: " << "\t" << this->getMutationBetaAge() << endl
                 << "mutDrift: " << "\t" << this->getMutationDrift() << endl
                 << "stepAlpha: " << "\t" << this->getStepAlpha() << endl
                 << "stepBeta: " << "\t" << this->getStepBeta() << endl
                 << "stepDrift: " << "\t" << this->getStepDrift() << endl << endl;
}


const string &Parameters::getName() const {
    return name;
}

bool Parameters::isReactionNormHelp() const {
    return REACTION_NORM_HELP;
}

bool Parameters::isReactionNormDispersal() const {
    return REACTION_NORM_DISPERSAL;
}

bool Parameters::isEvolutionHelpAfterDispersal() const {
    return EVOLUTION_HELP_AFTER_DISPERSAL;
}

bool Parameters::isLowSurvivalBreeder() const {
    return LOW_SURVIVAL_BREEDER;
}

bool Parameters::isLowSurvivalFloater() const {
    return LOW_SURVIVAL_FLOATER;
}

bool Parameters::isNoGroupAugmentation() const {
    return NO_GROUP_AUGMENTATION;
}

bool Parameters::isNoRelatedness() const {
    return NO_RELATEDNESS;
}

bool Parameters::isLogisticSurvival() const {
    return LOGISTIC_SURVIVAL;
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

int Parameters::getFixedGroupSize() const {
    return FIXED_GROUP_SIZE;
}

double Parameters::getM() const {
    return m;
}

double Parameters::getN() const {
    return n;
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

double Parameters::getMutationAlpha() const {
    return MUTATION_ALPHA;
}

double Parameters::getMutationAlphaAge() const {
    return MUTATION_ALPHA_AGE;
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


std::string Parameters::getName(std::string url) {


    unsigned first = url.find("parameters/");
    unsigned last = url.find(".yml");
    string name = url.substr(first, last - first);
    replace(name.begin(), name.end(), '/', '_');
    return std::string();
}

const string &Parameters::getMainWriter() const {
    return mainWriter;
}

const string &Parameters::getLastGenerationWriter() const {
    return lastGenerationWriter;
}





