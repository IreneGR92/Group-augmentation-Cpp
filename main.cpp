/***********************************************
 GROUP AUGMENTATION BASIC MODEL

 Level of help and dispersal with genetic bases. No reaction norms.
***********************************************/

//HEADER FILES

#include <cstdlib>
#include <stdio.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <vector>
#include "H:\PhD\CODE\random.h"

using namespace std;

ofstream fout("group augmentation.txt");     // output file
/*
// random numbers
mt19937 mt(time(0)); // random number generator
uniform_real_distribution<double> Uniform(0, 1); // real number between 0 and 1 (uniform)
normal_distribution<double> DriftNormal(0, 10);
*/

/*CONSTANTS AND STRUCTURES*/

SetSeed(0); ///change for each simulation

    //Fix values
const double m         = 0.8;       // predation pressure
//const double Pd        = 0.5;       // propensity to disperse

// Modifiers (I might need to make them evolve too)
//const double X0r    = 1; // inflexion point in the level of help formula for the influence of rank
//const double X0n    = 1; // inflexion point in the level of help formula for the influence of group size
const double K0     = 1; // min fecundity, fecundity when no help provided.
const double Xsh    = 1; // cost of help in survival
const double Xsr    = 1; // benefit of rank in survival
const double Xss    = 1; // benefit of group size in survival



//Genetic values
const double initAlpha    = 0.0;     // starting value of alpha (in gen 0)
const double mutAlpha     = 0.05;    // mutation rate in alpha for level of help
const double stepAlpha    = 0.4;     // mutation step size in alpha for level of help
const double initBeta     = 0.0;     // starting value of beta (in gen 0)
const double mutBeta      = 0.05;    // mutation rate in beta for the propensity to disperse
const double stepBeta     = 0.4;     // mutation step size in beta for the propensity to disperse
const double mutDrift     = 0.05;    // mutation rate in the neutral genetic value to track level of relatedness
const double stepDrift    = 0.4;     // mutation step size in the neutral genetic value to track level of relatedness

const int minsurv     = 50;     // min number of individual that survive



const double avFloatersSample = 10; //

enum classes {breeder, helper, floater};                  /// problem since helpers and floaters are vectors

const int maxcolon     = 50;     // max number of groups or colonies --> breeding spots. Whole population size = maxcolon * (numhelp + 1)
const int numhelp      = 2;       //number of helpers per group when initializing group

const int NumGen       = 1000;   // number of generations
const int numrep       = 20;     // number of replicates
const int skip         = 50;     // interval between print-outs

//Statistics and others
                                ///Check. Why declare here the statistic variables and not in the Statistics function?
int FloatersSample,gen,rep,killedInd,totalPop;
double  meanAlpha,stdevAlpha,meanBeta,stdevBeta, meanDrift, stdevDrift, corr_AlphaBeta,
        meanGroupsize, stdevGroupsize;         // population means, standard deviations and correlations




//Structures

struct Individual // define individual traits
{
	Individual(double alpha_,double beta_, classes own_);
	Individual(const Individual &mother);
    double alpha, beta, drift,                        // genetic values
            help, survival, fecundity;                /// phenotypic values. For fecundity, is it ok to have inside ind fecundity and inside group realfecundity?
    classes own;                                      // possible classes: breeder, helper, floater
    int ranks, realfecundity;

    //Functions inside Individual
    void calcHelp ();
    void calcSurvival(int vhelpersize);
    void Mutate();

};

Individual::Individual(double alpha_=0,double beta_=0, classes own_=helper){
	alpha=alpha_;
	beta=beta_;
	drift=Normal(0,10);
	own=own_;
	ranks = 1;
	survival = -1;      //out of range, so check later for errors
	fecundity = -1;     //out of range, so check later for errors
	help = -1;          //out of range, so check later for errors
}

Individual::Individual(const Individual &mother){
	alpha=mother.alpha;
	beta=mother.beta;
	drift=mother.drift;
	Mutate();
	own=helper;
	ranks = 1;
	survival = -1;      //out of range, so check later for errors
	fecundity = -1;     //out of range, so check later for errors
	help = -1;          //out of range, so check later for errors
}


struct Group // define group traits
{
    Group(double alpha_,double beta_,int numhelp_);
    double cumhelp;
    bool breederalive;                                     // for the breeder: 1 alive, 0 dead

    Individual vbreeder; // create breeder inside group
    vector<Individual> vhelpers; // create a vector of helpers inside each group

//Functions inside Individual
    void Dispersal(vector<Individual> &floaters);
    void Help();
    void Survival(vector<Individual> &floaters, vector<Group> population, int &deaths);
    void Breeder(vector<Individual> &floaters);
    void Fecundity();
    void Reproduction();
};

Group::	Group(double alpha_=0,double beta_=0,int numhelp_=2)
{
   	vbreeder = Individual(initAlpha,initBeta,breeder);
    breederalive=1;
   	for(int i=0;i<numhelp_;++i)
    {
    	vhelpers.push_back(Individual(initAlpha,initBeta,helper));
	}
}


/* INITIALISE POPULATION */
void InitGroup(vector<Group> &population)
{
    int i;
    for (i=0; i<maxcolon; i++)
    {
		population.push_back(Group(initAlpha,initBeta,numhelp));
    }
}


/* BECOME FLOATER (STAY VS DISPERSE) */
void Group::Dispersal(vector<Individual> &floaters)
{
	vector<Individual>::iterator dispersalIt = vhelpers.begin();
	int size = vhelpers.size();
	int count = 0;
	while (!vhelpers.empty() && size>count)
	{
		if (Uniform() < dispersalIt->beta) // beta is equal to dispersal propensity
		{
			dispersalIt->own=floater;
			floaters.push_back(*dispersalIt); //add the individual to the vector floaters in the last position
			*dispersalIt = vhelpers[vhelpers.size() - 1]; // this and next line removes the individual from the helpers vector
			vhelpers.pop_back();
			dispersalIt->help=0;
			++count;
		}
		else
			++dispersalIt, ++count;
	}
}

/*DISPLAY LEVEL OF HELP*/
void Individual::calcHelp ()
{
    help = alpha;
    //help = 1 / (1 + exp (alpha * (ranks - X0r) - beta * (Group.size - X0n)); //later on it will need to be inside group
}


void Group::Help () //Calculate accumulative help of all individuals inside of each group.
{
	cumhelp = 0;
//Level of help for helpers
    for (vector<Individual>::iterator helpIt = vhelpers.begin(); helpIt < vhelpers.end(); ++helpIt)
    {
		helpIt->calcHelp();
		cumhelp+= helpIt->help;
    }
//Level of help for breeders
	vbreeder.calcHelp();
	cumhelp += vbreeder.help;
}


void Individual::calcSurvival(int vhelpersize=0)
{
        survival = m / (1 + exp (Xsh*help - Xsr*ranks - Xss*(vhelpersize + 1))); // +1 to know group size (1 breeder + helpers)
}


/*SURVIVAL*/
void Group::Survival(vector<Individual> &floaters, vector<Group> population, int &deaths)
{

    //Calculate the survival of the helpers

    vector<Individual>::iterator survHIt = vhelpers.begin();
    int size = vhelpers.size();
    int count = 0;
    while (!vhelpers.empty() && size>count)
    {
        //Calculate value of survival
        survHIt->calcSurvival(vhelpers.size());
        //Mortality
        if (Uniform() > survHIt->survival)
        {
            *survHIt = vhelpers[vhelpers.size() - 1];
            vhelpers.pop_back();
            ++count;
            deaths++; ///set to 0 for each generation, init in main, and print out
        }
        else
        ++survHIt, ++count; //go to next individual
    }


    //Calculate the survival of the floaters

    vector<Individual>::iterator survFIt = floaters.begin();
    size = floaters.size();
    count = 0;
    while (!floaters.empty() && size>count)
    {
        //Calculate value of survival
        survFIt->calcSurvival(floaters.size());
        //Mortality
        if (Uniform() > survFIt->survival)
        {
            *survFIt = floaters[floaters.size() - 1];
            floaters.pop_back();
            ++count;
            deaths++; ///set to 0 for each generation, init in main, and print out
        }
        else
        ++survFIt, ++count; //go to next individual
    }

    //Calculate the survival of the breeder
    vbreeder.calcSurvival(vhelpers.size());
    if (Uniform() > vbreeder.survival)
    {
        breederalive=0;
    }
}


/* BECOME BREEDER */

void Group::Breeder(vector<Individual> &floaters)
{
    double randomposition = Uniform();///check which random numbers to use

    //Select a random sample from the floaters
        int i=0;
        int sumrank=0;
        int currentposition=0;
        poisson_distribution<int> PoissonFloat(avFloatersSample); //random sample size of floaters taken to compete for breeding spot
        uniform_real_distribution<int> UniformFloat(0, floaters.size()); //random floater ID taken in the sample
        int RandN = PoissonFloat;
        //vector<int>vRanks(RandN);
        //vector<int>vIndex(RandN);
        vector<Individual> *Candidates(RandN); //pointer vector
        vector<double>position;
        while (i < RandN)
        {
            //vIndex[i] = RandF;                      // save the address of the floater sampled
            //vRanks[i] = floaters[RandF].ranks;      // save the rank of the floater sampled
            Candidates[i]= &floaters[UniformFloat]
            i++
        }


    //Join the helpers in the group to the sample of floaters

        for (vector<Individual>::iterator helpIt = vhelpers.begin(); helpIt < vhelpers.end(); ++helpIt)
        {
            Candidates.push_back(&(*helpIt));
            //vRanks.push_back(*helpIt.ranks);
            //vIndex.push_back(*helpIt);          /// not sure this is saving the ID
        }
    //Choose breeder with higher likelihood for higher rank
        for (vector<Individual>::iterator *rankIt = Candidates.begin(); rankIt < Candidates.end(); ++rankIt) //*rankIt creates a vector of pointers to an individual
        {
            sumrank += *rankIt->ranks;
        }
        for (vector<Individual>::iterator *rank2It = Candidates.begin(); rank2It < Candidates.end(); ++rank2It)
        {
            position.push_back(*rank2It->ranks / sumrank + currentposition);
            currentposition = *position.end();
        }
        for (vector<Individual>::iterator *rank3It = Candidates.begin(); rank3It < Candidates.end(); ++rank3It)
        {
            if (randomposition < position[rank3It-Candidates.begin()])
            {
                vbreeder = **rank3It;
                breederalive = 1;

                if (*rank3It->own == floater)
                {
                    *rank3It = floaters[floaters.size() - 1];
                    floaters.pop_back();
                }

                else
                {
                    *rank3It = vhelpers[vhelpers.size() - 1];
                    vhelpers.pop_back();
                }

                vbreeder.own = breeder;

            }
        }
    }
}

/*REASSIGN FLOATERS*/

void Reassign(vector<Individual> &floaters, vector<Individual> &helpers) ///not inside either ind or group?
{
    uniform_real_distribution<int> UniformMaxCol(0, maxcolon);

    vector<Individual>::iterator indIt = floaters.begin();
    while (!floaters.empty())
    {
        Group[UniformMaxCol]->helpers.push_back(*indIt) //add the floater to the helper vector in a randomly selected group
        floaters.pop_back(); //remove the floater from its vector
    }
}


/* REPRODUCTION */

void Group::Fecundity()

{
    fecundity = K0 + Group.cumhelp;
    poisson_distribution<int> PoissonFec(fecundity);
    realfecundity = PoissonFec;

}

void Group::Reproduction(int realfecundity)
        // populate offspring generation
{
    int i;
    for (vector<Group>::iterator repIt = population.begin(); repIt < population.end; ++repIt)
    {
        for (i=0;i<realfecundity;i++) //number of offspring dependent on real fecundity
        {
        helpers.push_back(Individual(breeder)); //create a new individual as helper in the group. Call construct to assign the mother genetic values to the offspring, construct calls Mutate function.
        }
    }
}

void Individual::Mutate() // mutate genome of offspring /// first assign the mother values to the offspring
{
    if (Uniform(mt)<mutAlpha)
        Uniform(mt)<0.5? Individual.alpha-=Uniform(mt)*stepAlpha : Individual.alpha+=Uniform(mt)*stepAlpha;

    if (Uniform(mt)<mutBeta)
        Uniform(mt)<0.5? Individual.beta-=Uniform(mt)*stepBeta : Individual.beta+=Uniform(mt)*stepBeta;

    if (Uniform(mt)<mutDrift)
        Uniform(mt)<0.5? Individual.drift-=Uniform(mt)*stepDrift : Individual.drift+=Uniform(mt)*stepDrift;
}


/* CALCULATE STATISTICS */
void Statistics()
{
    //int i, j;
	double alpha,beta,drift,sumAlpha=0.0,sumBeta=0.0,sumDrift=0.0, sumsqAlpha=0.0,sumsqBeta=0.0,sumsqDrift=0.0,
        sumprodAlphaBeta=0.0,varAlpha,varBeta,varDrift;

	for (i=0;i<population.size;i++)
	{
		alpha=Individual[i].alpha;
		beta=Individual[i].beta;
		drift=Individual[i].drift;

        sumAlpha+=alpha;
		sumsqAlpha+=alpha*alpha;

		sumBeta+=beta;
		sumsqBeta+=beta*beta;

		sumDrift+=drift;
		sumsqDrift+=drift*drift;

		sumprodAlphaBeta+=alpha*beta;

   	}

    meanAlpha=sumAlpha/double(population.size);
    meanBeta=sumBeta/double(population.size);
    meanDrift=sumDrift/double(population.size); ///differentiate between within group and global

	varAlpha=sumsqAlpha/double(population.size)-meanAlpha*meanAlpha;
	varBeta=sumsqBeta/double(population.size)-meanBeta*meanBeta;
	varDrift=sumsqDrift/double(population.size)-meanDrift*meanDrift; ///differentiate between within group and global

// to know if there is a problem, variance cannot be negative
	varAlpha>0? stdevAlpha=sqrt(varAlpha):stdevAlpha=0;
	varBeta>0? stdevBeta=sqrt(varBeta):stdevBeta=0;
	varDrift>0? stdevDrift=sqrt(varDrift):stdevDrift=0; ///differentiate between within group and global

	(stdevAlpha>0 && stdevBeta>0)? corr_AlphaBeta=(sumprodAlphaBeta/double(popsize)-meanAlpha*meanBeta)/(stdevAlpha*stdevBeta):corr_AlphaBeta=0; ///add correlations between alpha and beta to withing group relatedness


}



/* WRITE PARAMETER SETTINGS TO OUTPUT FILE */
void Printparams()
{
  fout << endl << "PARAMETER VALUES" << endl

       << "initAlpha: " << "\t" << setprecision(4) << initAlpha << endl
       << "initBeta: " << "\t" << setprecision(4) << initBeta << endl
       << "initDrift: " << "\t" << setprecision(4) << initDrift << endl
       << "mutAlpha: " << "\t" << setprecision(4) << mutAlpha << endl
       << "stepAlpha: " << "\t" << setprecision(4) << stepAlpha << endl
       << "mutBeta: " << "\t" << setprecision(4) << mutBeta << endl
       << "stepBeta: " << "\t" << setprecision(4) << stepBeta << endl
       << "mutDrift: " << "\t" << setprecision(4) << mutDrift << endl
       << "stepDrift: " << "\t" << setprecision(4) << stepDrift << endl
       << "m: " << "\t" << m << endl
       << "K0: " << "\t" << K0 << endl
       << "Xsh: " << "\t" << Xsh << endl
       << "Xsr: " << "\t" << Xsr << endl
       << "Xss: " << "\t" << Xss << endl
       << "maxcolon: " << "\t" << maxcolon << endl
       << "initN: " << "\t" << initN << endl
       << "NumGen: " << "\t" << NumGen << endl;
}



/* WRITE GENETIC TRAIT VALUES TO OUTPUT FILE */
void WriteMeans()
{

  // show values on screen
  cout << setw(6) << gen
       << setw(9) << population.size            ///correct??
	   << setw(9) << setprecision(4) << meanAlpha
	   << setw(9) << setprecision(4) << meanBeta
	   << setw(9) << setprecision(4) << meanDrift
	   << endl;


  // write values to output file
  fout << gen
       << "\t" << population.size
	   << "\t" << setprecision(4) << meanAlpha
	   << "\t" << setprecision(4) << meanBeta
	   << "\t" << setprecision(4) << meanDrift
	   << "\t" << setprecision(4) << stdevAlpha
	   << "\t" << setprecision(4) << stdevBeta
	   << "\t" << setprecision(4) << stdevDrift
	   << "\t" << setprecision(4) << corr_AB
	   << endl;
}


/* MAIN PROGRAM */
int main()
{

	vector<Group> population (maxcolon);
	vector<Individual> floaters;

    for(rep=0;rep<numrep;rep++)
    {

    gen=0; // generation zero


    // column headings on screen
    cout << setw(6) << "gen" << setw(9) << "popsize" << setw(9) << "alpha" << setw(9) << "beta" << setw(9) << "drift" << endl;

	// column headings in output file
	fout << "generation" << "\t" << "popsize" << "\t"  <<  "meanAlpha" << "\t" << "meanBeta" << "\t" << "meanDrift" << "\t"
         << "stdevAlpha" << "\t" << "stdevBeta" << "\t" << "stdevDrift" << "\t" << "corr_AB" << "\t" << endl;


	InitGroup(vector<Group> &population)
	Statistics();
	WriteMeans();

	for (gen=1;gen<=NumGen;gen++)
	{
        int deaths=0; // to keep track of how many individuals die each generation
        /// include functions here? how can we call to go throw all the groups in the population?
		if (gen%skip==0){ Statistics(); WriteMeans();} // write output every 'skip' generations
	}

    fout << endl << endl << endl;

    }
    Printparams();

	return 0;
}
