/***********************************************
 GROUP AUGMENTATION MODEL

 Level of help and dispersal with genetic bases. Inclusion of reaction norms.
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
//#include <chrono> //if we use the system clock as seed

using namespace std;

// Output file
ofstream fout("group_augmentation_init.txt");
ofstream fout2("group_augmentation_last_generation_init.txt");


/*CONSTANTS AND STRUCTURES*/

// Random numbers
//mt19937 mt(time(0)); // random number generator
//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); // if we don't want to obtain the same simulation under the same parameters
unsigned seed = 0; //in the same run takes different random values, but different runs same values unless we change the seed
default_random_engine generator(seed);
uniform_real_distribution<double> DriftUniform(0, 100);
uniform_real_distribution<double> Uniform(0, 1);


//Run parameters
const bool REACTION_NORM_HELP = 1;		//Apply reaction norm to age for level of help? 
const bool REACTION_NORM_DISPERSAL = 1;	//Apply reaction norm to age for dispersal? 

const int MAX_COLONIES	  = 5000;     // max number of groups or colonies --> breeding spots.
const int NUM_GENERATIONS = 200000;
const int MAX_NUM_REPLICATES  = 20;
const int SKIP = 50;   // interval between print-outs

//Fix values 
const double PREDATION = 0.1;
const double BIAS_FLOAT_BREEDER = 2;
const int    INIT_NUM_HELPERS = 3;

// Modifiers
const double K0     = 1;	// min fecundity, fecundity when no help provided.
const double K1     = 1;	// benefit of cumhelp in the fecundity
const double Xsh    = 1;	// cost of help in survival
const double Xsn    = 1;	// benefit of group size in survival


//Genetic values

	//For help
const double INIT_ALPHA			= 0.0;			// bigger values higher level of help
const double INIT_ALPHA_AGE		= 0.0;			//linear term for age, positive: higher help with age
const double INIT_ALPHA_AGE2	= 0.0;			//quadratic term for age, positive: higher help with age

const double MUTATION_ALPHA		= 0.05;			// mutation rate in alpha for level of help
const double MUTATION_ALPHA_AGE = 0.05;
const double MUTATION_ALPHA_AGE2= 0.05;			
const double STEP_ALPHA			= 0.01;			// mutation step size in alpha for level of help
    

	//For dispersal
const double INIT_BETA			= 1.0;			// bigger values higher dispersal
const double INIT_BETA_AGE		= 0.0;			// 0: age has no effect, positive: dispersal decreases with age

const double MUTATION_BETA		= 0.05;			// mutation rate for the propensity to disperse
const double MUTATION_BETA_AGE	= 0.05;    
const double STEP_BETA			= 0.01;			// mutation step size for the propensity to disperse


	//For relatedness
const double MUTATION_DRIFT		= 0.05;			// mutation rate in the neutral selected value to track level of relatedness
const double STEP_DRIFT			= 0.01;			// mutation step size in the neutral genetic value to track level of relatedness


enum classes { BREEDER, HELPER, FLOATER };

const int NO_VALUE = -1;

//Population parameters and Statistics
int replica, gen, population, populationBeforeSurv, deaths, floatersgenerated, driftGroupSize, maxGroupSize, populationHelpers;
double relatedness; 
double  meanGroupsize, stdevGroupSize,  sumGroupSize, sumsqGroupSize, varGroupSize,
meanAge, stdevAge, sumAge, sumsqAge, varAge,
meanAlpha, stdevAlpha, sumAlpha, sumsqAlpha, varAlpha,
meanAlphaAge, stdevAlphaAge, sumAlphaAge, sumsqAlphaAge, varAlphaAge,
meanAlphaAge2, stdevAlphaAge2, sumAlphaAge2, sumsqAlphaAge2, varAlphaAge2,
meanBeta, stdevBeta, sumBeta, sumsqBeta, varBeta,
meanBetaAge, stdevBetaAge, sumBetaAge, sumsqBetaAge, varBetaAge,
meanHelp, stdevHelp, sumHelp, sumsqHelp, varHelp,
meanDispersal, stdevDispersal, sumDispersal, sumsqDispersal, varDispersal,
meanDriftB, sumDriftB, meanDriftH, sumDriftH,											//relatedness
meanDriftBH, meanDriftBB, sumDriftBH, sumDriftBB,
corr_HelpDispersal, sumprodHelpDispersal;


//Structures

struct Individual // define individual traits
{
	Individual(double alpha_ = 0, double alphaAge_ = 0, double alphaAge2_ = 0, double beta_ = 0, double betaAge_ = 0, double drift_ = 0, classes own_ = HELPER);
	Individual(const Individual &mother);
	double alpha, alphaAge, alphaAge2, beta, betaAge, drift, 		// genetic values                      
		dispersal, help, survival;									// phenotypic values
	classes fishType;												// possible classes: breeder, helper, floater
	int age;

	//Functions inside Individual
	double calcDispersal();
	double calcHelp();
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
	survival = NO_VALUE;
	help = 0;
	dispersal = 0;
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
	survival = copy.survival;
	help = copy.help;
	dispersal = copy.dispersal;
}


struct Group // define group traits
{
	Group(double alpha_, double alphaAge_, double alphaAge2_, double beta_, double betaAge_, int numhelp_);
	double cumhelp;
	int totalHelpers;
	bool breederalive;                                     // for the breeder: 1 alive, 0 dead
	int groupSize;
	double fecundity;
	int realfecundity;

	Individual vbreeder; // create breeder inside group
	vector<Individual> vhelpers; // create a vector of helpers inside each group

//Functions inside Group
	void Dispersal(vector<Individual> &vfloaters);
	void Help();
	void SurvivalGroup(int &deaths);
	void NewBreeder(vector<Individual> &vfloaters);
	void IncreaseAge();
	double TotalPopulation();
	void Fecundity();
	void Reproduction();
};


Group::Group(double alpha_ = INIT_ALPHA, double alphaAge_ = INIT_ALPHA_AGE, double alphaAge2_ = INIT_ALPHA_AGE2, double beta_ = INIT_BETA, double betaAge_ = INIT_BETA_AGE, int numhelp_ = INIT_NUM_HELPERS)
{
	vbreeder = Individual(alpha_, alphaAge_, alphaAge2_, beta_, betaAge_, DriftUniform(generator), BREEDER);
	breederalive = 1;
	fecundity = NO_VALUE;
	realfecundity = NO_VALUE;

	for (int i = 0; i < numhelp_; ++i)
	{
		vhelpers.push_back(Individual(alpha_, alphaAge_, alphaAge2_, beta_, betaAge_, DriftUniform(generator), HELPER));
	}

	TotalPopulation();
}


/* INITIALISE POPULATION */
void InitGroup(vector<Group> &vgroups)
{
	int i;
	for (i = 0; i < MAX_COLONIES; i++)
	{
		vgroups[i] = Group(INIT_ALPHA, INIT_ALPHA_AGE, INIT_ALPHA_AGE2, INIT_BETA, INIT_BETA_AGE, INIT_NUM_HELPERS);
	}
}


/* BECOME FLOATER (STAY VS DISPERSE) */

double Individual::calcDispersal()
{
	if (!REACTION_NORM_DISPERSAL) {

		dispersal = beta; // Range from 0 to 1 to compare to a Uniform distribution

		if (dispersal < 0 || dispersal > 1) {
			cout << "error in dispersal: " << dispersal << endl;
		}

	}
	else {
		dispersal = 1 / (1 + exp(betaAge*age - beta));
	}

	return dispersal;
}


void Group::Dispersal(vector<Individual> &vfloaters)
{
	vector<Individual>::iterator dispersalIt = vhelpers.begin();
	int sizevec = vhelpers.size();
	int counting = 0;

	while (!vhelpers.empty() && sizevec > counting)
	{
		dispersalIt->calcDispersal();
		if (dispersalIt->dispersal < 0) {
			cout << "error in dispersal: " << dispersalIt->dispersal << endl;
		}

		if (Uniform(generator) < dispersalIt->dispersal)
		{
			vfloaters.push_back(*dispersalIt); //add the individual to the vector floaters in the last position
			vfloaters[vfloaters.size() - 1].fishType = FLOATER;
			*dispersalIt = vhelpers[vhelpers.size() - 1]; // this and next line removes the individual from the helpers vector
			vhelpers.pop_back();
			++counting;
		}
		else
			++dispersalIt, ++counting;
	}
	//cout << "floater size " << vfloaters.size() << endl;
}


/*DISPLAY LEVEL OF HELP*/
double Individual::calcHelp()
{
	if (!REACTION_NORM_HELP) {
		help = alpha;
	}
	else {
		help = alpha + alphaAge * age + alphaAge2 * age*age;
		if (help < 0) { help = 0; }
	}

	return help;
}


void Group::Help() //Calculate accumulative help of all individuals inside of each group.
{
	cumhelp = 0;
	//Level of help for helpers
	for (vector<Individual>::iterator helpIt = vhelpers.begin(); helpIt < vhelpers.end(); ++helpIt)
	{
		helpIt->calcHelp();
		cumhelp += helpIt->help;
	}
	//Level of help for breeders
	//vbreeder.calcHelp();
	//cumhelp += vbreeder.help;
}

/*SURVIVAL*/

double Individual::calcSurvival(int totalHelpers)
{
	survival = (1 - PREDATION) / (1 + exp(Xsh*help - Xsn * (totalHelpers + 1))); // +1 to know group size (1 breeder + helpers)

	return survival;
}


void Group::SurvivalGroup(int &deaths)
{
	totalHelpers = vhelpers.size();

	//Calculate the survival of the helpers

	vector<Individual>::iterator survHIt = vhelpers.begin();
	int sizevec = vhelpers.size();
	int counting = 0;
	while (!vhelpers.empty() && sizevec > counting)
	{
		//Calculate value of survival
		survHIt->calcSurvival(totalHelpers);

		//Mortality
		if (Uniform(generator) > survHIt->survival)
		{
			*survHIt = vhelpers[vhelpers.size() - 1];
			vhelpers.pop_back();
			++counting;
			deaths++;
		}
		else
			++survHIt, ++counting; //go to next individual
	}

	//Calculate the survival of the breeder
	vbreeder.calcSurvival(totalHelpers);
	if (Uniform(generator) > vbreeder.survival)
	{
		breederalive = 0;
		vbreeder.age = NO_VALUE;
		deaths++;
	}
}

void SurvivalFloaters(vector<Individual> &vfloaters, int &deaths) //Calculate the survival of the floaters
{
	vector<Individual>::iterator survFIt = vfloaters.begin();
	int sizevec = vfloaters.size();
	int counting = 0;
	while (!vfloaters.empty() && sizevec > counting)
	{
		//Calculate value of survival
		survFIt->calcSurvival(0);

		//Mortality
		if (Uniform(generator) > survFIt->survival)
		{
			*survFIt = vfloaters[vfloaters.size() - 1];
			vfloaters.pop_back();
			++counting;
			deaths++;
		}
		else
			++survFIt, ++counting; //go to next individual
	}
}

/* BECOME BREEDER */

void Group::NewBreeder(vector<Individual> &vfloaters)
{
	//    Select a random sample from the floaters
	int i = 0;
	int sumage = 0;
	double currentposition = 0; //age of the previous ind taken from Candidates
	int UniformFloatNum;
	double RandP = Uniform(generator);
	int proportFloaters = round(vfloaters.size() * BIAS_FLOAT_BREEDER / MAX_COLONIES); ///justify the 10 multiplier

	vector<Individual*> Candidates;
	vector<double>position; //vector of age to choose with higher likelihood the ind with higher age
	vector<int>TemporaryCandidates; // to prevent taking the same ind several times in the sample

	if (vfloaters.size() > 0 && vfloaters.size() > proportFloaters) {
		while (i < proportFloaters)
		{
			uniform_int_distribution<int> UniformFloat(0, vfloaters.size() - 1); //random floater ID taken in the sample
			UniformFloatNum = UniformFloat(generator);
			TemporaryCandidates.push_back(UniformFloatNum); //add references of the floaters sampled to a vector
			sort(TemporaryCandidates.begin(), TemporaryCandidates.end()); //sort vector 
			i++;
		}

		int temp = 0;
		for (vector<int>::iterator itTempCandidates = TemporaryCandidates.begin(); itTempCandidates < TemporaryCandidates.end(); ++itTempCandidates)
		{ 
			if (!(*itTempCandidates == temp)) //to make sure the same ind is not taken more than ones
			{
				Candidates.push_back(&vfloaters[UniformFloatNum]);
				temp = *itTempCandidates;
			}
		}
	}

	else { ///When less floaters available than the sample size, takes all of them. Change to a proportion 
		for (vector<Individual>::iterator floatIt = vfloaters.begin(); floatIt < vfloaters.end(); ++floatIt)
		{
			Candidates.push_back(&(*floatIt));
		}
	}


	//    Join the helpers in the group to the sample of floaters

	for (vector<Individual>::iterator helpIt = vhelpers.begin(); helpIt < vhelpers.end(); ++helpIt)
	{
		Candidates.push_back(&(*helpIt));
	}


//     Choose breeder with higher likelihood for the highest
	for (vector<Individual*>::iterator ageIt = Candidates.begin(); ageIt < Candidates.end(); ++ageIt) //ageIt creates a vector of pointers to an individual
	{
		sumage += (*ageIt)->age; //add all the age from the vector Candidates
	}

	for (vector<Individual*>::iterator age2It = Candidates.begin(); age2It < Candidates.end(); ++age2It)
	{
		position.push_back(static_cast<double>((*age2It)->age) / static_cast<double>(sumage) + currentposition); //creates a vector with proportional segments to the age of each individual
		currentposition = position[position.size() - 1];
	}

	vector<Individual*>::iterator age3It = Candidates.begin();
	int counting = 0;
	while (counting < Candidates.size()) {
		if (RandP < position[age3It - Candidates.begin()]) //to access the same ind in the candidates vector
		{
			vbreeder = **age3It; //substitute the previous dead breeder
			breederalive = 1;

			if ((*age3It)->fishType == FLOATER) //delete the ind from the vector floaters
			{
				**age3It = vfloaters[vfloaters.size() - 1];
				vfloaters.pop_back();
			}
			else
			{
				**age3It = vhelpers[vhelpers.size() - 1]; //delete the ind from the vector helpers
				vhelpers.pop_back();
			}

			vbreeder.fishType = BREEDER; //modify the class
			counting = Candidates.size();//end loop
		}
		else
			++age3It, ++counting;
	}
}


/*REASSIGN FLOATERS*/

void Reassign(vector<Individual> &vfloaters, vector<Group> &vgroups)
{
	uniform_int_distribution<int> UniformMaxCol(0, MAX_COLONIES - 1);
	int selectGroup;
	vector<Individual>::iterator indIt;
	while (!vfloaters.empty())
	{
		indIt = vfloaters.end() - 1;
		selectGroup = UniformMaxCol(generator);
		indIt->fishType = HELPER; //modify the class
		vgroups[selectGroup].vhelpers.push_back(*indIt); //add the floater to the helper vector in a randomly selected group
		vfloaters.pop_back(); //remove the floater from its vector
	}
}


/* INCREASE AGE*/
void Group::IncreaseAge()
{
	for (vector<Individual>::iterator ageIt = vhelpers.begin(); ageIt < vhelpers.end(); ++ageIt)
	{
		ageIt->age++;
	}
	if (breederalive == 1) {
		vbreeder.age++;
	}
	else {
		vbreeder.age = NO_VALUE; //to check for dead breeders still existing in print last generation 
	}
}


/* TOTAL NUMBER OF INDIVIDUALS PER GROUP*/

double Group::TotalPopulation()
{
	if (breederalive == 1) {
		groupSize = vhelpers.size() + 1;
	}
	else {
		groupSize = vhelpers.size();
	}

	return groupSize;
}


/* REPRODUCTION */

void Group::Fecundity()
{
	//fecundity = K0 + K1 * cumhelp;
	//fecundity = K0 + K1 * log(cumhelp+1);
	fecundity = K0 + K1 * cumhelp / (1 + cumhelp * K1);

	poisson_distribution<int> PoissonFec(fecundity);
	realfecundity = PoissonFec(generator);
}

void Group::Reproduction() // populate offspring generation
{
	if (breederalive == 1)
	{
		for (int i = 0; i < realfecundity; i++) //number of offspring dependent on real fecundity
		{
			vhelpers.push_back(Individual(vbreeder.alpha, vbreeder.alphaAge, vbreeder.alphaAge2, vbreeder.beta, vbreeder.betaAge, vbreeder.drift)); //create a new individual as helper in the group. Call construct to assign the mother genetic values to the offspring, construct calls Mutate function.
		}
	}
}


void Individual::Mutate() // mutate genome of offspring 
{
	normal_distribution <double> NormalA(0, STEP_ALPHA); ///could be simplified if I decide to have all the steps size with the same magnitude
	normal_distribution <double> NormalB(0, STEP_BETA);
	normal_distribution <double> NormalD(0, STEP_DRIFT);

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
void Statistics(vector<Group>vgroups) {

	relatedness = 0.0, driftGroupSize = 0, populationHelpers = 0,
		meanGroupsize = 0.0, stdevGroupSize = 0.0, maxGroupSize = 0, sumGroupSize = 0.0, sumsqGroupSize = 0.0, varGroupSize = 0.0,
		meanAge = 0.0, stdevAge = 0.0, sumAge = 0.0, sumsqAge = 0.0, varAge = 0.0,
		meanAlpha = 0.0, stdevAlpha = 0.0, sumAlpha = 0.0, sumsqAlpha = 0.0, varAlpha = 0.0,
		meanAlphaAge = 0.0, stdevAlphaAge = 0.0, sumAlphaAge = 0.0, sumsqAlphaAge = 0.0, varAlphaAge = 0.0,
		meanAlphaAge2 = 0.0, stdevAlphaAge2 = 0.0, sumAlphaAge2 = 0.0, sumsqAlphaAge2 = 0.0, varAlphaAge2 = 0.0,
		meanBeta = 0.0, stdevBeta = 0.0, sumBeta = 0.0, sumsqBeta = 0.0, varBeta = 0.0,
		meanBetaAge = 0.0, stdevBetaAge = 0.0, sumBetaAge = 0.0, sumsqBetaAge = 0.0, varBetaAge = 0.0,
		meanHelp = 0.0, stdevHelp = 0.0, sumHelp = 0.0, sumsqHelp = 0.0, varHelp = 0.0,
		meanDispersal = 0.0, stdevDispersal = 0.0, sumDispersal = 0.0, sumsqDispersal = 0.0, varDispersal = 0.0,
		meanDriftB = 0.0, sumDriftB = 0.0, meanDriftH = 0.0, sumDriftH = 0.0,
		meanDriftBH = 0.0, meanDriftBB = 0.0, sumDriftBH = 0.0, sumDriftBB = 0.0, 
		corr_HelpDispersal = 0.0, sumprodHelpDispersal = 0.0;

	for (vector<Group>::iterator groupStatsIt = vgroups.begin(); groupStatsIt < vgroups.end(); ++groupStatsIt) {

		for (vector<Individual>::iterator indStatsIt = groupStatsIt->vhelpers.begin(); indStatsIt < groupStatsIt->vhelpers.end(); ++indStatsIt) {

			sumAge += indStatsIt->age;
			sumsqAge += indStatsIt->alpha*indStatsIt->age;
			
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

			sumHelp += indStatsIt->help;
			sumsqHelp += indStatsIt->help*indStatsIt->help;

			sumDispersal += indStatsIt->dispersal;
			sumsqDispersal += indStatsIt->dispersal*indStatsIt->dispersal;

			sumprodHelpDispersal += indStatsIt->help*indStatsIt->dispersal;

			if (groupStatsIt->breederalive) {
				sumDriftB += groupStatsIt->vbreeder.drift;
				sumDriftH += indStatsIt->drift;
				sumDriftBH += indStatsIt->drift*groupStatsIt->vbreeder.drift;
				sumDriftBB += groupStatsIt->vbreeder.drift*groupStatsIt->vbreeder.drift;
				++driftGroupSize;
			}
		}

		populationHelpers += groupStatsIt->vhelpers.size();

		sumGroupSize += groupStatsIt->groupSize;
		sumsqGroupSize += groupStatsIt->groupSize*groupStatsIt->groupSize;
		if (groupStatsIt->groupSize > maxGroupSize) maxGroupSize = groupStatsIt->groupSize;

		if (groupStatsIt->breederalive == 1) sumAge += groupStatsIt->vbreeder.age;
		if (groupStatsIt->breederalive == 1) sumsqAge += groupStatsIt->vbreeder.age*groupStatsIt->vbreeder.age;

		if (groupStatsIt->breederalive == 1) sumAlpha += groupStatsIt->vbreeder.alpha;
		if (groupStatsIt->breederalive == 1) sumsqAlpha += groupStatsIt->vbreeder.alpha*groupStatsIt->vbreeder.alpha;

		if (groupStatsIt->breederalive == 1) sumAlphaAge += groupStatsIt->vbreeder.alphaAge;
		if (groupStatsIt->breederalive == 1) sumsqAlphaAge += groupStatsIt->vbreeder.alphaAge*groupStatsIt->vbreeder.alphaAge;

		if (groupStatsIt->breederalive == 1) sumAlphaAge2 += groupStatsIt->vbreeder.alphaAge2;
		if (groupStatsIt->breederalive == 1) sumsqAlphaAge2 += groupStatsIt->vbreeder.alphaAge2*groupStatsIt->vbreeder.alphaAge2;

		if (groupStatsIt->breederalive == 1) sumBeta += groupStatsIt->vbreeder.beta;
		if (groupStatsIt->breederalive == 1) sumsqBeta += groupStatsIt->vbreeder.beta*groupStatsIt->vbreeder.beta;

		if (groupStatsIt->breederalive == 1) sumBetaAge += groupStatsIt->vbreeder.betaAge;
		if (groupStatsIt->breederalive == 1) sumsqBetaAge += groupStatsIt->vbreeder.betaAge*groupStatsIt->vbreeder.betaAge;

	}

	meanGroupsize = sumGroupSize / MAX_COLONIES;
	meanAge = sumAge / population;
	meanAlpha = sumAlpha / population; ///population=sumGroupSize so simplify!
	meanAlphaAge = sumAlphaAge / population;
	meanAlphaAge2 = sumAlphaAge2 / population;
	meanBeta = sumBeta / population;
	meanBetaAge = sumBetaAge / population;
	meanDriftB = sumDriftB / driftGroupSize;
	meanDriftH = sumDriftH / driftGroupSize;
	meanDriftBH = sumDriftBH / driftGroupSize;
	meanDriftBB = sumDriftBB / driftGroupSize;
	meanHelp = sumHelp / populationHelpers;
	meanDispersal = sumDispersal / populationHelpers;

	relatedness = (meanDriftBH - meanDriftB * meanDriftH) / (meanDriftBB - meanDriftB * meanDriftB);
	if ((meanDriftBB - meanDriftB * meanDriftB) == 0) { relatedness = 2; } //prevent to divide by 0

	/*if (gen == 60500)
	{
		cout << "meanDriftB= " << meanDriftB << '/t' << "meanDriftH= " << meanDriftH << '/t' << "meanDriftBH= " << meanDriftBH << '/t' << "meanDriftBB= " << meanDriftBB << endl;
		cout << "numerator=" << meanDriftBH - meanDriftB * meanDriftH << '/t' << "denominator= " << meanDriftBB - meanDriftB * meanDriftB << endl;
		cout << "relatedness= " << relatedness << endl;
	}*/

	varGroupSize = sumsqGroupSize / MAX_COLONIES - meanGroupsize * meanGroupsize;
	varAge = sumsqAge / population - meanAge * meanAge;
	varAlpha = sumsqAlpha / population - meanAlpha * meanAlpha;
	varAlphaAge = sumsqAlphaAge / population - meanAlphaAge * meanAlphaAge;
	varAlphaAge2 = sumsqAlphaAge2 / population - meanAlphaAge2 * meanAlphaAge2;
	varBeta = sumsqBeta / population - meanBeta * meanBeta;
	varBetaAge = sumsqBetaAge / population - meanBetaAge * meanBetaAge;
	varHelp = sumsqHelp / populationHelpers - meanHelp * meanHelp;
	varDispersal = sumsqDispersal / populationHelpers - meanDispersal * meanDispersal;

	// to know if there is a problem (variance cannot be negative)
	varGroupSize > 0 ? stdevGroupSize = sqrt(varGroupSize) : stdevGroupSize = 0;
	varAge > 0 ? stdevAge = sqrt(varAge) : stdevAge = 0;
	varAlpha > 0 ? stdevAlpha = sqrt(varAlpha) : stdevAlpha = 0;
	varAlphaAge > 0 ? stdevAlphaAge = sqrt(varAlphaAge) : stdevAlphaAge = 0;
	varAlphaAge2 > 0 ? stdevAlphaAge2 = sqrt(varAlphaAge2) : stdevAlphaAge2 = 0;
	varBeta > 0 ? stdevBeta = sqrt(varBeta) : stdevBeta = 0;
	varBetaAge > 0 ? stdevBetaAge = sqrt(varBetaAge) : stdevBetaAge = 0;
	varHelp > 0 ? stdevHelp = sqrt(varHelp) : stdevHelp = 0;
	varDispersal > 0 ? stdevDispersal = sqrt(varDispersal) : stdevDispersal = 0;

	(stdevHelp > 0 && stdevDispersal > 0) ? corr_HelpDispersal = (sumprodHelpDispersal / populationHelpers - meanHelp * meanDispersal) / (stdevHelp*stdevDispersal) : corr_HelpDispersal = 0;
}



/* WRITE PARAMETER SETTINGS TO OUTPUT FILE */
void Printparams()
{
	fout << "PARAMETER VALUES" << endl

		<< "Reaction norm help: "	   << "\t" << REACTION_NORM_HELP << endl
		<< "Reaction norm dispersal: " << "\t" << REACTION_NORM_DISPERSAL << endl
		<< "Initial population: " << "\t" << MAX_COLONIES * (INIT_NUM_HELPERS + 1) << endl
		<< "Number of colonies: " << "\t" << MAX_COLONIES << endl
		<< "Number generations: " << "\t" << NUM_GENERATIONS << endl
		<< "Number replicates: " << "\t" << MAX_NUM_REPLICATES << endl
		<< "Predation: " << "\t"		<< PREDATION << endl
		<< "Bias float breeder: "<<"\t" << BIAS_FLOAT_BREEDER << endl
		<< "initAlpha: " << "\t"		<< INIT_ALPHA << endl
		<< "initAlphaAge: " << "\t"		<< INIT_ALPHA_AGE << endl
		<< "initAlphaAge2: " << "\t"	<< INIT_ALPHA_AGE2 << endl
		<< "initBeta: " << "\t"			<< INIT_BETA << endl
		<< "initBetaAge: " << "\t"		<< INIT_BETA_AGE << endl
		<< "mutAlpha: " << "\t"			<< MUTATION_ALPHA << endl
		<< "mutAlphaAge: " << "\t"		<< MUTATION_ALPHA_AGE << endl
		<< "mutAlphaAge2: " << "\t"		<< MUTATION_ALPHA_AGE2 << endl
		<< "mutBeta: " << "\t"			<< MUTATION_BETA << endl
		<< "mutBetaAge: " << "\t"		<< MUTATION_BETA_AGE << endl
		<< "mutDrift: " << "\t"			<< MUTATION_DRIFT << endl
		<< "stepAlpha: " << "\t"		<< STEP_ALPHA << endl
		<< "stepBeta: " << "\t"			<< STEP_BETA << endl
		<< "stepDrift: " << "\t"		<< STEP_DRIFT << endl
		<< "K0: " << "\t"	<< K0 << endl
		<< "K1: " << "\t"	<< K1 << endl
		<< "Xsh: " << "\t"	<< Xsh << endl
		<< "Xsn: " << "\t"	<< Xsn << endl << endl;

	fout2 << "PARAMETER VALUES" << endl

		<< "Reaction norm help: "	   << "\t" << REACTION_NORM_HELP << endl
		<< "Reaction norm dispersal: " << "\t" << REACTION_NORM_DISPERSAL << endl
		<< "Initial population: " << "\t" << MAX_COLONIES * (INIT_NUM_HELPERS + 1) << endl
		<< "Number of colonies: " << "\t" << MAX_COLONIES << endl
		<< "Number generations: " << "\t" << NUM_GENERATIONS << endl
		<< "Number replicates: " << "\t" << MAX_NUM_REPLICATES << endl
		<< "Predation: " << "\t"		<< PREDATION << endl
		<< "Bias float breeder: "<<"\t" << BIAS_FLOAT_BREEDER << endl
		<< "initAlpha: " << "\t"		<< INIT_ALPHA << endl
		<< "initAlphaAge: " << "\t"		<< INIT_ALPHA_AGE << endl
		<< "initAlphaAge2: " << "\t"	<< INIT_ALPHA_AGE2 << endl
		<< "initBeta: " << "\t"			<< INIT_BETA << endl
		<< "initBetaAge: " << "\t"		<< INIT_BETA_AGE << endl
		<< "mutAlpha: " << "\t"			<< MUTATION_ALPHA << endl
		<< "mutAlphaAge: " << "\t"		<< MUTATION_ALPHA_AGE << endl
		<< "mutAlphaAge2: " << "\t"		<< MUTATION_ALPHA_AGE2 << endl
		<< "mutBeta: " << "\t"			<< MUTATION_BETA << endl
		<< "mutBetaAge: " << "\t"		<< MUTATION_BETA_AGE << endl
		<< "mutDrift: " << "\t"			<< MUTATION_DRIFT << endl
		<< "stepAlpha: " << "\t"		<< STEP_ALPHA << endl
		<< "stepBeta: " << "\t"			<< STEP_BETA << endl
		<< "stepDrift: " << "\t"		<< STEP_DRIFT << endl
		<< "K0: " << "\t"	<< K0 << endl
		<< "K1: " << "\t"	<< K1 << endl
		<< "Xsh: " << "\t"	<< Xsh << endl
		<< "Xsn: " << "\t"	<< Xsn << endl << endl;

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
		<< setw(9) << setprecision(2) << meanGroupsize
		<< setw(9) << maxGroupSize
		<< setw(9) << setprecision(2) << meanAge
		<< setw(9) << setprecision(4) << meanAlpha
		<< setw(9) << setprecision(4) << meanAlphaAge
		<< setw(9) << setprecision(4) << meanAlphaAge2
		<< setw(9) << setprecision(4) << meanBeta
		<< setw(9) << setprecision(4) << meanBetaAge
		<< setw(9) << setprecision(4) << relatedness
		<< endl;


	// write values to output file
	fout << fixed << showpoint 
		<< gen
		<< "\t" << population
		<< "\t" << deaths
		<< "\t" << setprecision(2) << meanGroupsize
		<< "\t" << setprecision(4) << meanAge
		<< "\t" << setprecision(4) << meanAlpha
		<< "\t" << setprecision(4) << meanAlphaAge
		<< "\t" << setprecision(4) << meanAlphaAge2
		<< "\t" << setprecision(4) << meanBeta
		<< "\t" << setprecision(4) << meanBetaAge
		<< "\t" << setprecision(4) << meanHelp
		<< "\t" << setprecision(4) << meanDispersal
		<< "\t" << setprecision(4) << relatedness
		<< "\t" << setprecision(2) << stdevGroupSize
		<< "\t" << setprecision(2) << stdevAge
		<< "\t" << setprecision(4) << stdevAlpha
		<< "\t" << setprecision(4) << stdevAlphaAge
		<< "\t" << setprecision(4) << stdevAlphaAge2
		<< "\t" << setprecision(4) << stdevBeta
		<< "\t" << setprecision(4) << stdevBetaAge
		<< "\t" << setprecision(4) << stdevHelp
		<< "\t" << setprecision(4) << stdevDispersal
		<< "\t" << setprecision(4) << corr_HelpDispersal
		<< endl;
}
void wait_for_return()
{
	std::cout << "Hit <Enter> to continue\n";
	getchar();
}

/* MAIN PROGRAM */
int main() {
	
	Printparams();
	
	// column headings in output file 1
	fout << "Generation" << "\t" << "Population" << "\t" << "Deaths" << "\t"
		<< "Group_size" << "\t" << "Age" << "\t" << "meanAlpha" << "\t" << "meanAlphaAge" << "\t" << "meanAlphaAge2" << "\t"
		<< "meanBeta" << "\t" << "meanBetaAge" << "\t" << "meanHelp" << "\t" << "meanDispersal" << "\t" << "Relatedness" << "\t"
		<< "SD_GroupSize" << "\t" << "SD_Age" << "\t" << "SD_Alpha" << "\t" << "SD_AlphaAge" << "\t" << "SD_AlphaAge2" << "\t"
		<< "SD_Beta" << "\t" << "SD_BetaAge" << "\t" << "SD_Help" << "\t" << "SD_Dispersal" << "\t" << "corr_Help_Disp" << endl;

	// column headings in output file 2
	fout2 << "replica" << "\t" << "groupID" << "\t" << "type" << "\t" << "age" << "\t" 
		<< "alpha" << "\t" << "alphaAge" << "\t" << "alphaAge2"<< "\t" 
		<< "beta" << "\t" << "betaAge" << "\t" << "drift"  << endl;



	for (replica = 0; replica < MAX_NUM_REPLICATES; replica++) {

		gen = 0;
		deaths = 0; // to keep track of how many individuals die each generation
		population = 0; //total of ind in the whole simulation for the expecific generation
		populationBeforeSurv = 0;
		floatersgenerated = 0;

		// column headings on screen
		cout << setw(6) << "gen" << setw(9) << "pop" << setw(9) << "deaths" << setw(9)
			<< "float" << setw(9) << "group" << setw(9) << "maxGroup" << setw(9) << "age" << setw(9) 
			<< "alpha" << setw(9) << "alphaAge" << setw(9) << "alphaAge2" << setw(9)
			<< "beta" << setw(9) << "betaAge" << setw(9) << "relat" << endl;


		vector<Individual> vfloaters;
		vector<Group> vgroups(MAX_COLONIES);

		InitGroup(vgroups);

		for (vector<Group>::iterator relatednessIt = vgroups.begin(); relatednessIt < vgroups.end(); ++relatednessIt) {
			population += relatednessIt->TotalPopulation(); //calculate number of ind in the whole population
		}

		Statistics(vgroups);
		WriteMeans();

		for (gen = 1; gen <= NUM_GENERATIONS; gen++)
		{
			//cout << "\t" << "\t" << "\t" << "\t" << "\t" << "GENERATION "<<gen<< " STARTS NOW!!!" <<endl;

			//if (gen == 60550) {
			//	wait_for_return();
			//}

			deaths = 0; // to keep track of how many individuals die each generation
			population = 0; //total of ind in the whole simulation for the expecific generation
			populationBeforeSurv = 0;
			floatersgenerated = 0;

			//cout << "Floaters before dispersal: " << vfloaters.size() << endl;
			for (vector<Group>::iterator itDispersal = vgroups.begin(); itDispersal < vgroups.end(); ++itDispersal)
			{
				populationBeforeSurv += itDispersal->TotalPopulation();
				itDispersal->Dispersal(vfloaters);
				floatersgenerated = vfloaters.size();
				//    cout << "Floaters after dispersal: " << vfloaters.size() << endl;
			}

			for (vector<Group>::iterator itHelpSurvival = vgroups.begin(); itHelpSurvival < vgroups.end(); ++itHelpSurvival)
			{
				itHelpSurvival->Help();
				itHelpSurvival->SurvivalGroup(deaths);
			}

			SurvivalFloaters(vfloaters, deaths);
			//           cout << "total deaths: " << deaths << endl;

			for (vector<Group>::iterator itBreeder = vgroups.begin(); itBreeder < vgroups.end(); ++itBreeder)
			{
				if (itBreeder->breederalive == 0)
				{
					//cout << vgroups.begin() - breederIt << endl;
					itBreeder->NewBreeder(vfloaters);
				}
			}

			Reassign(vfloaters, vgroups);
			if (!vfloaters.empty()) {
				cout << "Not all floaters were reassigned!" << endl;
			}

			for (vector<Group>::iterator itAgeRelatedness = vgroups.begin(); itAgeRelatedness < vgroups.end(); ++itAgeRelatedness)
			{
				itAgeRelatedness->IncreaseAge(); //add 1 rank or age to all individuals alive

				population += itAgeRelatedness->TotalPopulation(); //calculate number of ind in the whole population
			}

			if (gen%SKIP == 0) {   // write output every 'skip' generations
				Statistics(vgroups);
				WriteMeans();
			}

			//Print last generation
			if (gen == NUM_GENERATIONS) {

				int groupID = 0;

				for (vector<Group>::iterator itGroups = vgroups.begin(); itGroups < vgroups.end(); ++itGroups)
				{
					fout2 << fixed << showpoint
						<< replica + 1
						<< "\t" << groupID
						<< "\t" << itGroups->vbreeder.fishType
						<< "\t" << setprecision(4) << itGroups->vbreeder.age
						<< "\t" << setprecision(4) << itGroups->vbreeder.alpha
						<< "\t" << setprecision(4) << itGroups->vbreeder.alphaAge
						<< "\t" << setprecision(4) << itGroups->vbreeder.alphaAge2
						<< "\t" << setprecision(4) << itGroups->vbreeder.beta
						<< "\t" << setprecision(4) << itGroups->vbreeder.betaAge
						<< "\t" << setprecision(4) << itGroups->vbreeder.drift
						<< endl;

					for (vector<Individual>::iterator itHelpers = itGroups->vhelpers.begin(); itHelpers < itGroups->vhelpers.end(); ++itHelpers) {
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
							<< endl;
					}

					groupID++;
				}
			}

			for (vector<Group>::iterator itReproduction = vgroups.begin(); itReproduction < vgroups.end(); ++itReproduction)
			{
				itReproduction->Fecundity();
				itReproduction->Reproduction();
			}

		}

		//fout << endl << endl << endl;
		//fout2 << endl << endl << endl;

	}

	wait_for_return();

	return 0;
}
