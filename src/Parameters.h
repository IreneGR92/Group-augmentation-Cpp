
#ifndef CPP_PARAMETERS_H
#define CPP_PARAMETERS_H

#include <iomanip>
#include <string>
#include <fstream>
#include <random>

//Singleton
class Parameters {

public:
    Parameters();

    virtual ~Parameters();

    explicit Parameters(const std::string& url);

private:


    std::string name;
///#Run parameters
    bool REACTION_NORM_HELP;    //Apply reaction norm to age for dispersal?
    bool REACTION_NORM_DISPERSAL; // Apply reaction norm to age for dispersal?
    bool EVOLUTION_HELP_AFTER_DISPERSAL; // help evolves only after the evolution of dispersal?
    bool DIRECT_BROOD_CARE_ONLY; // right after an individual becomes breeder, it does not receive help for the first breeding cycle
    bool LOW_SURVIVAL_BREEDER;
    bool NO_GROUP_AUGMENTATION;
    bool NO_RELATEDNESS;       //Apply implementation to remove the effect of relatedness?
    bool NO_RELATEDNESS_RANDOM_GROUP; // When applying no relatedness, helpers shuffled to random group instead of maintaining group size
    bool AGE_NO_INFLUENCE_RELATEDNESS; // Age no influence of who inherits territory, randomly sampled from candidate (helpers + sample floaters)

    int MAX_COLONIES;     // max number of groups or colonies --> breeding spots.
    int NUM_GENERATIONS;
    int MAX_NUM_REPLICATES;
    int SKIP;   // interval between print-outs

//Fix values
    int INIT_NUM_HELPERS;     //initial number of helpers per group
    double BIAS_FLOAT_BREEDER; //mean of number of groups a floater can visit to try to become a breeder compared to 1 group for helpers
    int FIXED_GROUP_SIZE;       //in the implementation of no group augmentation, virtual group size for survival for breeder and helpers
    int MIN_AGE_BECOME_BREEDER; //minimum age for individuals to be able to become breeders

// Modifiers in survival.
    double m;      //base mortality
    double n;      //mortality linked to dispersal
    double X0;     //base survival without the effect of help or group size
    double Xsh;    // cost of help in survival
    double Xsn;    // benefit of group size in survival

//Modifiers in fecundity
    double K0;    // min fecundity, fecundity when no help provided.
    double K1;    // benefit of cumHelp in the fecundity


//Genetic values

//For help
    double INIT_ALPHA;            // bigger values higher level of help
    double INIT_ALPHA_AGE;            //linear term for age, positive: higher help with age

    double MUTATION_ALPHA;            // mutation rate in alpha for level of help
    double MUTATION_ALPHA_AGE;
    double STEP_ALPHA;            // mutation step size in alpha for level of help


//For dispersal
    double INIT_BETA;            // bigger values higher dispersal
    double INIT_BETA_AGE;            // 0: age has no effect, positive: dispersal decreases with age

    double MUTATION_BETA;            // mutation rate for the propensity to disperse
    double MUTATION_BETA_AGE;
    double STEP_BETA;            // mutation step size for the propensity to disperse


//For relatedness
    double MUTATION_DRIFT;            // mutation rate in the neutral selected value to track level of relatedness
    double STEP_DRIFT; // mutation step size in the neutral genetic value to track level of relatedness

    std::default_random_engine *generator;

    std::string getName(std::string url);

    void print(std::ofstream &outputStream);

    std::ofstream *mainWriter;
    std::ofstream *lastGenerationWriter;

public:


    std::uniform_real_distribution<double> driftUniform;
    std::uniform_real_distribution<double> uniform;

    void print();


    const std::string &getName() const;

    bool isReactionNormHelp() const;

    bool isReactionNormDispersal() const;

    bool isEvolutionHelpAfterDispersal() const;

    bool isDirectBroodCareOnly() const;

    bool isLowSurvivalBreeder() const;

    bool isNoGroupAugmentation() const;

    bool isNoRelatedness() const;

    bool isNoRelatednessRandomGroup() const;

    bool isAgeNoInfluenceInheritance() const;

    int getMaxColonies() const;

    int getNumGenerations() const;

    int getMaxNumReplicates() const;

    int getSkip() const;

    int getInitNumHelpers() const;

    double getBiasFloatBreeder() const;

    int getFixedGroupSize() const;

    int getMinAgeBecomeBreeder() const;

    double getM() const;

    double getN() const;

    double getX0() const;

    double getXsh() const;

    double getXsn() const;

    double getK0() const;

    double getK1() const;

    double getInitAlpha() const;

    double getInitAlphaAge() const;

    double getMutationAlpha() const;

    double getMutationAlphaAge() const;

    double getStepAlpha() const;

    double getInitBeta() const;

    double getInitBetaAge() const;

    double getMutationBeta() const;

    double getMutationBetaAge() const;

    double getStepBeta() const;

    double getMutationDrift() const;

    double getStepDrift() const;

    static const int NO_VALUE = -1;

    std::ofstream *getMainWriter() const;

    std::ofstream *getLastGenerationWriter() const;

    std::default_random_engine *getGenerator() const;

    static Parameters *instance();

};


#endif //CPP_PARAMETERS_H
