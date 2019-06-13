/***********************************************
 GROUP AUGMENTATION MODEL
 - Passive group augmentation: individuals help in order to increase group size which in turn increases survival
 - Active group augmentation or delayed reciprocity: if inherit the breeding possition, individuals benefit of the help given by the recruits in the group.


 Stochastic individual based model.
 Limited breeding spots, unlimited population size; one breeder per group.
 Population overlap, turn over when breeder dies. 
 Older/more dominant individual higher probability of becoming new breeder.
 Evolution of level of help and dispersal. Inclusion of reaction norm to age.
 Dispersal produces temporal dispersers/floaters that can reproduce in another group or join as helpers. 
 Survival dependent on group size, level of help and predation/environment.
 Fecundity dependent on cummulative level of help within group.
 Relatedness as an emergent property.

***********************************************/

/*HEADER FILES*/

#include <cstdlib>
#include <stdio.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <random>
#include "parameters.h"
//#include <chrono> //if we use the system clock as seed

using namespace std;

// Output file
ofstream fout("group_augmentation_" + name + ".txt");
ofstream fout2("group_augmentation_last_generation_" + name + ".txt");


/*CONSTANTS AND STRUCTURES*/

// Random numbers
//mt19937 mt(time(0)); // random number generator
//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); // if we don't want to obtain the same simulation under the same parameters
unsigned seed = 0; //in the same run takes different random values, but different runs same values unless we change the seed
default_random_engine generator(seed);
uniform_real_distribution<double> DriftUniform(0, 100);
uniform_real_distribution<double> Uniform(0, 1);


enum classes { BREEDER, HELPER, FLOATER };

const int NO_VALUE = -1;

//Population parameters and Statistics
int replica, gen, population, driftGroupSize, maxGroupSize, populationHelpers, countExpressedHelp;
int populationBeforeSurv, deaths, floatersgenerated, newbreederFloater, newbreederHelper, inheritance; //counters
double relatedness;
double  meanGroupSize, stdevGroupSize,  sumGroupSize, sumsqGroupSize, varGroupSize,
meanAge, stdevAge, sumAge, sumsqAge, varAge,
meanAlpha, stdevAlpha, sumAlpha, sumsqAlpha, varAlpha,
meanAlphaAge, stdevAlphaAge, sumAlphaAge, sumsqAlphaAge, varAlphaAge,
meanAlphaAge2, stdevAlphaAge2, sumAlphaAge2, sumsqAlphaAge2, varAlphaAge2,
meanBeta, stdevBeta, sumBeta, sumsqBeta, varBeta,
meanBetaAge, stdevBetaAge, sumBetaAge, sumsqBetaAge, varBetaAge,
meanHelp, stdevHelp, sumHelp, sumsqHelp, varHelp,
meanCumHelp, stdevCumHelp, sumCumHelp, sumsqCumHelp, varCumHelp,
meanDispersal, stdevDispersal, sumDispersal, sumsqDispersal, varDispersal,
meanSurvival, stdevSurvival, sumSurvival, sumsqSurvival, varSurvival,
meanDriftB, sumDriftB, meanDriftH, sumDriftH,			//relatedness related
meanDriftBH, meanDriftBB, sumDriftBH, sumDriftBB,		//relatedness related
corr_HelpDispersal, sumprodHelpDispersal,
corr_HelpGroup, sumprodHelpGroup;


//Structures

struct Individual // define individual traits
{
	explicit Individual(double alpha_ = 0, double alphaAge_ = 0, double alphaAge2_ = 0, double beta_ = 0, double betaAge_ = 0, double drift_ = 0, classes fishType_ = HELPER);
	Individual(const Individual &mother);
	double alpha, alphaAge, alphaAge2, beta, betaAge, drift, 		// genetic values
		dispersal, help, survival;									// phenotypic values
	classes fishType;												// possible classes: breeder, helper, floater
	int age;
	bool inherit;													//did the new breeder inherit the territory or did it disperse?
	bool expressedHelp;

	//Functions inside Individual
	double calcDispersal();
	void calcHelp();
	double calcSurvival(int totalHelpers);
	void Mutate();
};


Individual::Individual(double alpha_, double alphaAge_, double alphaAge2_, double beta_, double betaAge_, double drift_, classes fishType_) {
	alpha = alpha_;
	alphaAge = alphaAge_;
	alphaAge2 = alphaAge2_;
	beta = beta_;
	betaAge = betaAge_;
	drift = drift_;
	Mutate();
	fishType = fishType_;
	age = 1;
	inherit = true;
	expressedHelp = false;
	survival = NO_VALUE;
	help = 0;
	dispersal = NO_VALUE;
}

Individual::Individual(const Individual &copy) {
	alpha = copy.alpha;
	alphaAge = copy.alphaAge;
	alphaAge2 = copy.alphaAge2;
	beta = copy.beta;
	betaAge = copy.betaAge;
	drift = copy.drift;
	fishType = copy.fishType;
	age = copy.age;
	inherit = copy.inherit;
	expressedHelp = copy.expressedHelp;
	survival = copy.survival;
	help = copy.help;
	dispersal = copy.dispersal;
}


struct Group // define group traits
{
	Group(double alpha_, double alphaAge_, double alphaAge2_, double beta_, double betaAge_, int numhelp_);
	double cumHelp;
	int totalHelpers;
	bool breederAlive;                                     // for the breeder: 1 alive, 0 dead
	int groupSize;
	double fecundity;
	int realFecundity;

	Individual breeder; // create breeder inside group
	vector<Individual> helpers; // create a vector of helpers inside each group

//Functions inside Group
	void Disperse(vector<Individual> &floaters);
	void CumHelp();
	void SurvivalGroup(int &deaths);
	void NewBreeder(vector<Individual> &floaters, int &newBreederFloater, int &newBreederHelper, int &inheritance);
	void IncreaseAge();
	double TotalPopulation();
	void Fecundity();
	void Reproduction();
};


Group::Group(double alpha_ = INIT_ALPHA, double alphaAge_ = INIT_ALPHA_AGE, double alphaAge2_ = INIT_ALPHA_AGE2, double beta_ = INIT_BETA, double betaAge_ = INIT_BETA_AGE, int numhelp_ = INIT_NUM_HELPERS)
{
	breeder = Individual(alpha_, alphaAge_, alphaAge2_, beta_, betaAge_, DriftUniform(generator), BREEDER);
	breederAlive = true;
	fecundity = NO_VALUE;
	realFecundity = NO_VALUE;

	for (int i = 0; i < numhelp_; ++i)
	{
		helpers.emplace_back(alpha_, alphaAge_, alphaAge2_, beta_, betaAge_, DriftUniform(generator), HELPER);
	}

	TotalPopulation();
}


/* INITIALISE POPULATION */
void InitGroup(vector<Group> &groups)
{
	int i;
	for (i = 0; i < MAX_COLONIES; i++)
	{
		groups[i] = Group(INIT_ALPHA, INIT_ALPHA_AGE, INIT_ALPHA_AGE2, INIT_BETA, INIT_BETA_AGE, INIT_NUM_HELPERS);
	}
}


/* BECOME FLOATER (STAY VS DISPERSE) */

double Individual::calcDispersal()
{
	if (!REACTION_NORM_DISPERSAL) {

		dispersal = beta; // Range from 0 to 1 to compare to a Uniform distribution
	}
	else {
		dispersal = 1 / (1 + exp(betaAge*age - beta));
	}

    if (dispersal < 0 || dispersal > 1) {
        cout << "error in dispersal: " << dispersal << endl;
    }

	return dispersal;
}


void Group::Disperse(vector<Individual> &floaters)
{
    vector<Individual, std::allocator<Individual>>::iterator dispersalIt;
    dispersalIt = helpers.begin();
	int sizevec = helpers.size();
	int counting = 0;

	while (!helpers.empty() && sizevec > counting)
	{
		dispersalIt->calcDispersal();

		if (Uniform(generator) < dispersalIt->dispersal)
		{
			dispersalIt->inherit = false; //the location of the individual is not the natal territory
            dispersalIt->expressedHelp = false;
			floaters.push_back(*dispersalIt); //add the individual to the vector floaters in the last position
			floaters[floaters.size() - 1].fishType = FLOATER;
			*dispersalIt = helpers[helpers.size() - 1]; // this and next line removes the individual from the helpers vector
			helpers.pop_back();
			++counting;
		}
		else
			++dispersalIt, ++counting;
	}
	//cout << "floater size " << floaters.size() << endl;
}


/*DISPLAY LEVEL OF HELP*/
void Individual::calcHelp() {

    if (fishType == HELPER) {
        if (!REACTION_NORM_HELP) {
            help = alpha;
        } else {
            help = alpha + alphaAge * age + alphaAge2 * age * age;
            if (help < 0) { help = 0; }
        }
        expressedHelp = true;
    }
}


void Group::CumHelp() //Calculate accumulative help of all individuals inside of each group.
{
	cumHelp = 0;
	//Level of help for helpers
    vector<Individual, std::allocator<Individual>>::iterator helpIt;
    for (helpIt = helpers.begin(); helpIt < helpers.end(); ++helpIt)
	{
		helpIt->calcHelp();
		cumHelp += helpIt->help;
	}
	//Level of help for breeders
	//breeder.calcHelp();
	//cumHelp += breeder.help;
}

/*SURVIVAL*/

double Individual::calcSurvival(int totalHelpers)
{
    if (OLD_SURVIVAL_FORMULA){
        survival = (1 - X0) / (1 + exp(Xsh*help - Xsn * (totalHelpers + 1))); // +1 to know group size (1 breeder + helpers)
    }else{
        survival = X0 + Xsn / (1 + exp(-(totalHelpers + 1))) - Xsh / (1 + exp(-help)); //alternative implementation of survival, if Xsn=Xsh, equivalent size effect of help and group size in survival

        if (survival > 0.95) {
            survival = 0.95;
            cout << "survival greater than 1" << endl;
        }
    }

	return survival;
}


void Group::SurvivalGroup(int &deaths)
{
	totalHelpers = helpers.size();

	//Calculate the survival of the helpers

    vector<Individual, std::allocator<Individual>>::iterator survHIt;
    survHIt = helpers.begin();
	int sizevec = helpers.size();
	int counting = 0;
	while (!helpers.empty() && sizevec > counting)
	{
		//Calculate value of survival
		survHIt->calcSurvival(totalHelpers);

		//Mortality
		if (Uniform(generator) > survHIt->survival)
		{
			*survHIt = helpers[helpers.size() - 1];
			helpers.pop_back();
			++counting;
			deaths++;
		}
		else
			++survHIt, ++counting; //go to next individual
	}

	//Calculate the survival of the breeder
	breeder.calcSurvival(totalHelpers);
	if (Uniform(generator) > breeder.survival)
	{
		breederAlive = false;
		breeder.age = NO_VALUE;
		deaths++;
	}
}

void SurvivalFloaters(vector<Individual> &floaters, int &deaths) //Calculate the survival of the floaters
{
    vector<Individual, std::allocator<Individual>>::iterator survFIt;
    survFIt = floaters.begin();
	int sizevec = floaters.size();
	int counting = 0;
	while (!floaters.empty() && sizevec > counting)
	{
		//Calculate value of survival
		survFIt->calcSurvival(0);

		//Mortality
		if (Uniform(generator) > survFIt->survival)
		{
			*survFIt = floaters[floaters.size() - 1];
			floaters.pop_back();
			++counting;
			deaths++;
		}
		else
			++survFIt, ++counting; //go to next individual
	}
}

/* BECOME BREEDER */

void Group::NewBreeder(vector<Individual> &floaters, int &newBreederFloater, int &newBreederHelper, int &inheritance)
{
	//    Select a random sample from the floaters
	int i = 0;
	int sumage = 0;
	double currentposition = 0; //age of the previous ind taken from Candidates
	int UniformFloatNum = 0;
	double RandP = Uniform(generator);
    int proportFloaters;
    proportFloaters = round(floaters.size() * BIAS_FLOAT_BREEDER / MAX_COLONIES);

	vector<Individual*> Candidates;
	vector<double>position; //vector of age to choose with higher likelihood the ind with higher age
	vector<int>TemporaryCandidates; // to prevent taking the same ind several times in the sample

	if (!floaters.empty() && floaters.size() > proportFloaters) {
		while (i < proportFloaters)
		{
			uniform_int_distribution<int> UniformFloat(0, floaters.size() - 1); //random floater ID taken in the sample
			UniformFloatNum = UniformFloat(generator);
			TemporaryCandidates.push_back(UniformFloatNum); //add references of the floaters sampled to a vector
			sort(TemporaryCandidates.begin(), TemporaryCandidates.end()); //sort vector
			i++;
		}

		int temp = 0;
        vector<int, std::allocator<int>>::iterator itTempCandidates;
        for (itTempCandidates = TemporaryCandidates.begin(); itTempCandidates < TemporaryCandidates.end(); ++itTempCandidates)
		{
			if (*itTempCandidates != temp) //to make sure the same ind is not taken more than ones
			{
				Candidates.push_back(&floaters[UniformFloatNum]);
				temp = *itTempCandidates;
			}
		}
	}

	else if (!floaters.empty() && floaters.size() < proportFloaters) { //TODO:When less floaters available than the sample size, takes all of them. Change to a proportion?
        vector<Individual, std::allocator<Individual>>::iterator floatIt;
        for (floatIt = floaters.begin(); floatIt < floaters.end(); ++floatIt)
		{
			Candidates.push_back(&(*floatIt));
		}
	}


	//    Join the helpers in the group to the sample of floaters

    vector<Individual, std::allocator<Individual>>::iterator helpIt;
    for (helpIt = helpers.begin(); helpIt < helpers.end(); ++helpIt)
	{
		Candidates.push_back(&(*helpIt));
	}


//     Choose breeder with higher likelihood for the highest age
    vector<Individual *, std::allocator<Individual *>>::iterator ageIt;
    for (ageIt = Candidates.begin(); ageIt < Candidates.end(); ++ageIt) //ageIt creates a vector of pointers to an individual
	{
		sumage += (*ageIt)->age; //add all the age from the vector Candidates
	}

    vector<Individual *, std::allocator<Individual *>>::iterator age2It;
    for (age2It = Candidates.begin(); age2It < Candidates.end(); ++age2It)
	{
		position.push_back(static_cast<double>((*age2It)->age) / static_cast<double>(sumage) + currentposition); //creates a vector with proportional segments to the age of each individual
		currentposition = position[position.size() - 1];
	}

	if (floaters.empty() && Candidates.size() != helpers.size()){ cout << "Error assigning empty floaters to Breeder" << endl; }

    vector<Individual *, std::allocator<Individual *>>::iterator age3It;
    age3It = Candidates.begin();
	int counting = 0;
	while (counting < Candidates.size()) {
		if (RandP < position[age3It - Candidates.begin()]) //to access the same ind in the candidates vector
		{
			breeder = **age3It; //substitute the previous dead breeder
			breederAlive = true;

			if ((*age3It)->fishType == FLOATER) //delete the ind from the vector floaters
			{
				**age3It = floaters[floaters.size() - 1];
				floaters.pop_back();
				newBreederFloater++;
				if ((*age3It)->inherit == 1) {
					cout << "error in inheritance" << endl;
				}
			}
			else
			{
				**age3It = helpers[helpers.size() - 1]; //delete the ind from the vector helpers
				helpers.pop_back();
				newBreederHelper++;
				if ((*age3It)->inherit == 1) {
					inheritance++;					//calculates how many individuals that become breeders are natal to the territory
				}
			}

			breeder.fishType = BREEDER; //modify the class
			counting = Candidates.size();//end loop
		}
		else
			++age3It, ++counting;
	}
}


/*REASSIGN FLOATERS*/

void Reassign(vector<Individual> &floaters, vector<Group> &groups)
{
    if (!NO_RELATEDNESS) {
        uniform_int_distribution<int> UniformMaxCol(0, MAX_COLONIES - 1);
        int selectGroup;
        vector<Individual>::iterator indIt;
        while (!floaters.empty()) {
            indIt = floaters.end() - 1;
            indIt->help = 0;
            selectGroup = UniformMaxCol(generator);
            indIt->fishType = HELPER; //modify the class
            groups[selectGroup].helpers.push_back(
                    *indIt); //add the floater to the helper vector in a randomly selected group
            floaters.pop_back(); //remove the floater from its vector
        }
    }

    else{

        double sumcumHelp = 0;
        double currentposition = 0;
        double RandP = Uniform(generator);
        int allNoHelp = 0;

        vector<double> position; //vector of cumHelp to choose with higher likelihood the ind with higher age

        vector<Group, std::allocator<Group>>::iterator groupIt;
        for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
            sumcumHelp += 1+groupIt->cumHelp; //add all the cumHelp from the vector Groups
            if (groupIt->cumHelp != MAX_COLONIES) { allNoHelp++; } //to check if all groups display cumhelp 0

            //if (sumcumHelp != 0) { cout << "sumcumHelp =" << sumcumHelp << '\t' << "allNoHelp =" << allNoHelp << endl; } //track
        }

        if (allNoHelp != 0) {

            for (groupIt = groups.begin(); groupIt < groups.end(); ++groupIt) {
                position.push_back(static_cast<double>(1+(groupIt)->cumHelp) / static_cast<double>(sumcumHelp) +
                                   currentposition); //creates a vector with proportional segments to the cumHelp of each group
                currentposition = position[position.size() - 1];

                //cout << "position size =" << position.size() << '\t' <<"position = " << currentposition << endl;
            }

            vector<Individual, std::allocator<Individual>>::iterator floatIt;
            for (floatIt = floaters.begin(); floatIt < floaters.end(); ++floatIt) {
                floatIt->fishType = HELPER; //all floaters pass to be helpers
            }

            while (!floaters.empty()) {
                for (int i = 0; i < groups.size(); i++) {
                    if (!floaters.empty()) {
                        if (RandP < position[i]) //to access the same ind in the candidates vector
                        {
                            groups[i].helpers.push_back(floaters[floaters.size() -
                                                                 1]); //add the floater to the helper vector in a randomly selected group
                            floaters.pop_back();
                            //cout << "floater size =" << floaters.size() << endl;
                        }
                    }
                    //cout << "after go to all groups =" << floaters.size() << endl;
                }
            }
        } else {
            uniform_int_distribution<int> UniformMaxCol(0, MAX_COLONIES - 1);
            int selectGroup;
            vector<Individual>::iterator floatIt;
            while (!floaters.empty()) {
                floatIt = floaters.end() - 1;
                selectGroup = UniformMaxCol(generator);
                floatIt->fishType = HELPER; //modify the class
                groups[selectGroup].helpers.push_back(*floatIt); //add the floater to the helper vector in a randomly selected group
                floaters.pop_back(); //remove the floater from its vector
            }
        }
    }
}


/* INCREASE AGE*/
void Group::IncreaseAge()
{
    vector<Individual, std::allocator<Individual>>::iterator ageIt;
    for (ageIt = helpers.begin(); ageIt < helpers.end(); ++ageIt)
	{
		ageIt->age++;
	}

	if (breederAlive) {
		breeder.age++;
	}
	else {
		breeder.age = NO_VALUE; //to check for dead breeders still existing in print last generation
	}
}


/* TOTAL NUMBER OF INDIVIDUALS PER GROUP*/

double Group::TotalPopulation()
{
	if (breederAlive) {
		groupSize = helpers.size() + 1;
	}
	else {
		groupSize = helpers.size();
	}

	return groupSize;
}


/* REPRODUCTION */

void Group::Fecundity()
{
	if (!NO_RELATEDNESS){
        fecundity = K0 + K1 * cumHelp / (1 + cumHelp * K1); //fecundity function of cummulative help in the group
	}else{
        fecundity = K0;
    }


	poisson_distribution<int> PoissonFec(fecundity);
	realFecundity = PoissonFec(generator); //integer number
}

void Group::Reproduction() // populate offspring generation
{
	if (breederAlive)
	{
		for (int i = 0; i < realFecundity; i++) //number of offspring dependent on real fecundity
		{
			helpers.emplace_back(breeder.alpha, breeder.alphaAge, breeder.alphaAge2, breeder.beta, breeder.betaAge, breeder.drift); //create a new individual as helper in the group. Call construct to assign the mother genetic values to the offspring, construct calls Mutate function.
		}
	}
}


void Individual::Mutate() // mutate genome of offspring 
{
	normal_distribution <double> NormalA(0, STEP_ALPHA); //TODO: could be simplified if I decide to have all the steps size with the same magnitude
	normal_distribution <double> NormalB(0, STEP_BETA);
	normal_distribution <double> NormalD(0, STEP_DRIFT);

    if (EVOLUTION_HELP_AFTER_DISPERSAL){
        if (gen < NUM_GENERATIONS / 4) {
            MUTATION_ALPHA = 0;
            MUTATION_ALPHA_AGE = 0;
            MUTATION_ALPHA_AGE2 = 0;
        }
        else {
            MUTATION_ALPHA = 0.05;
            MUTATION_ALPHA_AGE = 0.05;
            MUTATION_ALPHA_AGE2 = 0.05;
        }
    }

	if (Uniform(generator) < MUTATION_ALPHA) {
		alpha += NormalA(generator);
		if (!REACTION_NORM_HELP) {
			if (alpha < 0) { alpha = 0; }
		}
	}
	if (REACTION_NORM_HELP) {
		if (Uniform(generator) < MUTATION_ALPHA_AGE) {
			alphaAge += NormalA(generator);
		}

		if (Uniform(generator) < MUTATION_ALPHA_AGE2) {
			alphaAge2 += NormalA(generator);
		}
	}

	if (Uniform(generator) < MUTATION_BETA) {
		beta += NormalB(generator);
		if (!REACTION_NORM_DISPERSAL) {
			if (beta < 0) { beta = 0; }
			if (beta > 1) { beta = 1; }
		}
	}
	if (REACTION_NORM_DISPERSAL) {
		if (Uniform(generator) < MUTATION_BETA_AGE) {
			betaAge += NormalD(generator);
		}
	}

	if (Uniform(generator) < MUTATION_DRIFT) {
		drift += NormalD(generator);
	}
}


/* CALCULATE STATISTICS */
void Statistics(vector<Group>groups) {

	relatedness = 0.0, driftGroupSize = 0, populationHelpers = 0, countExpressedHelp = 0,
		meanGroupSize = 0.0, stdevGroupSize = 0.0, maxGroupSize = 0, sumGroupSize = 0.0, sumsqGroupSize = 0.0, varGroupSize = 0.0,
		meanAge = 0.0, stdevAge = 0.0, sumAge = 0.0, sumsqAge = 0.0, varAge = 0.0,
		meanAlpha = 0.0, stdevAlpha = 0.0, sumAlpha = 0.0, sumsqAlpha = 0.0, varAlpha = 0.0,
		meanAlphaAge = 0.0, stdevAlphaAge = 0.0, sumAlphaAge = 0.0, sumsqAlphaAge = 0.0, varAlphaAge = 0.0,
		meanAlphaAge2 = 0.0, stdevAlphaAge2 = 0.0, sumAlphaAge2 = 0.0, sumsqAlphaAge2 = 0.0, varAlphaAge2 = 0.0,
		meanBeta = 0.0, stdevBeta = 0.0, sumBeta = 0.0, sumsqBeta = 0.0, varBeta = 0.0,
		meanBetaAge = 0.0, stdevBetaAge = 0.0, sumBetaAge = 0.0, sumsqBetaAge = 0.0, varBetaAge = 0.0,
		meanHelp = 0.0, stdevHelp = 0.0, sumHelp = 0.0, sumsqHelp = 0.0, varHelp = 0.0,
		meanCumHelp = 0.0, stdevCumHelp = 0.0, sumCumHelp = 0.0, sumsqCumHelp = 0.0, varCumHelp = 0.0,
		meanDispersal = 0.0, stdevDispersal = 0.0, sumDispersal = 0.0, sumsqDispersal = 0.0, varDispersal = 0.0,
		meanSurvival = 0.0, stdevSurvival = 0.0, sumSurvival = 0.0, sumsqSurvival = 0.0, varSurvival = 0.0,
		meanDriftB = 0.0, sumDriftB = 0.0, meanDriftH = 0.0, sumDriftH = 0.0,
		meanDriftBH = 0.0, meanDriftBB = 0.0, sumDriftBH = 0.0, sumDriftBB = 0.0, 
		corr_HelpDispersal = 0.0, sumprodHelpDispersal = 0.0,
		corr_HelpGroup = 0.0, sumprodHelpGroup = 0.0;

    vector<Group, std::allocator<Group>>::iterator groupStatsIt;
    for (groupStatsIt = groups.begin(); groupStatsIt < groups.end(); ++groupStatsIt) {

        vector<Individual, std::allocator<Individual>>::iterator indStatsIt;
        for (indStatsIt = groupStatsIt->helpers.begin(); indStatsIt < groupStatsIt->helpers.end(); ++indStatsIt) {

			sumAge += indStatsIt->age;
			sumsqAge += indStatsIt->age*indStatsIt->age;
			
			sumAlpha += indStatsIt->alpha;
			sumsqAlpha += indStatsIt->alpha*indStatsIt->alpha;

			sumAlphaAge += indStatsIt->alphaAge;
			sumsqAlphaAge += indStatsIt->alphaAge*indStatsIt->alphaAge;

			sumAlphaAge2 += indStatsIt->alphaAge2;
			sumsqAlphaAge2 += indStatsIt->alphaAge2*indStatsIt->alphaAge2;

			sumBeta += indStatsIt->beta;
			sumsqBeta += indStatsIt->beta*indStatsIt->beta;

			sumBetaAge += indStatsIt->betaAge;
			sumsqBetaAge += indStatsIt->betaAge*indStatsIt->betaAge;

			if (indStatsIt->expressedHelp) {
				sumHelp += indStatsIt->help;
				sumsqHelp += indStatsIt->help*indStatsIt->help;
				countExpressedHelp++;
			}

			sumsqHelp += indStatsIt->help*indStatsIt->help;

			sumDispersal += indStatsIt->dispersal;
			sumsqDispersal += indStatsIt->dispersal*indStatsIt->dispersal;

			sumSurvival += indStatsIt->survival;
			sumsqSurvival += indStatsIt->survival*indStatsIt->survival;

			if (indStatsIt->expressedHelp) {
				sumprodHelpDispersal += indStatsIt->help*indStatsIt->dispersal;
			}

			if (groupStatsIt->breederAlive) {
				sumDriftB += groupStatsIt->breeder.drift;
				sumDriftH += indStatsIt->drift;
                sumDriftBH += indStatsIt->drift * groupStatsIt->breeder.drift;
				sumDriftBB += groupStatsIt->breeder.drift * groupStatsIt->breeder.drift;
				++driftGroupSize;
			}

		}

		populationHelpers += groupStatsIt->helpers.size();

		sumGroupSize += groupStatsIt->groupSize;
		sumsqGroupSize += groupStatsIt->groupSize*groupStatsIt->groupSize;
		if (groupStatsIt->groupSize > maxGroupSize) maxGroupSize = groupStatsIt->groupSize;

		if (groupStatsIt->breederAlive) sumAge += groupStatsIt->breeder.age;
		if (groupStatsIt->breederAlive) sumsqAge += groupStatsIt->breeder.age*groupStatsIt->breeder.age;

		if (groupStatsIt->breederAlive) sumAlpha += groupStatsIt->breeder.alpha;
		if (groupStatsIt->breederAlive) sumsqAlpha += groupStatsIt->breeder.alpha*groupStatsIt->breeder.alpha;

		if (groupStatsIt->breederAlive) sumAlphaAge += groupStatsIt->breeder.alphaAge;
		if (groupStatsIt->breederAlive) sumsqAlphaAge += groupStatsIt->breeder.alphaAge*groupStatsIt->breeder.alphaAge;
		
		if (groupStatsIt->breederAlive) sumAlphaAge2 += groupStatsIt->breeder.alphaAge2;
		if (groupStatsIt->breederAlive) sumsqAlphaAge2 += groupStatsIt->breeder.alphaAge2*groupStatsIt->breeder.alphaAge2;

		if (groupStatsIt->breederAlive) sumBeta += groupStatsIt->breeder.beta;
		if (groupStatsIt->breederAlive) sumsqBeta += groupStatsIt->breeder.beta*groupStatsIt->breeder.beta;

		if (groupStatsIt->breederAlive) sumBetaAge += groupStatsIt->breeder.betaAge;
		if (groupStatsIt->breederAlive) sumsqBetaAge += groupStatsIt->breeder.betaAge*groupStatsIt->breeder.betaAge;

		if (groupStatsIt->breederAlive) sumSurvival += groupStatsIt->breeder.survival;
		if (groupStatsIt->breederAlive) sumsqSurvival += groupStatsIt->breeder.survival*groupStatsIt->breeder.survival;

		sumCumHelp += groupStatsIt->cumHelp;
		sumsqCumHelp += groupStatsIt->cumHelp*groupStatsIt->cumHelp;

		sumprodHelpGroup += groupStatsIt->cumHelp*groupStatsIt->groupSize;
	}

	meanGroupSize = sumGroupSize / MAX_COLONIES;
	meanAge = sumAge / population;
	meanAlpha = sumAlpha / population; //TODO: population=sumGroupSize so simplify!
	meanAlphaAge = sumAlphaAge / population;
	meanAlphaAge2 = sumAlphaAge2 / population;
	meanBeta = sumBeta / population;
	meanBetaAge = sumBetaAge / population;
	meanDriftB = sumDriftB / driftGroupSize;
	meanDriftH = sumDriftH / driftGroupSize;
	meanDriftBH = sumDriftBH / driftGroupSize;
	meanDriftBB = sumDriftBB / driftGroupSize;
	meanHelp = sumHelp / countExpressedHelp;
	meanCumHelp = sumCumHelp / MAX_COLONIES;
	meanDispersal = sumDispersal / populationHelpers;
	meanSurvival = sumSurvival / population;

	relatedness = (meanDriftBH - meanDriftB * meanDriftH) / (meanDriftBB - meanDriftB * meanDriftB); //covariate of a neutral selected gene
	if ((meanDriftBB - meanDriftB * meanDriftB) == 0) { relatedness = 2; } //prevent to divide by 0

	varGroupSize = sumsqGroupSize / MAX_COLONIES - meanGroupSize * meanGroupSize;
	varAge = sumsqAge / population - meanAge * meanAge;
	varAlpha = sumsqAlpha / population - meanAlpha * meanAlpha;
	varAlphaAge = sumsqAlphaAge / population - meanAlphaAge * meanAlphaAge;
	varAlphaAge2 = sumsqAlphaAge2 / population - meanAlphaAge2 * meanAlphaAge2;
	varBeta = sumsqBeta / population - meanBeta * meanBeta;
	varBetaAge = sumsqBetaAge / population - meanBetaAge * meanBetaAge;
	varHelp = sumsqHelp / countExpressedHelp - meanHelp * meanHelp;
	varCumHelp = sumsqCumHelp / MAX_COLONIES - meanCumHelp * meanCumHelp;
	varDispersal = sumsqDispersal / populationHelpers - meanDispersal * meanDispersal;
	varSurvival = sumsqSurvival / population - meanSurvival * meanSurvival;

	// to know if there is a problem (variance cannot be negative)
	varGroupSize > 0 ? stdevGroupSize = sqrt(varGroupSize) : stdevGroupSize = 0;
	varAge > 0 ? stdevAge = sqrt(varAge) : stdevAge = 0;
	varAlpha > 0 ? stdevAlpha = sqrt(varAlpha) : stdevAlpha = 0;
	varAlphaAge > 0 ? stdevAlphaAge = sqrt(varAlphaAge) : stdevAlphaAge = 0;
	varAlphaAge2 > 0 ? stdevAlphaAge2 = sqrt(varAlphaAge2) : stdevAlphaAge2 = 0;
	varBeta > 0 ? stdevBeta = sqrt(varBeta) : stdevBeta = 0;
	varBetaAge > 0 ? stdevBetaAge = sqrt(varBetaAge) : stdevBetaAge = 0;
	varHelp > 0 ? stdevHelp = sqrt(varHelp) : stdevHelp = 0;
	varCumHelp > 0 ? stdevCumHelp = sqrt(varCumHelp) : stdevCumHelp = 0;
	varDispersal > 0 ? stdevDispersal = sqrt(varDispersal) : stdevDispersal = 0;
	varSurvival > 0 ? stdevSurvival = sqrt(varBeta) : stdevSurvival = 0;

	//Correlations
	(stdevHelp > 0 && stdevDispersal > 0) ? corr_HelpDispersal = (sumprodHelpDispersal / countExpressedHelp - meanHelp * meanDispersal) / (stdevHelp*stdevDispersal) : corr_HelpDispersal = 0;
	(stdevCumHelp > 0 && stdevGroupSize > 0) ? corr_HelpGroup = (sumprodHelpGroup / MAX_COLONIES - meanCumHelp * meanGroupSize) / (stdevCumHelp*stdevGroupSize) : corr_HelpGroup = 0;

}



/* WRITE PARAMETER SETTINGS TO OUTPUT FILE */
void Printparams()
{
	fout << "PARAMETER VALUES" << endl

        << "Reaction_norm_help?: " << "\t" << REACTION_NORM_HELP << endl
        << "Reaction_norm_dispersal?: " << "\t" << REACTION_NORM_DISPERSAL << endl
        << "No_effect_relatedness?: " << "\t" << NO_RELATEDNESS << endl
        << "Evolution_help_after_dispersal?: " << "\t" << EVOLUTION_HELP_AFTER_DISPERSAL << endl
        << "Old_formula_survival?: " << "\t" <<OLD_SURVIVAL_FORMULA << endl
        << "Initial_population: " << "\t" << MAX_COLONIES * (INIT_NUM_HELPERS + 1) << endl
        << "Number_of_colonies: " << "\t" << MAX_COLONIES << endl
        << "Number_generations: " << "\t" << NUM_GENERATIONS << endl
        << "Number_replicates: " << "\t" << MAX_NUM_REPLICATES << endl
        << "Bias_float_breeder: " << "\t" << BIAS_FLOAT_BREEDER << endl
        << "X0(Base_survival): " << "\t" << X0 << endl
        << "Xh(Cost_help_survival): " << "\t" << Xsh << endl
        << "Xn(Benefit_group_size_survival(Xn): " << "\t" << Xsn << endl
        << "K0(Base_fecundity): " << "\t" << K0 << endl
        << "K1(Benefit_help_fecundity): " << "\t" << K1 << endl
        << "initAlpha: " << "\t" << INIT_ALPHA << endl
        << "initAlphaAge: " << "\t" << INIT_ALPHA_AGE << endl
        << "initAlphaAge2: " << "\t" << INIT_ALPHA_AGE2 << endl
        << "initBeta: " << "\t" << INIT_BETA << endl
        << "initBetaAge: " << "\t" << INIT_BETA_AGE << endl
        << "mutAlpha: " << "\t" << MUTATION_ALPHA << endl
        << "mutAlphaAge: " << "\t" << MUTATION_ALPHA_AGE << endl
        << "mutAlphaAge2: " << "\t" << MUTATION_ALPHA_AGE2 << endl
        << "mutBeta: " << "\t" << MUTATION_BETA << endl
        << "mutBetaAge: " << "\t" << MUTATION_BETA_AGE << endl
        << "mutDrift: " << "\t" << MUTATION_DRIFT << endl
        << "stepAlpha: " << "\t" << STEP_ALPHA << endl
        << "stepBeta: " << "\t" << STEP_BETA << endl
        << "stepDrift: " << "\t" << STEP_DRIFT << endl << endl;


	fout2 << "PARAMETER VALUES" << endl

	    << "Reaction_norm_help?: " << "\t" << REACTION_NORM_HELP << endl
        << "Reaction_norm_dispersal?: " << "\t" << REACTION_NORM_DISPERSAL << endl
        << "No_effect_relatedness?: " << "\t" << NO_RELATEDNESS << endl
        << "Evolution_help_after_dispersal?: " << "\t" << EVOLUTION_HELP_AFTER_DISPERSAL << endl
        << "Old_formula_survival?: " << "\t" <<OLD_SURVIVAL_FORMULA << endl
        << "Initial_population: " << "\t" << MAX_COLONIES * (INIT_NUM_HELPERS + 1) << endl
        << "Number_of_colonies: " << "\t" << MAX_COLONIES << endl
        << "Number_generations: " << "\t" << NUM_GENERATIONS << endl
        << "Number_replicates: " << "\t" << MAX_NUM_REPLICATES << endl
        << "Bias_float_breeder: " << "\t" << BIAS_FLOAT_BREEDER << endl
        << "X0(Base_survival): " << "\t" << X0 << endl
        << "Xh(Cost_help_survival): " << "\t" << Xsh << endl
        << "Xn(Benefit_group_size_survival(Xn): " << "\t" << Xsn << endl
        << "K0(Base_fecundity): " << "\t" << K0 << endl
        << "K1(Benefit_help_fecundity): " << "\t" << K1 << endl
        << "initAlpha: " << "\t" << INIT_ALPHA << endl
        << "initAlphaAge: " << "\t" << INIT_ALPHA_AGE << endl
        << "initAlphaAge2: " << "\t" << INIT_ALPHA_AGE2 << endl
        << "initBeta: " << "\t" << INIT_BETA << endl
        << "initBetaAge: " << "\t" << INIT_BETA_AGE << endl
        << "mutAlpha: " << "\t" << MUTATION_ALPHA << endl
        << "mutAlphaAge: " << "\t" << MUTATION_ALPHA_AGE << endl
        << "mutAlphaAge2: " << "\t" << MUTATION_ALPHA_AGE2 << endl
        << "mutBeta: " << "\t" << MUTATION_BETA << endl
        << "mutBetaAge: " << "\t" << MUTATION_BETA_AGE << endl
        << "mutDrift: " << "\t" << MUTATION_DRIFT << endl
        << "stepAlpha: " << "\t" << STEP_ALPHA << endl
        << "stepBeta: " << "\t" << STEP_BETA << endl
        << "stepDrift: " << "\t" << STEP_DRIFT << endl << endl;

}



/* WRITE GENETIC TRAIT VALUES TO OUTPUT FILE */
void WriteMeans()
{

	// show values on screen
	cout << fixed << showpoint
		<< setw(6) << gen
		<< setw(9) << population
		<< setw(9) << deaths
		<< setw(9) << floatersgenerated
		<< setw(9) << setprecision(2) << meanGroupSize
		<< setw(9) << maxGroupSize
		<< setw(9) << setprecision(2) << meanAge
		<< setw(9) << setprecision(4) << meanAlpha
		<< setw(9) << setprecision(4) << meanAlphaAge
		<< setw(9) << setprecision(4) << meanAlphaAge2
		<< setw(9) << setprecision(4) << meanBeta
		<< setw(9) << setprecision(4) << meanBetaAge
		<< setw(9) << setprecision(4) << meanHelp
		<< setw(9) << setprecision(4) << meanDispersal
		<< setw(9) << setprecision(2) << meanSurvival
		<< setw(9) << setprecision(2) << relatedness
		<< endl;


	// write values to output file
	fout << fixed << showpoint 
		<< gen
		<< "\t" << population
		<< "\t" << deaths
		<< "\t" << floatersgenerated
		<< "\t" << setprecision(4) << meanGroupSize
		<< "\t" << setprecision(4) << meanAge
		<< "\t" << setprecision(4) << meanAlpha
		<< "\t" << setprecision(4) << meanAlphaAge
		<< "\t" << setprecision(4) << meanAlphaAge2
		<< "\t" << setprecision(4) << meanBeta
		<< "\t" << setprecision(4) << meanBetaAge
		<< "\t" << setprecision(4) << meanHelp
		<< "\t" << setprecision(4) << meanCumHelp
		<< "\t" << setprecision(4) << meanDispersal
		<< "\t" << setprecision(4) << meanSurvival
		<< "\t" << setprecision(4) << relatedness
		<< "\t" << setprecision(4) << stdevGroupSize
		<< "\t" << setprecision(4) << stdevAge
		<< "\t" << setprecision(4) << stdevAlpha
		<< "\t" << setprecision(4) << stdevAlphaAge
		<< "\t" << setprecision(4) << stdevAlphaAge2
		<< "\t" << setprecision(4) << stdevBeta
		<< "\t" << setprecision(4) << stdevBetaAge
		<< "\t" << setprecision(4) << stdevHelp
		<< "\t" << setprecision(4) << stdevCumHelp
		<< "\t" << setprecision(4) << stdevDispersal
		<< "\t" << setprecision(4) << stdevSurvival
		<< "\t" << setprecision(4) << corr_HelpDispersal
		<< "\t" << setprecision(4) << corr_HelpGroup
		<< "\t" << newbreederFloater
		<< "\t" << newbreederHelper
		<< "\t" << inheritance
		<< endl;
}


/* MAIN PROGRAM */
int main() {
	
	Printparams();
	
	// column headings in output file 1
	fout << "Generation" << "\t" << "Population" << "\t" << "Deaths" << "\t" << "Floaters" << "\t" 
		<< "Group_size" << "\t" << "Age" << "\t" << "meanAlpha" << "\t" << "meanAlphaAge" << "\t" << "meanAlphaAge2" << "\t" << "meanBeta" << "\t" << "meanBetaAge" << "\t" 
		<< "meanHelp" << "\t" << "meanCumHelp" << "\t" << "meanDispersal" << "\t" << "meanSurvival" << "\t" << "Relatedness" << "\t"
		<< "SD_GroupSize" << "\t" << "SD_Age" << "\t" << "SD_Alpha" << "\t" << "SD_AlphaAge" << "\t" << "SD_AlphaAge2" << "\t"<< "SD_Beta" << "\t" << "SD_BetaAge" << "\t" 
		<< "SD_Help" << "\t" << "SD_CumHelp" << "\t" << "SD_Dispersal" << "\t" << "SD_Survival" << "\t" << "corr_Help_Disp" << "\t" << "corr_Help_Group" << "\t"
		<< "newBreederFloater" << "\t" << "newBreederHelper" << "\t" << "inheritance" << endl;

	// column headings in output file 2
	fout2 << "replica" << "\t" << "groupID" << "\t" << "type" << "\t" << "age" << "\t" 
		<< "alpha" << "\t" << "alphaAge" << "\t" << "alphaAge2"<< "\t" 
		<< "beta" << "\t" << "betaAge" << "\t" << "drift" 
		<< "\t" << "help" << "\t" << "dispersal" << "\t" << "survival" << endl;



	for (replica = 0; replica < MAX_NUM_REPLICATES; replica++) {

		gen = 0;
		deaths = 0; // to keep track of how many individuals die each generation
		population = 0; //total of ind in the whole simulation for the expecific generation
		populationBeforeSurv = 0;
		floatersgenerated = 0;
		newbreederFloater = 0, newbreederHelper = 0, inheritance = 0; //to know if the new breeder was a helper or a floater

		// column headings on screen
		cout << setw(6) << "gen" << setw(9) << "pop" << setw(9) << "deaths" << setw(9)
			<< "float" << setw(9) << "group" << setw(9) << "maxGroup" << setw(9) << "age" << setw(9) 
			<< "alpha" << setw(9) << "alphaAge" << setw(9) << "alphaAge2" << setw(9)
			<< "beta" << setw(9) << "betaAge" << setw(9) 
			<< "help" << setw(9) << "disper" << setw(9) << "surv" << setw(9) << "relat" << endl;


		vector<Individual> floaters;
		vector<Group> groups(MAX_COLONIES);

		InitGroup(groups);

        vector<Group, std::allocator<Group>>::iterator itPopulation;
        for (itPopulation = groups.begin(); itPopulation < groups.end(); ++itPopulation) {
			population += itPopulation->TotalPopulation(); //calculate number of ind in the whole population
		}

		Statistics(groups);
		WriteMeans();

		for (gen = 1; gen <= NUM_GENERATIONS; gen++)
		{
			//cout << "\t" << "\t" << "\t" << "\t" << "\t" << "GENERATION "<<gen<< " STARTS NOW!!!" <<endl;

			deaths = 0; // to keep track of how many individuals die each generation
			population = 0; //total of ind in the whole simulation for the expecific generation
			populationBeforeSurv = 0;
			floatersgenerated = 0;
			newbreederFloater = 0, newbreederHelper = 0, inheritance = 0;

			//cout << "Floaters before dispersal: " << floaters.size() << endl;
            vector<Group, std::allocator<Group>>::iterator itDispersal;
            for (itDispersal = groups.begin(); itDispersal < groups.end(); ++itDispersal)
			{
				populationBeforeSurv += itDispersal->TotalPopulation();
                itDispersal->Disperse(floaters);
				floatersgenerated = floaters.size();
				//    cout << "Floaters after dispersal: " << floaters.size() << endl;
			}

            vector<Group, std::allocator<Group>>::iterator itHelpSurvival;
            for (itHelpSurvival = groups.begin(); itHelpSurvival < groups.end(); ++itHelpSurvival)
			{
				itHelpSurvival->CumHelp();
				itHelpSurvival->SurvivalGroup(deaths);
			}

			SurvivalFloaters(floaters, deaths);
			//           cout << "total deaths: " << deaths << endl;

            vector<Group, std::allocator<Group>>::iterator itBreeder;
            for (itBreeder = groups.begin(); itBreeder < groups.end(); ++itBreeder)
			{
				if (!itBreeder->breederAlive)
				{
					//cout << groups.begin() - breederIt << endl;
					itBreeder->NewBreeder(floaters, newbreederFloater, newbreederHelper, inheritance);
				}
			}

			Reassign(floaters, groups);
			if (!floaters.empty()) {
				cout << "Not all floaters were reassigned!" << endl;
			}

            vector<Group, std::allocator<Group>>::iterator itAge;
            for (itAge = groups.begin(); itAge < groups.end(); ++itAge)
			{
				itAge->IncreaseAge(); //add 1 rank or age to all individuals alive

				population += itAge->TotalPopulation(); //calculate number of ind in the whole population
			}

			if (gen%SKIP == 0) {   // write output every 'skip' generations
				Statistics(groups);
				WriteMeans();
			}
			

			//Print last generation
			if (gen == NUM_GENERATIONS) {

				int groupID = 0;

                vector<Group, std::allocator<Group>>::iterator itGroups;
                for (itGroups = groups.begin(); itGroups < groups.end(); ++itGroups)
				{
					fout2 << fixed << showpoint
						<< replica + 1
						<< "\t" << groupID
						<< "\t" << itGroups->breeder.fishType
						<< "\t" << setprecision(4) << itGroups->breeder.age
						<< "\t" << setprecision(4) << itGroups->breeder.alpha
						<< "\t" << setprecision(4) << itGroups->breeder.alphaAge
						<< "\t" << setprecision(4) << itGroups->breeder.alphaAge2
						<< "\t" << setprecision(4) << itGroups->breeder.beta
						<< "\t" << setprecision(4) << itGroups->breeder.betaAge
						<< "\t" << setprecision(4) << itGroups->breeder.drift
						<< "\t" << setprecision(4) << "NA"
						<< "\t" << setprecision(4) << "NA"
						<< "\t" << setprecision(4) << itGroups->breeder.survival
						<< endl;

					for (vector<Individual>::iterator itHelpers = itGroups->helpers.begin(); itHelpers < itGroups->helpers.end(); ++itHelpers) {
						fout2 << fixed << showpoint 
							<< replica + 1 
							<< "\t" << groupID
							<< "\t" << itHelpers->fishType
							<< "\t" << setprecision(4) << itHelpers->age
							<< "\t" << setprecision(4) << itHelpers->alpha
							<< "\t" << setprecision(4) << itHelpers->alphaAge
							<< "\t" << setprecision(4) << itHelpers->alphaAge2
							<< "\t" << setprecision(4) << itHelpers->beta
							<< "\t" << setprecision(4) << itHelpers->betaAge
							<< "\t" << setprecision(4) << itHelpers->drift
							<< "\t" << setprecision(4) << itHelpers->help
							<< "\t" << setprecision(4) << itHelpers->dispersal
							<< "\t" << setprecision(4) << itHelpers->survival
							<< endl;
					}

					groupID++;
				}
			}


            vector<Group, std::allocator<Group>>::iterator itReproduction;
            for (itReproduction = groups.begin(); itReproduction < groups.end(); ++itReproduction)
			{
				itReproduction->Fecundity();
				itReproduction->Reproduction();
			}

		}

	}


	return 0;
}
