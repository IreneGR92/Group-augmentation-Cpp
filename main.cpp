/***********************************************
 GROUP AUGMENTATION BASIC MODEL

 Level of help and dispersal with genetic bases. No reaction norms.
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

ofstream fout("group_augmentation.txt");     // output file


/*CONSTANTS AND STRUCTURES*/

// Random numbers
//mt19937 mt(time(0)); // random number generator
//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count(); // if we don't want to obtain the same simulation under the same parameters
unsigned seed = 0; //in the same run takes different random values, but different runs same values unless we change the seed
default_random_engine generator(seed);
//normal_distribution<double> DriftNormal(0, 10);
uniform_real_distribution<double> DriftNormal(0, 10); //not relevent which one we use
uniform_real_distribution<double> Uniform(0, 1);


//Run parameters
const int MAX_COLONIES = 100;     // max number of groups or colonies --> breeding spots. Whole population size = maxcolon * (numhelp + 1)
const int INIT_NUM_HELPERS = 2;

const int NUM_GENERATIONS = 10000;
const int NUM_REPLICATES = 1;
const int SKIP = 50;   // interval between print-outs

//Fix values 
const double PREDATION = 0.1;

// Modifiers
//const double X0r    = 1; // inflexion point in the level of help formula for the influence of rank/age
//const double X0n    = 1; // inflexion point in the level of help formula for the influence of group size
const double K0 = 1; // min fecundity, fecundity when no help provided.
const double K1 = 1; // benefit of cumhelp in the fecundity
const double Xsh = 1; // cost of help in survival
const double Xsn = 2; // benefit of group size in survival


//Genetic values
const double INIT_ALPHA = 0.0;     // starting value of alpha (in gen 0)
const double MUTATION_ALPHA = 0.05;    // mutation rate in alpha for level of help
const double STEP_ALPHA = 0.1;     // mutation step size in alpha for level of help
const double INIT_BETA = 0.0;     // starting value of beta (in gen 0)
const double MUTATION_BETA = 0.05;    // mutation rate in beta for the propensity to disperse
const double STEP_BETA = 0.1;     // mutation step size in beta for the propensity to disperse
const double MUTATION_DRIFT = 0.05;    // mutation rate in the neutral selected value to track level of relatedness
const double STEP_DRIFT = 0.1;     // mutation step size in the neutral genetic value to track level of relatedness

//const int minsurv     = 50;     // min number of individual that survive
const int CAP_NUM_HELPERS = 7;	  // cap for the total number of individuals inside a group. Affect fecundity no survival (assumes smaller size fish will die)
const double CAP_SURVIVAL = 0.4;	  //adds extra mortality when population is bigger than CAP_NUM_HELPERS*MAX_COLONIES
const double PROB_CAP_SURVIVAL = 0.2;

//const double avFloatersSample = 10; ///average number of floaters sampled from the total ///Check first if there are enough floaters, take a proportion instead??

enum classes { BREEDER, HELPER, FLOATER };

const int NO_VALUE = -1;

//Statistics
int gen;
int population, populationBeforeSurv, deaths, floatersgenerated, emptyGroup, totalHelpersPop;
double relatednessGl; //global relatedness, covariance of population parameters
double  meanGroupsize, stdevGroupSize, maxGroupSize, sumGroupSize, sumsqGroupSize, varGroupSize,
meanAlpha, stdevAlpha, sumAlpha, sumsqAlpha, varAlpha,
meanBeta, stdevBeta, sumBeta, sumsqBeta, varBeta,
meanDriftB, sumDriftB, meanDriftH, sumDriftH,
meanDriftBH, meanDriftBB, sumDriftBH, sumDriftBB, productDriftHB, productDriftBB, //global relatedness
meanRelatedness, stdevRelatedness, sumRelatedness, sumsqRelatedness, varRelatedness, //within group relatedness
corr_AlphaBeta, sumprodAlphaBeta;


//Structures

struct Individual // define individual traits
{
	Individual(double alpha_ = 0, double beta_ = 0, double drift_ = 0, classes own_ = HELPER);
	Individual(const Individual &mother);
	double alpha, beta, drift,                        // genetic values
		dispersal, help, survival;                // phenotypic values
	classes own;                                      // possible classes: breeder, helper, floater
	int age;

	//Functions inside Individual
	double calcDispersal();
	double calcHelp();
	double calcSurvival(int totalHelpers);
	void Mutate();
};


Individual::Individual(double alpha_, double beta_, double drift_, classes own_) {
	alpha = alpha_;
	beta = beta_;
	drift = drift_;
	Mutate();
	own = own_;
	age = 1;
	survival = NO_VALUE;
	help = NO_VALUE;
	dispersal = NO_VALUE;
}

Individual::Individual(const Individual &copy) {
	alpha = copy.alpha;
	beta = copy.beta;
	drift = copy.drift;
	own = copy.own;
	age = copy.age;
	survival = copy.survival;
	help = copy.help;
	dispersal = copy.dispersal;
}


struct Group // define group traits
{
	Group(double alpha_, double beta_, int numhelp_);
	double cumhelp;
	int totalHelpers;
	bool breederalive;                                     // for the breeder: 1 alive, 0 dead
	int groupSize;
	double fecundity;
	int realfecundity;
	double driftHelpers, relatedness, productDriftHB, productDriftBB;

	Individual vbreeder; // create breeder inside group
	vector<Individual> vhelpers; // create a vector of helpers inside each group

//Functions inside Group
	void Dispersal(vector<Individual> &vfloaters);
	void Help();
	void SurvivalGroup(int &deaths);
	void Breeder(vector<Individual> &vfloaters);
	void IncreaseAge();
	void Relatedness();
	void ProductDrift();
	double TotalPopulation();
	void Fecundity();
	void Reproduction();
};


Group::Group(double alpha_ = INIT_ALPHA, double beta_ = INIT_BETA, int numhelp_ = INIT_NUM_HELPERS)
{
	vbreeder = Individual(alpha_, beta_, DriftNormal(generator), BREEDER);
	breederalive = 1;
	fecundity = NO_VALUE;
	realfecundity = NO_VALUE;

	for (int i = 0; i < numhelp_; ++i)
	{
		vhelpers.push_back(Individual(alpha_, beta_, DriftNormal(generator), HELPER));
	}
}


/* INITIALISE POPULATION */
void InitGroup(vector<Group> &vgroups)
{
	int i;
	for (i = 0; i < MAX_COLONIES; i++)
	{
		vgroups[i] = Group(INIT_ALPHA, INIT_BETA, INIT_NUM_HELPERS);
	}
}


/* BECOME FLOATER (STAY VS DISPERSE) */

double Individual::calcDispersal()
{
	dispersal = beta; // Range from 0 to 1 to compare to a Uniform distribution

	if (dispersal < 0 || dispersal > 1) {
		cout << "error in dispersal: " << dispersal << endl;
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
			vfloaters[vfloaters.size() - 1].own = FLOATER;
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
	help = alpha;
	if (alpha < 0)
	{
		cout << "error in help: " << alpha << endl;
	}
	//help = 1 / (1 + exp (alpha * (age - X0r) - beta * (Group.size - X0n)); //later on it will need to be inside group

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
	vbreeder.calcHelp();
	cumhelp += vbreeder.help;
}

/*SURVIVAL*/

double Individual::calcSurvival(int totalHelpers)
{
	survival = (1 - PREDATION) / (1 + exp(Xsh*help - Xsn * (totalHelpers + 1))); // +1 to know group size (1 breeder + helpers)
	if ((populationBeforeSurv > MAX_COLONIES*CAP_NUM_HELPERS || totalHelpers > CAP_NUM_HELPERS) && Uniform(generator) > PROB_CAP_SURVIVAL) {
		survival -= CAP_SURVIVAL;
	}
	if (survival < 0) {
		survival = 0;
	}
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
	vbreeder.calcSurvival(vhelpers.size());
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
		//        cout << survFIt->own << "   " <<survFIt->age << endl;

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

void Group::Breeder(vector<Individual> &vfloaters)
{
	//    Select a random sample from the floaters
	int i = 0;
	int sumage = 0;
	double currentposition = 0; //age of the previous ind taken from Candidates
	int UniformFloatNum;
	//poisson_distribution<int> PoissonFloat(avFloatersSample); //random sample size of floaters taken to compete for breeding spot
	double RandP = Uniform(generator);
	//int RandN = PoissonFloat (generator);
	int proportFloaters = round(vfloaters.size() * 10 / MAX_COLONIES); ///justify the 10 multiplier

	//cout << proportFloaters << endl;

	vector<Individual*> Candidates;
	vector<double>position; //vector of age to choose with higher likelihood the ind with higher age
	vector<int>TemporaryCandidates; // to prevent taking the same ind several times in the sample

	//        cout << "vfloaters: " << vfloaters.size() <<endl;

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

	//cout << "Helpers size: " << vhelpers.size() << endl;
	//cout << "Candidates size: " << Candidates.size() <<endl;

//     Choose breeder with higher likelihood for the highest
	for (vector<Individual*>::iterator ageIt = Candidates.begin(); ageIt < Candidates.end(); ++ageIt) //ageIt creates a vector of pointers to an individual
	{
		sumage += (*ageIt)->age; //add all the age from the vector Candidates
//            cout << (*ageIt)->own << "  " << (*ageIt)->age << endl;

	}
	//        cout << "sumage: " << sumage <<endl;
	//        cout << "size candidates: " << Candidates.size() <<endl;

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

			if ((*age3It)->own == FLOATER) //delete the ind from the vector floaters
			{
				**age3It = vfloaters[vfloaters.size() - 1];
				vfloaters.pop_back();
			}
			else
			{
				**age3It = vhelpers[vhelpers.size() - 1]; //delete the ind from the vector helpers
				vhelpers.pop_back();
			}

			vbreeder.own = BREEDER; //modify the class
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
	int selecGroup;
	vector<Individual>::iterator indIt;
	while (!vfloaters.empty())
	{
		indIt = vfloaters.end() - 1;
		selecGroup = UniformMaxCol(generator);
		indIt->own = HELPER; //modify the class
		vgroups[selecGroup].vhelpers.push_back(*indIt); //add the floater to the helper vector in a randomly selected group
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
		//        cout << "Age breeder: " << vbreeder.age << endl;
	}
	else {
		vbreeder.age = NO_VALUE;
		//		 cout << "Age breeder: BREEDER DEAD!" << endl;
	}
}

/* RELATEDNESS*/
void Group::Relatedness() { //0 means they are completly related

	driftHelpers = 0;
	relatedness = 0;

	for (vector<Individual>::iterator relatednessIt = vhelpers.begin(); relatednessIt < vhelpers.end(); ++relatednessIt)
	{
		driftHelpers += relatednessIt->drift;
		//cout<< driftHelpers/ vhelpers.size()
	}

	if (breederalive == 1 && vhelpers.size() != 0)
	{
		relatedness = abs(vbreeder.drift - driftHelpers / vhelpers.size());
	}
	else
	{
		relatedness = NO_VALUE;
	}
}

void Group::ProductDrift() { //to calculate global relatedness

	driftHelpers = 0;
	emptyGroup = 0;

	for (vector<Individual>::iterator relatednessIt = vhelpers.begin(); relatednessIt < vhelpers.end(); ++relatednessIt)
	{
		driftHelpers += relatednessIt->drift;
	}

	if (breederalive == 1 && vhelpers.size() != 0)
	{
		driftHelpers = driftHelpers / vhelpers.size();

		productDriftHB = vbreeder.drift * driftHelpers;
		productDriftBB = vbreeder.drift * vbreeder.drift;
	}
	else
	{
		productDriftHB = NO_VALUE;
	}

	if (breederalive == 0) {
		emptyGroup++;
	}
}


/*CALCULATE TOTAL NUMBER OF INDIVIDUALS PER GROUP*/

double Group::TotalPopulation()
{
	groupSize = vhelpers.size() + 1;
	return groupSize;
}


/* REPRODUCTION */

void Group::Fecundity()
{
	fecundity = K0 + K1 * cumhelp;
	poisson_distribution<int> PoissonFec(fecundity);
	realfecundity = PoissonFec(generator);

	//    cout << "Fecundity: " << fecundity <<"\t"<< "Real Fecundity: " << realfecundity << endl;
}

void Group::Reproduction() // populate offspring generation
{
	if (breederalive == 1)
	{
		for (int i = 0; i < realfecundity; i++) //number of offspring dependent on real fecundity
		{
			vhelpers.push_back(Individual(vbreeder.alpha, vbreeder.beta, vbreeder.drift)); //create a new individual as helper in the group. Call construct to assign the mother genetic values to the offspring, construct calls Mutate function.
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
		if (alpha < 0) { alpha = 0; }
	}

	if (Uniform(generator) < MUTATION_BETA) {
		beta += NormalB(generator);
		if (beta < 0) { beta = 0; }
		if (beta > 1) { beta = 1; }
	}

	if (Uniform(generator) < MUTATION_DRIFT) {
		drift += NormalD(generator);
	}
}


/* CALCULATE STATISTICS */
void Statistics(vector<Group>vgroups) {

	int exludedGroups = 0;
	relatednessGl = 0;

	for (vector<Group>::iterator groupStatsIt = vgroups.begin(); groupStatsIt < vgroups.end(); ++groupStatsIt) {

		for (vector<Individual>::iterator indStatsIt = groupStatsIt->vhelpers.begin(); indStatsIt < groupStatsIt->vhelpers.end(); ++indStatsIt) {

			sumAlpha += indStatsIt->alpha;
			sumsqAlpha += indStatsIt->alpha*indStatsIt->alpha;

			sumBeta += indStatsIt->beta;
			sumsqBeta += indStatsIt->beta*indStatsIt->beta;

			sumDriftH += indStatsIt->drift;
		}

		totalHelpersPop += groupStatsIt->groupSize;

		sumGroupSize += groupStatsIt->groupSize;
		sumsqGroupSize += groupStatsIt->groupSize*groupStatsIt->groupSize;
		if (groupStatsIt->groupSize > maxGroupSize) maxGroupSize = groupStatsIt->groupSize;

		sumAlpha += groupStatsIt->vbreeder.alpha;
		sumsqAlpha += groupStatsIt->vbreeder.alpha*groupStatsIt->vbreeder.alpha;

		sumBeta += groupStatsIt->vbreeder.beta;
		sumsqBeta += groupStatsIt->vbreeder.beta*groupStatsIt->vbreeder.beta;

		sumDriftB += groupStatsIt->vbreeder.drift;

		sumDriftBH += groupStatsIt->productDriftHB;
		sumDriftBB += groupStatsIt->productDriftBB;

		if (groupStatsIt->relatedness != NO_VALUE) {
			sumRelatedness += groupStatsIt->relatedness;
			sumsqRelatedness += groupStatsIt->relatedness*groupStatsIt->relatedness;
		}
		else {
			exludedGroups++; //prevent to calculate mean of relatedness for empty groups
		}

		sumprodAlphaBeta += groupStatsIt->vbreeder.alpha*groupStatsIt->vbreeder.beta;
	}

	meanGroupsize = sumGroupSize / MAX_COLONIES;
	meanAlpha = sumAlpha / population; ///population=sumGroupSize so simplify!
	meanBeta = sumBeta / population;
	meanRelatedness = sumRelatedness / (MAX_COLONIES - exludedGroups);
	meanDriftB = sumDriftB / (MAX_COLONIES - emptyGroup); //maxcolon - emptyGroup = number of breeders
	meanDriftH = sumDriftH / totalHelpersPop;
	meanDriftBH = sumDriftBH / (MAX_COLONIES - emptyGroup);
	meanDriftBB = sumDriftBB / (MAX_COLONIES - emptyGroup);

	relatednessGl = (meanDriftBH - meanDriftB * meanDriftH) / (meanDriftBB - meanDriftB * meanDriftB);

	varGroupSize = sumsqGroupSize / MAX_COLONIES - meanGroupsize * meanGroupsize;
	varAlpha = sumsqAlpha / population - meanAlpha * meanAlpha;
	varBeta = sumsqBeta / population - meanBeta * meanBeta;
	varRelatedness = sumsqRelatedness / MAX_COLONIES - meanRelatedness * meanRelatedness;

	// to know if there is a problem, variance cannot be negative
	varGroupSize > 0 ? stdevGroupSize = sqrt(varGroupSize) : stdevGroupSize = 0;
	varAlpha > 0 ? stdevAlpha = sqrt(varAlpha) : stdevAlpha = 0;
	varBeta > 0 ? stdevBeta = sqrt(varBeta) : stdevBeta = 0;
	varRelatedness > 0 ? stdevRelatedness = sqrt(varRelatedness) : stdevRelatedness = 0;

	(stdevAlpha > 0 && stdevBeta > 0) ? corr_AlphaBeta = (sumprodAlphaBeta / population - meanAlpha * meanBeta) / (stdevAlpha*stdevBeta) : corr_AlphaBeta = 0;
	///add correlations between alpha and beta to withing group relatedness
}



/* WRITE PARAMETER SETTINGS TO OUTPUT FILE */
void Printparams()
{
	fout << endl << "PARAMETER VALUES" << endl

		<< "Initial population: " << "\t" << MAX_COLONIES * (INIT_NUM_HELPERS + 1) << endl
		<< "Number of colonies: " << "\t" << MAX_COLONIES << endl
		<< "Number generations: " << "\t" << NUM_GENERATIONS << endl
		<< "Predation: " << "\t" << PREDATION << endl
		<< "initAlpha: " << "\t" << setprecision(4) << INIT_ALPHA << endl
		<< "initBeta: " << "\t" << setprecision(4) << INIT_BETA << endl
		<< "mutAlpha: " << "\t" << setprecision(4) << MUTATION_ALPHA << endl
		<< "stepAlpha: " << "\t" << setprecision(4) << STEP_ALPHA << endl
		<< "mutBeta: " << "\t" << setprecision(4) << MUTATION_BETA << endl
		<< "stepBeta: " << "\t" << setprecision(4) << STEP_BETA << endl
		<< "mutDrift: " << "\t" << setprecision(4) << MUTATION_DRIFT << endl
		<< "stepDrift: " << "\t" << setprecision(4) << STEP_DRIFT << endl
		<< "K0: " << "\t" << K0 << endl
		<< "K1: " << "\t" << K1 << endl
		<< "Xsh: " << "\t" << Xsh << endl
		<< "Xsn: " << "\t" << Xsn << endl;

}



/* WRITE GENETIC TRAIT VALUES TO OUTPUT FILE */
void WriteMeans()
{

	// show values on screen
	cout << setw(6) << gen
		<< setw(9) << population
		<< setw(9) << deaths
		<< setw(9) << floatersgenerated
		<< setw(9) << meanGroupsize
		<< setw(9) << maxGroupSize
		<< setw(9) << setprecision(4) << meanAlpha
		<< setw(9) << setprecision(4) << meanBeta
		<< setw(9) << setprecision(4) << meanRelatedness
		<< setw(9) << setprecision(4) << relatednessGl
		<< endl;


	// write values to output file
	fout << gen
		<< "\t" << population
		<< "\t" << deaths
		<< "\t" << meanGroupsize
		<< "\t" << setprecision(4) << meanAlpha
		<< "\t" << setprecision(4) << meanBeta
		<< "\t" << setprecision(4) << meanRelatedness
		<< "\t" << setprecision(4) << relatednessGl
		<< "\t" << setprecision(4) << stdevGroupSize
		<< "\t" << setprecision(4) << stdevAlpha
		<< "\t" << setprecision(4) << stdevBeta
		<< "\t" << setprecision(4) << stdevRelatedness
		<< "\t" << setprecision(4) << corr_AlphaBeta
		<< endl;
}
void wait_for_return()
{
	std::cout << "Hit <Enter> to continue\n";
	getchar();
}

/* MAIN PROGRAM */
int main() {

	for (int rep = 0; rep < NUM_REPLICATES; rep++) {

		gen = 0;

		// Population statistics
		meanGroupsize = 0.0, stdevGroupSize = 0.0, maxGroupSize = 0.0, sumGroupSize = 0.0, sumsqGroupSize = 0.0, varGroupSize = 0.0,
			meanAlpha = 0.0, stdevAlpha = 0.0, sumAlpha = 0.0, sumsqAlpha = 0.0, varAlpha = 0.0,
			meanBeta = 0.0, stdevBeta = 0.0, sumBeta = 0.0, sumsqBeta = 0.0, varBeta = 0.0,
			meanDriftB = 0.0, sumDriftB = 0.0, meanDriftH = 0.0, sumDriftH = 0.0,
			meanDriftBH = 0.0, meanDriftBB = 0.0, sumDriftBH = 0.0, sumDriftBB = 0.0, productDriftHB = 0.0, productDriftBB = 0.0,
			meanRelatedness = 0.0, stdevRelatedness = 0.0, sumRelatedness = 0.0, sumsqRelatedness = 0.0, varRelatedness = 0.0,
			corr_AlphaBeta = 0.0, sumprodAlphaBeta = 0.0;


		// column headings on screen
		cout << setw(6) << "gen" << setw(9) << "pop" << setw(9) << "deaths" << setw(9)
			<< "float" << setw(9) << "group" << setw(9) << "maxGroup" << setw(9)
			<< "alpha" << setw(9) << "beta" << setw(9) << "relat" << setw(9) << "relatGl" << endl;

		// column headings in output file
		fout << "Generation" << "\t" << "Population" << "\t" << "Deaths" << "\t"
			<< "Group_size" << "\t" << "meanAlpha" << "\t" << "meanBeta" << "\t" << "meanRelatedness" << "\t" << "RelatednessGl" << "\t"
			<< "SD_GroupSize" << "\t" << "SD_Alpha" << "\t" << "SD_Beta" << "\t" << "SD_Relatedness"
			<< "\t" << "corr_AB" << "\t" << endl;

		vector<Individual> vfloaters;
		vector<Group> vgroups(MAX_COLONIES);

		InitGroup(vgroups);
	
		for (vector<Group>::iterator relatednessIt = vgroups.begin(); relatednessIt < vgroups.end(); ++relatednessIt) {
			relatednessIt->ProductDrift();
			population += relatednessIt->TotalPopulation(); //calculate number of ind in the whole population
		}

		Statistics(vgroups);
		WriteMeans();

		for (gen = 1; gen <= NUM_GENERATIONS; gen++)
		{
			//cout << "\t" << "\t" << "\t" << "\t" << "\t" << "GENERATION "<<gen<< " STARTS NOW!!!" <<endl;

			if (gen == 500) {
				wait_for_return();
			}

			deaths = 0; // to keep track of how many individuals die each generation
			population = 0; //total of ind in the whole simulation for the expecific generation
			populationBeforeSurv = 0;
			floatersgenerated = 0;

			// Population statistics
			meanGroupsize = 0.0, stdevGroupSize = 0.0, maxGroupSize = 0.0, sumGroupSize = 0.0, sumsqGroupSize = 0.0, varGroupSize = 0.0,
				meanAlpha = 0.0, stdevAlpha = 0.0, sumAlpha = 0.0, sumsqAlpha = 0.0, varAlpha = 0.0,
				meanBeta = 0.0, stdevBeta = 0.0, sumBeta = 0.0, sumsqBeta = 0.0, varBeta = 0.0,
				meanDriftB = 0.0, sumDriftB = 0.0, meanDriftH = 0.0, sumDriftH = 0.0,
				meanDriftBH = 0.0, meanDriftBB = 0.0, sumDriftBH = 0.0, sumDriftBB = 0.0, productDriftHB = 0.0, productDriftBB = 0.0,
				meanRelatedness = 0.0, stdevRelatedness = 0.0, sumRelatedness = 0.0, sumsqRelatedness = 0.0, varRelatedness = 0.0,
				corr_AlphaBeta = 0.0, sumprodAlphaBeta = 0.0;

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
					itBreeder->Breeder(vfloaters);
				}
			}

			//        cout << "Floaters before reassign: " << vfloaters.size() << endl;
			Reassign(vfloaters, vgroups);
			if (!vfloaters.empty()) {
				cout << "Not all floaters were reassigned!" << endl;
			}


			//        cout << "Floaters after reassign: " << vfloaters.size() << endl;

			for (vector<Group>::iterator itAgeRelatedness = vgroups.begin(); itAgeRelatedness < vgroups.end(); ++itAgeRelatedness)
			{
				itAgeRelatedness->IncreaseAge(); //add 1 rank or age to all individuals alive
				itAgeRelatedness->Relatedness();
				itAgeRelatedness->ProductDrift();

				population += itAgeRelatedness->TotalPopulation(); //calculate number of ind in the whole population
			}

			if (gen%SKIP == 0) {   // write output every 'skip' generations
				Statistics(vgroups);
				WriteMeans();
			}

			for (vector<Group>::iterator itReproduction = vgroups.begin(); itReproduction < vgroups.end(); ++itReproduction)
			{
				itReproduction->Fecundity();
				itReproduction->Reproduction();
			}
		}

		fout << endl << endl << endl;

	}
	Printparams();

	wait_for_return();

	return 0;
}
