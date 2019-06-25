
#ifndef CPP_PARAMETERS_H
#define CPP_PARAMETERS_H


#include <string>
#include <fstream>

class Parameters {


    std::string name;
///Run parameters
    bool REACTION_NORM_HELP;    //Apply reaction norm to age for dispersal?
    bool NO_RELATEDNESS;       //Apply implementation to remove the effect of relatedness?

    bool REACTION_NORM_DISPERSAL; // Apply reaction norm to age for dispersal?
    bool EVOLUTION_HELP_AFTER_DISPERSAL; // help evolves only after the evolution of dispersal?
    bool OLD_SURVIVAL_FORMULA;

    int MAX_COLONIES;     // max number of groups or colonies --> breeding spots.
    int NUM_GENERATIONS;
    int MAX_NUM_REPLICATES;
    int SKIP;   // interval between print-outs

//Fix values
    int INIT_NUM_HELPERS;     //initial number of helpers per group
    double BIAS_FLOAT_BREEDER; //mean of number of groups a floater can visit to try to become a breeder compared to 1 group for helpers

// Modifiers in survival. X0 + Xsn - Xsh =< 1
    double X0; //base survival without the effect of help or group size
    double Xsh;    // cost of help in survival
    double Xsn;    // benefit of group size in survival

//Modifiers in fecundity
    double K0;    // min fecundity, fecundity when no help provided.
    double K1;    // benefit of cumHelp in the fecundity


//Genetic values

//For help
    double INIT_ALPHA;            // bigger values higher level of help
    double INIT_ALPHA_AGE;            //linear term for age, positive: higher help with age
    double INIT_ALPHA_AGE2;            //quadratic term for age, positive: higher help with age

    double MUTATION_ALPHA;            // mutation rate in alpha for level of help
    double MUTATION_ALPHA_AGE;
    double MUTATION_ALPHA_AGE2;
    double STEP_ALPHA;            // mutation step size in alpha for level of help


//For dispersal
    double INIT_BETA;            // bigger values higher dispersal
    double INIT_BETA_AGE;            // 0: age has no effect, positive: dispersal decreases with age

    double MUTATION_BETA;            // mutation rate for the propensity to disperse
    double MUTATION_BETA_AGE;
    double STEP_BETA;            // mutation step size for the propensity to disperse


//For relatedness
    double MUTATION_DRIFT;            // mutation rate in the neutral selected value to track level of relatedness
    double STEP_DRIFT;            // mutation step size in the neutral genetic value to track level of relatedness




public:

    Parameters();

    Parameters(std::string url);

    const std::string &getName() const;

    bool isReactionNormHelp() const;

    bool isNoRelatedness() const;

    bool isEvolutionHelpAfterDispersal() const;

    bool isReactionNormDispersal() const;

    bool isOldSurvivalFormula() const;

    int getMaxColonies() const;

    int getNumGenerations() const;

    int getMaxNumReplicates() const;

    int getSkip() const;

    int getInitNumHelpers() const;

    double getBiasFloatBreeder() const;

    double getX0() const;

    double getXsh() const;

    double getXsn() const;

    double getK0() const;

    double getK1() const;

    double getInitAlpha() const;

    double getInitAlphaAge() const;

    double getInitAlphaAge2() const;

    double getMutationAlpha() const;

    double getMutationAlphaAge() const;

    double getMutationAlphaAge2() const;

    double getStepAlpha() const;

    double getInitBeta() const;

    double getInitBetaAge() const;

    double getMutationBeta() const;

    double getMutationBetaAge() const;

    double getStepBeta() const;

    double getMutationDrift() const;

    double getStepDrift() const;

    void setMutationAlpha(double mutationAlpha);

    void setMutationAlphaAge(double mutationAlphaAge);

    void setMutationAlphaAge2(double mutationAlphaAge2);

};


#endif //CPP_PARAMETERS_H
