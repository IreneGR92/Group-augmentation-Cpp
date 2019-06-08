
#ifndef GROUP_AUGMENTATION_PARAMETERS_H
#define GROUP_AUGMENTATION_PARAMETERS_H

std::string name = "test";

///Run parameters
const bool REACTION_NORM_HELP = false;  	//Apply reaction norm to age for level of help?
const bool REACTION_NORM_DISPERSAL = false;	//Apply reaction norm to age for dispersal?
const bool NO_RELATEDNESS = false;       //Apply implementation to remove the effect of relatedness?
const bool EVOLUTION_HELP_AFTER_DISPERSAL = false; // help evolves only after the evolution of dispersal?
const bool OLD_SURVIVAL_FORMULA = false;

const int MAX_COLONIES	  = 5000;     // max number of groups or colonies --> breeding spots.
const int NUM_GENERATIONS = 100000;
const int MAX_NUM_REPLICATES  = 20;
const int SKIP = 50;   // interval between print-outs

//Fix values
const int    INIT_NUM_HELPERS = 3;	 //initial number of helpers per group
const double BIAS_FLOAT_BREEDER = 2; //mean of number of groups a floater can visit to try to become a breeder compared to 1 group for helpers

// Modifiers in survival. X0 + Xsn - Xsh =< 1
const double X0	 = 0.7; //base survival without the effect of help or group size
const double Xsh = 0.4;	// cost of help in survival
const double Xsn = 0.4;	// benefit of group size in survival

//Modifiers in fecundity
const double K0  = 1;	// min fecundity, fecundity when no help provided.
const double K1  = 1;	// benefit of cumHelp in the fecundity


//Genetic values

//For help
const double INIT_ALPHA			= 0.0;			// bigger values higher level of help
const double INIT_ALPHA_AGE		= 0.0;			//linear term for age, positive: higher help with age
const double INIT_ALPHA_AGE2	= 0.0;			//quadratic term for age, positive: higher help with age

double MUTATION_ALPHA		    = 0.05;			// mutation rate in alpha for level of help
double MUTATION_ALPHA_AGE       = 0.05;
double MUTATION_ALPHA_AGE2      = 0.05;
const double STEP_ALPHA			= 0.04;			// mutation step size in alpha for level of help


//For dispersal
const double INIT_BETA			= 1.0;			// bigger values higher dispersal
const double INIT_BETA_AGE		= 0.0;			// 0: age has no effect, positive: dispersal decreases with age

const double MUTATION_BETA		= 0.05;			// mutation rate for the propensity to disperse
const double MUTATION_BETA_AGE	= 0.05;
const double STEP_BETA			= 0.04;			// mutation step size for the propensity to disperse


//For relatedness
const double MUTATION_DRIFT		= 0.05;			// mutation rate in the neutral selected value to track level of relatedness
const double STEP_DRIFT			= 0.04;			// mutation step size in the neutral genetic value to track level of relatedness

#endif //GROUP_AUGMENTATION_PARAMETERS_H
